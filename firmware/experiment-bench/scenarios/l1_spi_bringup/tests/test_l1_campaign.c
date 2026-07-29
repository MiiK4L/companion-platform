// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Tests de la DEFINITION de campagne EXP-L1-BRINGUP-001. Ils verifient que les
// profils sont bien declaratifs et REJOUABLES, et que la table des cas est
// coherente (2 topologies x 3 modes, sources de charge actives/inactives selon
// le mode). AUCUNE execution, aucune mesure : rien n'est acquis ici.
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "profiles/profile.h"
#include "scenarios/l1_spi_bringup/l1_profile.h"
#include "scenarios/l1_spi_bringup/l1_scenario.h"
#include "tests/test_support.h"

int g_test_failures = 0;

static void test_profiles_declaratifs(void) {
  // Ecran : rafales larges, aucune IRQ montante, aucune faute injectee.
  EXPECT(strcmp(l1_screen_profile.profile_id, "l1-screen") == 0, "id du profil ecran");
  EXPECT_EQ_INT(l1_screen_profile.packet_size, 256, "ecran : rafale large");
  EXPECT_EQ_INT(l1_screen_profile.irq_policy, BENCH_IRQ_NEVER, "ecran : pas d'IRQ");
  EXPECT_EQ_INT(l1_screen_profile.payload_pattern, BENCH_PAYLOAD_INCREMENT,
                "ecran : motif incremental");

  // Module : echanges courts, IRQ par transaction.
  EXPECT(strcmp(l1_module_profile.profile_id, "l1-module") == 0, "id du profil module");
  EXPECT_EQ_INT(l1_module_profile.packet_size, 32, "module : echanges courts");
  EXPECT_EQ_INT(l1_module_profile.irq_policy, BENCH_IRQ_PER_TRANSACTION,
                "module : IRQ par transaction");

  // La campagne mesure un comportement NOMINAL : aucune faute injectee.
  EXPECT_EQ_INT(l1_screen_profile.fault_crc_every, 0, "ecran : aucune faute CRC");
  EXPECT_EQ_INT(l1_screen_profile.fault_timeout_every, 0, "ecran : aucun timeout force");
  EXPECT_EQ_INT(l1_module_profile.fault_crc_every, 0, "module : aucune faute CRC");
  EXPECT_EQ_INT(l1_module_profile.fault_timeout_every, 0, "module : aucun timeout force");

  // Parametres de charge renseignes (pas de valeur par defaut oubliee).
  EXPECT(l1_screen_profile.spi_clock_hz > 0, "ecran : frequence SPI declaree");
  EXPECT(l1_module_profile.spi_clock_hz > 0, "module : frequence SPI declaree");
  EXPECT(l1_screen_profile.timeout_ticks > 0, "ecran : timeout declare");
  EXPECT(l1_module_profile.timeout_ticks > 0, "module : timeout declare");
  EXPECT(l1_screen_profile.transaction_count > 0, "ecran : budget de transactions");
  EXPECT(l1_module_profile.transaction_count > 0, "module : budget de transactions");
}

static void test_rejouabilite(void) {
  // Meme seed -> meme suite (rejouabilite exacte).
  uint64_t a = 0;
  uint64_t b = 0;
  bench_profile_seed(&l1_module_profile, &a);
  bench_profile_seed(&l1_module_profile, &b);
  for (int i = 0; i < 8; i++) {
    EXPECT_EQ_INT(bench_profile_next(&a), bench_profile_next(&b),
                  "module : suite rejouable");
  }

  // Vecteurs GOLDEN : ancrent le flux de payload contre toute derive silencieuse.
  const uint8_t golden_module[4] = {0xD5, 0xA3, 0xFC, 0x75};
  uint8_t buf[4] = {0};
  uint64_t s = 0;
  bench_profile_seed(&l1_module_profile, &s);
  bench_profile_fill_payload(&l1_module_profile, buf, 4, &s);
  EXPECT(memcmp(buf, golden_module, 4) == 0, "module : payload golden (seed 0x0BADC0DE)");

  // Motif incremental de l'ecran : deterministe et independant de la seed.
  uint8_t sbuf[4] = {0};
  uint64_t ss = 0;
  bench_profile_seed(&l1_screen_profile, &ss);
  bench_profile_fill_payload(&l1_screen_profile, sbuf, 4, &ss);
  EXPECT(sbuf[0] == 0 && sbuf[1] == 1 && sbuf[2] == 2 && sbuf[3] == 3,
         "ecran : payload incremental deterministe");
}

static void test_table_des_cas(void) {
  EXPECT_EQ_INT(L1_CASE_COUNT, 6, "6 cas = 2 topologies x 3 modes");

  unsigned shared = 0;
  unsigned separated = 0;
  unsigned per_mode[3] = {0, 0, 0};

  for (unsigned i = 0; i < L1_CASE_COUNT; i++) {
    const l1_case_t *c = &l1_cases[i];
    EXPECT(c->case_id != NULL && c->variant_id != NULL, "cas identifie");

    if (c->topology == L1_TOPOLOGY_SPI_SHARED) {
      shared++;
      EXPECT(strcmp(c->variant_id, "spi-shared") == 0, "variant_id coherent (shared)");
    } else {
      separated++;
      EXPECT(strcmp(c->variant_id, "spi-separated") == 0,
             "variant_id coherent (separated)");
    }
    per_mode[c->mode]++;

    // Sources de charge actives/inactives selon le MODE (controle).
    switch (c->mode) {
      case L1_MODE_SCREEN_ONLY:
        EXPECT(c->screen != NULL, "screen-only : ecran actif");
        EXPECT(c->module == NULL, "screen-only : simulateur inactif");
        break;
      case L1_MODE_MODULE_ONLY:
        EXPECT(c->screen == NULL, "module-only : ecran sans trafic");
        EXPECT(c->module != NULL, "module-only : simulateur actif");
        break;
      case L1_MODE_CONCURRENT:
        EXPECT(c->screen != NULL && c->module != NULL,
               "concurrent : les deux sources actives");
        break;
    }
  }

  EXPECT_EQ_INT(shared, 3, "3 cas spi-shared");
  EXPECT_EQ_INT(separated, 3, "3 cas spi-separated");
  EXPECT_EQ_INT(per_mode[L1_MODE_SCREEN_ONLY], 2, "screen-only sur les 2 topologies");
  EXPECT_EQ_INT(per_mode[L1_MODE_MODULE_ONLY], 2, "module-only sur les 2 topologies");
  EXPECT_EQ_INT(per_mode[L1_MODE_CONCURRENT], 2, "concurrent sur les 2 topologies");

  // Les deux topologies partagent EXACTEMENT les memes profils : seule la
  // topologie change, sinon la comparaison ne serait pas controlee.
  EXPECT(l1_cases[0].screen == l1_cases[3].screen, "meme profil ecran des deux cotes");
  EXPECT(l1_cases[1].module == l1_cases[4].module, "meme profil module des deux cotes");
}

int main(void) {
  test_profiles_declaratifs();
  test_rejouabilite();
  test_table_des_cas();
  if (g_test_failures == 0) {
    printf("OK: definition de campagne EXP-L1-BRINGUP-001\n");
    return 0;
  }
  printf("ECHEC: %d test(s)\n", g_test_failures);
  return 1;
}

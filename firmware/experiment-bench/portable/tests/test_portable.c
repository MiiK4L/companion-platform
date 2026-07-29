// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Tests unitaires du coeur portable : CRC, echeances, profils (determinisme +
// fautes), compteurs bruts vs derives, codec du protocole, machine d'etat SPI
// (nominal + timeout). Aucune dependance plateforme.
#include <string.h>

#include "counters/counters.h"
#include "crc/crc.h"
#include "profiles/profile.h"
#include "protocol/protocol.h"
#include "scheduler/scheduler.h"
#include "tests/test_support.h"
#include "transport/transport.h"

int g_test_failures = 0;

static void test_crc_stable_and_sensitive(void) {
  const uint8_t a[] = {1, 2, 3, 4};
  const uint8_t b[] = {1, 2, 3, 5};
  EXPECT_EQ_INT(bench_crc32(a, 4), bench_crc32(a, 4), "crc stable");
  EXPECT(bench_crc32(a, 4) != bench_crc32(b, 4), "crc sensible au contenu");
}

static void test_deadline_and_timeout(void) {
  bench_deadline_t d = bench_deadline_make(100, 10);
  EXPECT(!bench_timeout_expired(d, 109), "pas encore expire");
  EXPECT(bench_timeout_expired(d, 110), "expire a l'echeance");
}

static void test_profile_deterministic(void) {
  const bench_profile_t p = {"demo", 1, 42, 4, 8, 2, 100, 0, 0};
  uint64_t s1 = 0, s2 = 0;
  bench_profile_seed(&p, &s1);
  bench_profile_seed(&p, &s2);
  for (int i = 0; i < 5; i++) {
    EXPECT_EQ_INT(bench_profile_next(&s1), bench_profile_next(&s2),
                  "meme seed -> meme suite");
  }
}

static void test_profile_faults(void) {
  const bench_profile_t p = {"f", 1, 1, 10, 8, 0, 100, 3, 5};
  EXPECT(bench_profile_fault_crc(&p, 2), "faute CRC a l'index 2 (3e tx)");
  EXPECT(!bench_profile_fault_crc(&p, 1), "pas de faute CRC index 1");
  EXPECT(bench_profile_fault_timeout(&p, 4), "timeout a l'index 4 (5e tx)");
}

static void test_counters_raw_vs_derived(void) {
  bench_counters_t c;
  bench_counters_reset(&c);
  bench_counters_record_tx(&c, 1, 100, 10);
  bench_counters_record_tx(&c, 0, 0, 30);
  bench_counters_record_crc_error(&c);
  EXPECT_EQ_INT(c.tx_ok, 1, "brut tx_ok");
  EXPECT_EQ_INT(c.tx_failed, 1, "brut tx_failed");
  EXPECT_EQ_INT(c.crc_errors, 1, "brut crc_errors");
  bench_stats_t s = bench_counters_derive(&c, 200);
  EXPECT(s.latency_mean_ticks == 20.0, "derive : latence moyenne (10+30)/2");
  EXPECT(s.tx_success_ratio == 0.5, "derive : ratio de succes 1/2");
  EXPECT(s.throughput_bytes_per_tick == 0.5, "derive : debit 100/200");
}

static void test_protocol_codec(void) {
  bench_cmd_t start = bench_protocol_parse("START");
  EXPECT_EQ_INT(start.type, BENCH_CMD_START, "parse START");
  bench_cmd_t sel = bench_protocol_parse("SELECT demo-1");
  EXPECT_EQ_INT(sel.type, BENCH_CMD_SELECT_PROFILE, "parse SELECT");
  EXPECT(strcmp(sel.arg, "demo-1") == 0, "argument de SELECT");
  bench_cmd_t bad = bench_protocol_parse("NOPE");
  EXPECT_EQ_INT(bad.type, BENCH_CMD_UNKNOWN, "commande inconnue");

  bench_counters_t c;
  bench_counters_reset(&c);
  c.tx_ok = 3;
  char line[256];
  int n = bench_protocol_format_counters(line, sizeof(line), &c);
  EXPECT(n > 0, "serialisation compteurs");
  EXPECT(strstr(line, "tx_ok=3") != NULL, "compteur serialise");
}

static void test_transport_nominal_and_timeout(void) {
  bench_txn_t txn;
  bench_txn_begin(&txn, BENCH_TRANSPORT_SPI, 10, 0, 100);
  EXPECT_EQ_INT(bench_txn_advance(&txn, 4, 10), BENCH_TXN_ACTIVE, "en cours");
  EXPECT_EQ_INT(bench_txn_advance(&txn, 6, 20), BENCH_TXN_DONE, "termine");
  EXPECT_EQ_INT(bench_txn_latency(&txn, 20), 20, "latence = now - start");

  bench_txn_t slow;
  bench_txn_begin(&slow, BENCH_TRANSPORT_SPI, 10, 0, 5);
  EXPECT_EQ_INT(bench_txn_advance(&slow, 1, 6), BENCH_TXN_TIMEOUT, "timeout");
}

int main(void) {
  test_crc_stable_and_sensitive();
  test_deadline_and_timeout();
  test_profile_deterministic();
  test_profile_faults();
  test_counters_raw_vs_derived();
  test_protocol_codec();
  test_transport_nominal_and_timeout();
  if (g_test_failures == 0) {
    printf("OK: coeur portable (tous les tests)\n");
    return 0;
  }
  printf("ECHEC: %d test(s)\n", g_test_failures);
  return 1;
}

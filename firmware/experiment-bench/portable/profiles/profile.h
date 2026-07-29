// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Profil de trafic 100% DECLARATIF (donnees uniquement, aucun code de scenario).
// Le MOTEUR ne fait qu'EXECUTER ce que le profil decrit ; il ne contient aucun
// parametre de scenario code en dur. Un profil decrit donc TOUT : frequence SPI,
// tailles de trames, delais, politique IRQ, motif de payload, injections de
// fautes et seed de reproductibilite. L'identifiant + la version + la seed
// garantissent la REJOUABILITE exacte.
#ifndef BENCH_PROFILE_H
#define BENCH_PROFILE_H

#include <stddef.h>
#include <stdint.h>

// Motif de payload (declaratif).
typedef enum {
  BENCH_PAYLOAD_SEEDED = 0,    // octets pseudo-aleatoires deterministes (seed)
  BENCH_PAYLOAD_CONSTANT = 1,  // octet constant = payload_fill
  BENCH_PAYLOAD_INCREMENT = 2, // payload_fill, +1, +2, ... (mod 256)
} bench_payload_pattern_t;

// Politique d'emission de l'IRQ par l'esclave (declaratif).
typedef enum {
  BENCH_IRQ_PER_TRANSACTION = 0,  // IRQ apres chaque transaction
  BENCH_IRQ_ON_ERROR = 1,         // IRQ seulement en cas de rejet (CRC/format)
  BENCH_IRQ_NEVER = 2,            // aucune IRQ emise
} bench_irq_policy_t;

typedef struct {
  const char *profile_id;      // identifiant stable
  uint32_t profile_version;    // version du profil
  uint64_t seed;               // graine de reproductibilite
  uint32_t spi_clock_hz;       // frequence SPI declaree (timing realise par l'adaptateur/sim)
  uint32_t transaction_count;  // nombre de transactions
  uint32_t packet_size;        // octets de payload par transaction
  uint32_t inter_delay_ticks;  // delai entre transactions (honore par le moteur)
  uint32_t timeout_ticks;      // budget d'une transaction
  bench_payload_pattern_t payload_pattern;  // motif de remplissage du payload
  uint8_t payload_fill;        // graine du motif constant/incremental
  bench_irq_policy_t irq_policy;  // politique IRQ de l'esclave
  uint32_t fault_crc_every;    // injecte une faute CRC toutes les N tx (0 = aucune)
  uint32_t fault_crc_byte;     // index d'octet corrompu lors d'une faute CRC
  uint32_t fault_timeout_every;  // force un timeout toutes les N tx (0 = aucun)
} bench_profile_t;

// Generateur pseudo-aleatoire DETERMINISTE (xorshift64), portable.
void bench_profile_seed(const bench_profile_t *profile, uint64_t *state);
uint32_t bench_profile_next(uint64_t *state);

// Remplit "buf" (len octets) selon le motif DECLARE par le profil. Pour le motif
// seede, "state" est avance (deterministe). Aucune logique de scenario ailleurs.
void bench_profile_fill_payload(const bench_profile_t *profile, uint8_t *buf,
                                uint32_t len, uint64_t *state);

// Vrai si la transaction d'index i (0-base) doit subir la faute demandee.
int bench_profile_fault_crc(const bench_profile_t *profile, uint32_t index);
int bench_profile_fault_timeout(const bench_profile_t *profile, uint32_t index);

#endif  // BENCH_PROFILE_H

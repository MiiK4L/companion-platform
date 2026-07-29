// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Profil de trafic 100% DECLARATIF (donnees uniquement, aucun code). Un profil
// est une structure POD constante : identifiant, version et seed garantissent la
// REJOUABILITE exacte d'un scenario. Le generateur ci-dessous est deterministe
// (seedé par le profil) et portable.
#ifndef BENCH_PROFILE_H
#define BENCH_PROFILE_H

#include <stdint.h>

typedef struct {
  const char *profile_id;      // identifiant stable
  uint32_t profile_version;    // version du profil
  uint64_t seed;               // graine de reproductibilite
  uint32_t transaction_count;  // nombre de transactions
  uint32_t packet_size;        // octets par transaction
  uint32_t inter_delay_ticks;  // delai entre transactions
  uint32_t timeout_ticks;      // timeout d'une transaction
  uint32_t fault_crc_every;    // injecte une faute CRC toutes les N tx (0 = aucune)
  uint32_t fault_timeout_every; // injecte un timeout toutes les N tx (0 = aucun)
} bench_profile_t;

// Generateur pseudo-aleatoire DETERMINISTE (xorshift64), portable.
void bench_profile_seed(const bench_profile_t *profile, uint64_t *state);
uint32_t bench_profile_next(uint64_t *state);

// Vrai si la transaction d'index i (0-base) doit subir la faute demandee.
int bench_profile_fault_crc(const bench_profile_t *profile, uint32_t index);
int bench_profile_fault_timeout(const bench_profile_t *profile, uint32_t index);

#endif  // BENCH_PROFILE_H

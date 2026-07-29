// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Arithmetique SATURANTE portable. Les compteurs et le transport ne doivent
// JAMAIS deborder silencieusement : une addition qui depasserait la capacite du
// type est plafonnee a la valeur maximale (comportement defini et documente).
#ifndef BENCH_SAT_H
#define BENCH_SAT_H

#include <stdint.h>

// Addition saturante 32 bits (plafonnee a UINT32_MAX).
static inline uint32_t bench_sat_add_u32(uint32_t a, uint32_t b) {
  uint32_t sum = a + b;
  if (sum < a) {  // debordement modulaire detecte
    return UINT32_MAX;
  }
  return sum;
}

// Addition saturante 64 bits (plafonnee a UINT64_MAX).
static inline uint64_t bench_sat_add_u64(uint64_t a, uint64_t b) {
  uint64_t sum = a + b;
  if (sum < a) {
    return UINT64_MAX;
  }
  return sum;
}

// Incremente un compteur 32 bits en saturant.
static inline void bench_sat_inc_u32(uint32_t *c) { *c = bench_sat_add_u32(*c, 1); }

#endif  // BENCH_SAT_H

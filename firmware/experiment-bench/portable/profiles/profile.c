// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "profiles/profile.h"

void bench_profile_seed(const bench_profile_t *profile, uint64_t *state) {
  // Etat initial deterministe derive de la seed (jamais 0 pour xorshift).
  *state = profile->seed ? profile->seed : 0x9E3779B97F4A7C15ull;
}

uint32_t bench_profile_next(uint64_t *state) {
  uint64_t x = *state;
  x ^= x << 13;
  x ^= x >> 7;
  x ^= x << 17;
  *state = x;
  return (uint32_t)(x >> 32);
}

void bench_profile_fill_payload(const bench_profile_t *profile, uint8_t *buf,
                                uint32_t len, uint64_t *state) {
  // Execute le motif DECLARE ; aucune politique n'est decidee par le moteur.
  switch (profile->payload_pattern) {
    case BENCH_PAYLOAD_CONSTANT:
      for (uint32_t i = 0; i < len; i++) {
        buf[i] = profile->payload_fill;
      }
      break;
    case BENCH_PAYLOAD_INCREMENT:
      for (uint32_t i = 0; i < len; i++) {
        buf[i] = (uint8_t)((profile->payload_fill + i) & 0xFFu);
      }
      break;
    case BENCH_PAYLOAD_SEEDED:
    default:
      for (uint32_t i = 0; i < len; i++) {
        buf[i] = (uint8_t)(bench_profile_next(state) & 0xFFu);
      }
      break;
  }
}

int bench_profile_fault_crc(const bench_profile_t *profile, uint32_t index) {
  return profile->fault_crc_every != 0 &&
         ((index + 1) % profile->fault_crc_every) == 0;
}

int bench_profile_fault_timeout(const bench_profile_t *profile, uint32_t index) {
  return profile->fault_timeout_every != 0 &&
         ((index + 1) % profile->fault_timeout_every) == 0;
}

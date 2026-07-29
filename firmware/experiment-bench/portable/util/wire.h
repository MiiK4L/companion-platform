// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Serialisation entiere BORNEE et DETERMINISTE (gros-boutiste / network order).
// Portable, sans dependance : utilisee par le format de trame et le protocole
// pour garantir un encodage identique quelle que soit l'endianness de la cible.
#ifndef BENCH_WIRE_H
#define BENCH_WIRE_H

#include <stddef.h>
#include <stdint.h>

static inline void bench_wire_put_u16(uint8_t *p, uint16_t v) {
  p[0] = (uint8_t)((v >> 8) & 0xFF);
  p[1] = (uint8_t)(v & 0xFF);
}

static inline void bench_wire_put_u32(uint8_t *p, uint32_t v) {
  p[0] = (uint8_t)((v >> 24) & 0xFF);
  p[1] = (uint8_t)((v >> 16) & 0xFF);
  p[2] = (uint8_t)((v >> 8) & 0xFF);
  p[3] = (uint8_t)(v & 0xFF);
}

static inline void bench_wire_put_u64(uint8_t *p, uint64_t v) {
  bench_wire_put_u32(p, (uint32_t)(v >> 32));
  bench_wire_put_u32(p + 4, (uint32_t)(v & 0xFFFFFFFFu));
}

static inline uint16_t bench_wire_get_u16(const uint8_t *p) {
  return (uint16_t)(((uint16_t)p[0] << 8) | (uint16_t)p[1]);
}

static inline uint32_t bench_wire_get_u32(const uint8_t *p) {
  return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
         ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static inline uint64_t bench_wire_get_u64(const uint8_t *p) {
  return ((uint64_t)bench_wire_get_u32(p) << 32) | (uint64_t)bench_wire_get_u32(p + 4);
}

#endif  // BENCH_WIRE_H

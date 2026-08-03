// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "telemetry/ring.h"

#include <string.h>

#include "util/sat.h"

#define BENCH_RING_NO_SEQ 0xFFFFFFFFu

void bench_ring_init(bench_ring_t *ring, bench_sample_t *slots, uint32_t capacity) {
  memset(ring, 0, sizeof(*ring));
  ring->slots = slots;
  ring->capacity = capacity;
  ring->has_last_seq = 0;
}

int bench_ring_push(bench_ring_t *ring, const bench_sample_t *sample) {
  if (ring->capacity == 0 || ring->count >= ring->capacity) {
    // Tampon plein : on PERD l'echantillon plutot que d'attendre le puits.
    // La perte est comptee (saturante) et laissera un marqueur de lacune.
    bench_sat_inc_u32(&ring->producer_drop);
    bench_sat_inc_u32(&ring->pending_gap);
    return 0;
  }
  const uint32_t tail = (ring->head + ring->count) % ring->capacity;
  ring->slots[tail] = *sample;
  ring->count++;
  ring->last_seq = sample->sequence_id;
  ring->has_last_seq = 1;
  return 1;
}

int bench_ring_pop(bench_ring_t *ring, bench_sample_t *out) {
  if (ring->count == 0) {
    return 0;
  }
  *out = ring->slots[ring->head];
  ring->head = (ring->head + 1) % ring->capacity;
  ring->count--;
  return 1;
}

uint32_t bench_ring_take_gap(bench_ring_t *ring, uint32_t *after_seq) {
  const uint32_t lost = ring->pending_gap;
  ring->pending_gap = 0;
  if (after_seq != NULL) {
    *after_seq = ring->has_last_seq ? ring->last_seq : BENCH_RING_NO_SEQ;
  }
  return lost;
}

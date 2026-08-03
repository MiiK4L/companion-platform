// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "telemetry/ring.h"

#include <stddef.h>
#include <string.h>

#include "util/sat.h"

void bench_ring_init(bench_ring_t *ring, bench_sample_t *slots, uint32_t capacity,
                     bench_gap_record_t *gaps, uint32_t gap_capacity) {
  memset(ring, 0, sizeof(*ring));
  ring->slots = slots;
  ring->capacity = capacity;
  ring->gaps = gaps;
  ring->gap_capacity = (gaps != NULL) ? gap_capacity : 0;
  ring->has_last_seq = 0;
}

// Index de la plage la plus recente (derniere inseree).
static bench_gap_record_t *newest_gap(bench_ring_t *ring) {
  if (ring->gap_count == 0) {
    return NULL;
  }
  const uint32_t idx = (ring->gap_head + ring->gap_count - 1) % ring->gap_capacity;
  return &ring->gaps[idx];
}

static void record_drop(bench_ring_t *ring) {
  bench_sat_inc_u32(&ring->producer_drop);
  if (ring->gaps == NULL || ring->gap_capacity == 0) {
    return;  // pertes comptees, mais non localisables faute de place declaree
  }
  // Des pertes consecutives (sans depot accepte entre elles) forment UNE seule
  // plage : meme position chronologique.
  bench_gap_record_t *last = newest_gap(ring);
  if (last != NULL && last->after_pushed_total == ring->pushed_total) {
    last->lost_count = bench_sat_add_u32(last->lost_count, 1);
    return;
  }
  if (ring->gap_count == ring->gap_capacity) {
    // Plus de place : on FUSIONNE dans la plage la plus recente et on le
    // signale. L'information de perte n'est jamais supprimee.
    if (last != NULL) {
      last->lost_count = bench_sat_add_u32(last->lost_count, 1);
    }
    bench_sat_inc_u32(&ring->gap_records_merged);
    return;
  }
  const uint32_t idx = (ring->gap_head + ring->gap_count) % ring->gap_capacity;
  ring->gaps[idx].lost_count = 1;
  ring->gaps[idx].after_seq =
      ring->has_last_seq ? ring->last_seq : BENCH_RING_NO_SEQ;
  ring->gaps[idx].after_pushed_total = ring->pushed_total;
  ring->gap_count++;
}

int bench_ring_push(bench_ring_t *ring, const bench_sample_t *sample) {
  if (ring->capacity == 0 || ring->slots == NULL || ring->count >= ring->capacity) {
    record_drop(ring);
    return 0;
  }
  const uint32_t tail = (ring->head + ring->count) % ring->capacity;
  ring->slots[tail] = *sample;
  ring->count++;
  ring->pushed_total++;
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
  ring->popped_total++;
  return 1;
}

int bench_ring_peek_gap(const bench_ring_t *ring, bench_gap_record_t *out) {
  if (ring->gap_count == 0 || ring->gaps == NULL) {
    return 0;
  }
  const bench_gap_record_t *g = &ring->gaps[ring->gap_head];
  // ECHUE seulement si tous les echantillons qui la precedent sont depiles.
  if (ring->popped_total < g->after_pushed_total) {
    return 0;
  }
  *out = *g;
  return 1;
}

void bench_ring_commit_gap(bench_ring_t *ring) {
  if (ring->gap_count == 0 || ring->gaps == NULL) {
    return;
  }
  ring->gap_head = (ring->gap_head + 1) % ring->gap_capacity;
  ring->gap_count--;
}

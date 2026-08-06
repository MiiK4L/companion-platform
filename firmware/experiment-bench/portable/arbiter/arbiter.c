// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "arbiter/arbiter.h"

#include <stddef.h>
#include <string.h>

#include "scheduler/scheduler.h"  // bench_elapsed (wrap-safe)
#include "util/sat.h"

void bench_arbiter_init(bench_arbiter_t *arb, bench_arb_policy_t policy,
                        bench_bus_request_t *slots, uint32_t capacity,
                        bench_ticks_t starvation_threshold_ticks) {
  memset(arb, 0, sizeof(*arb));
  arb->policy = policy;
  arb->slots = slots;
  arb->capacity = (slots != NULL) ? capacity : 0;
  arb->owner = BENCH_ARB_NO_OWNER;
  arb->last_granted = BENCH_ARB_NO_OWNER;
  arb->starvation_threshold_ticks = starvation_threshold_ticks;
  for (uint32_t i = 0; i < BENCH_MAX_PRODUCERS; i++) {
    arb->priority_of[i] = (uint8_t)i;  // par defaut : id = priorite
  }
}

void bench_arbiter_set_priority(bench_arbiter_t *arb, uint8_t producer_id,
                                uint8_t priority) {
  if (producer_id < BENCH_MAX_PRODUCERS) {
    arb->priority_of[producer_id] = priority;
  }
}

int bench_arbiter_busy(const bench_arbiter_t *arb, bench_ticks_t now) {
  if (arb->owner == BENCH_ARB_NO_OWNER) {
    return 0;
  }
  // ATOMICITE : detenu tant que la transaction n'est pas terminee.
  return bench_elapsed(arb->owned_since, now) <
         bench_elapsed(arb->owned_since, arb->owned_until);
}

uint32_t bench_arbiter_depth(const bench_arbiter_t *arb) { return arb->count; }

bench_ticks_t bench_arbiter_oldest_pending_age(const bench_arbiter_t *arb,
                                               bench_ticks_t now) {
  if (arb->count == 0) {
    return 0;
  }
  // La tete de file est la plus ancienne INSERTION (ordre d'arrivee conserve).
  bench_ticks_t oldest = 0;
  for (uint32_t i = 0; i < arb->count; i++) {
    const bench_bus_request_t *r = &arb->slots[(arb->head + i) % arb->capacity];
    const bench_ticks_t age = bench_elapsed(r->requested_at, now);
    if (age > oldest) {
      oldest = age;
    }
  }
  return oldest;
}

int bench_arbiter_request(bench_arbiter_t *arb, uint8_t producer_id,
                          bench_ticks_t now) {
  if (producer_id >= BENCH_MAX_PRODUCERS) {
    return 0;
  }
  if (arb->capacity == 0 || arb->count >= arb->capacity) {
    // File pleine : debordement COMPTE par producteur, jamais silencieux.
    bench_sat_inc_u32(&arb->overflow_by_producer[producer_id]);
    return 0;
  }
  const uint32_t tail = (arb->head + arb->count) % arb->capacity;
  arb->slots[tail].producer_id = producer_id;
  arb->slots[tail].requested_at = now;
  // Rang d'arrivee GLOBAL et strictement croissant : c'est lui qui rend le
  // tie-break DETERMINISTE quand plusieurs demandes arrivent au meme tick.
  arb->slots[tail].arrival = arb->arrival_counter++;
  arb->count++;
  bench_sat_inc_u32(&arb->depth_by_producer[producer_id]);
  if (arb->count > arb->max_depth_total) {
    arb->max_depth_total = arb->count;
  }
  return 1;
}

// Choisit l'index (relatif a head) du prochain servi selon la politique.
static uint32_t select_index(const bench_arbiter_t *arb) {
  uint32_t best = 0;
  const bench_bus_request_t *b = &arb->slots[arb->head % arb->capacity];

  switch (arb->policy) {
    case BENCH_ARB_FIFO:
      // Ordre d'arrivee STABLE : la tete de file, sans exception.
      return 0;

    case BENCH_ARB_ROUND_ROBIN: {
      // Sert en priorite un producteur DIFFERENT du dernier servi ; a defaut,
      // la tete de file. Un producteur temporairement inactif ne bloque donc
      // jamais le tourniquet, et sa reactivation le replace naturellement.
      for (uint32_t i = 0; i < arb->count; i++) {
        const bench_bus_request_t *r =
            &arb->slots[(arb->head + i) % arb->capacity];
        if (r->producer_id != arb->last_granted) {
          return i;
        }
      }
      return 0;
    }

    case BENCH_ARB_FIXED_PRIORITY:
    default: {
      // Mode de STRESS : priorite la plus forte d'abord ; a priorite egale,
      // rang d'arrivee le plus faible (tie-break stable).
      for (uint32_t i = 1; i < arb->count; i++) {
        const bench_bus_request_t *r =
            &arb->slots[(arb->head + i) % arb->capacity];
        const uint8_t pr = arb->priority_of[r->producer_id];
        const uint8_t pb = arb->priority_of[b->producer_id];
        if (pr < pb || (pr == pb && r->arrival < b->arrival)) {
          b = r;
          best = i;
        }
      }
      return best;
    }
  }
}

// Retire l'element d'index relatif "idx" en preservant l'ordre des autres.
static bench_bus_request_t remove_at(bench_arbiter_t *arb, uint32_t idx) {
  const bench_bus_request_t taken =
      arb->slots[(arb->head + idx) % arb->capacity];
  for (uint32_t i = idx; i + 1 < arb->count; i++) {
    arb->slots[(arb->head + i) % arb->capacity] =
        arb->slots[(arb->head + i + 1) % arb->capacity];
  }
  arb->count--;
  return taken;
}

uint8_t bench_arbiter_grant(bench_arbiter_t *arb, bench_ticks_t now,
                            bench_ticks_t duration, bench_ticks_t *bus_wait_out) {
  if (bus_wait_out != NULL) {
    *bus_wait_out = 0;
  }
  // ATOMICITE : aucune attribution tant que le bus est detenu.
  if (bench_arbiter_busy(arb, now)) {
    return BENCH_ARB_NO_OWNER;
  }
  if (arb->count == 0) {
    return BENCH_ARB_NO_OWNER;
  }

  const uint32_t idx = select_index(arb);
  const bench_bus_request_t req = remove_at(arb, idx);
  const uint8_t pid = req.producer_id;

  const bench_ticks_t wait = bench_elapsed(req.requested_at, now);
  if (bus_wait_out != NULL) {
    *bus_wait_out = wait;
  }
  if (wait > arb->max_bus_wait_ticks[pid]) {
    arb->max_bus_wait_ticks[pid] = wait;
  }
  if (arb->starvation_threshold_ticks > 0 &&
      wait > arb->starvation_threshold_ticks) {
    bench_sat_inc_u32(&arb->requests_over_starvation_threshold[pid]);
  }
  if (arb->depth_by_producer[pid] > 0) {
    arb->depth_by_producer[pid]--;
  }

  arb->owner = pid;
  arb->owned_since = now;
  arb->owned_until = now + duration;
  arb->last_granted = pid;
  return pid;
}

void bench_arbiter_release(bench_arbiter_t *arb, bench_ticks_t now) {
  (void)now;
  arb->owner = BENCH_ARB_NO_OWNER;
}

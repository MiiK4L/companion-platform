// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "concurrent/concurrent.h"

#include <stddef.h>
#include <string.h>

#include "scheduler/scheduler.h"
#include "telemetry/record.h"
#include "util/sat.h"

void bench_concurrent_init(bench_concurrent_t *e, bench_ticks_t occupancy_ticks,
                           bench_conc_sink_t sink, void *sink_ctx) {
  memset(e, 0, sizeof(*e));
  e->bus_occupancy_ticks = occupancy_ticks;
  e->sink = sink;
  e->sink_ctx = sink_ctx;
  e->shared_topology = 0;
  for (uint32_t i = 0; i < BENCH_CONC_MAX_PRODUCERS; i++) {
    bench_counters_reset(&e->counters[i]);
  }
}

void bench_concurrent_add_producer(bench_concurrent_t *e,
                                   const bench_profile_t *profile, int enabled,
                                   bench_arbiter_t *bus) {
  if (e->producer_count >= BENCH_CONC_MAX_PRODUCERS) {
    return;
  }
  const uint8_t id = e->producer_count;
  bench_producer_init(&e->producers[id], id, profile, enabled);
  e->bus_of[id] = bus;
  e->producer_count++;
  // Topologie PARTAGEE si tous les producteurs pointent le meme arbitre.
  e->shared_topology = 1;
  for (uint8_t i = 1; i < e->producer_count; i++) {
    if (e->bus_of[i] != e->bus_of[0]) {
      e->shared_topology = 0;
      break;
    }
  }
}

// Publie un resultat terminal et met a jour les compteurs bruts.
static void emit_result(bench_concurrent_t *e, bench_producer_t *p,
                        uint8_t status, uint8_t cause) {
  bench_conc_result_t r;
  r.producer_id = p->producer_id;
  r.local_sequence = p->local_sequence;
  // L'ORDRE GLOBAL est strictement monotone et n'est jamais reutilise : il
  // capture l'entrelacement reel, que la sequence locale ne peut pas exprimer.
  r.global_order = e->global_order++;
  r.requested_at = p->requested_at;
  r.granted_at = p->granted_at;
  r.released_at = p->released_at;
  r.bus_wait_ticks = p->bus_wait_ticks;
  r.status = status;
  r.timeout_cause = cause;

  const uint8_t id = p->producer_id;
  const bench_ticks_t latency = bench_elapsed(p->requested_at, p->released_at);
  bench_counters_record_tx(&e->counters[id], status == BENCH_SAMPLE_OK,
                           p->profile->packet_size, latency);
  if (status == BENCH_SAMPLE_TIMEOUT) {
    bench_counters_record_timeout(&e->counters[id]);
  }
  e->total_bus_wait[id] = bench_sat_add_u64(e->total_bus_wait[id], p->bus_wait_ticks);
  if (cause < 5) {
    bench_sat_inc_u32(&e->timeout_by_cause[cause]);
  }

  if (e->sink != NULL) {
    e->sink(e->sink_ctx, &r);
  }

  p->local_sequence++;
  p->issued++;
  p->pending = 0;
  p->next_due_at = e->now + p->profile->inter_delay_ticks;
}

// Prochaine echeance : la plus proche parmi les cadences et les liberations.
static int next_deadline(const bench_concurrent_t *e, bench_ticks_t *out) {
  int found = 0;
  bench_ticks_t best = 0;
  for (uint8_t i = 0; i < e->producer_count; i++) {
    const bench_producer_t *p = &e->producers[i];
    if (bench_producer_has_work(p) && !p->pending) {
      if (!found || p->next_due_at < best) {
        best = p->next_due_at;
        found = 1;
      }
    }
  }
  // Fin d'occupation d'un bus detenu.
  for (uint8_t i = 0; i < e->producer_count; i++) {
    const bench_arbiter_t *a = e->bus_of[i];
    if (a != NULL && a->owner != BENCH_ARB_NO_OWNER) {
      if (!found || a->owned_until < best) {
        best = a->owned_until;
        found = 1;
      }
    }
  }
  if (found) {
    *out = best;
  }
  return found;
}

int bench_concurrent_step(bench_concurrent_t *e) {
  // 1) Liberation des bus dont la transaction atomique est terminee.
  for (uint8_t i = 0; i < e->producer_count; i++) {
    bench_arbiter_t *a = e->bus_of[i];
    if (a == NULL || a->owner == BENCH_ARB_NO_OWNER) {
      continue;
    }
    if (!bench_arbiter_busy(a, e->now)) {
      bench_producer_t *p = &e->producers[a->owner];
      p->released_at = e->now;
      bench_arbiter_release(a, e->now);
      // Timeout PERIPHERIQUE si la transaction depasse le budget declare.
      const bench_ticks_t total = bench_elapsed(p->requested_at, p->released_at);
      if (p->profile->timeout_ticks > 0 && total > p->profile->timeout_ticks) {
        emit_result(e, p, BENCH_SAMPLE_TIMEOUT, BENCH_TIMEOUT_PERIPHERAL_RESPONSE);
      } else {
        emit_result(e, p, BENCH_SAMPLE_OK, BENCH_TIMEOUT_NONE);
      }
    }
  }

  // 2) Demandes de bus dues a l'instant courant. L'ordre de balayage fixe le
  //    TIE-BREAK des demandes simultanees : identifiant de producteur croissant,
  //    materialise ensuite par le rang d'arrivee de l'arbitre.
  for (uint8_t i = 0; i < e->producer_count; i++) {
    bench_producer_t *p = &e->producers[i];
    if (!bench_producer_due(p, e->now)) {
      continue;
    }
    p->requested_at = e->now;
    p->bus_wait_ticks = 0;
    if (bench_arbiter_request(e->bus_of[i], p->producer_id, e->now)) {
      p->pending = 1;
    } else {
      // File pleine : la transaction n'a jamais pu etre soumise.
      p->granted_at = e->now;
      p->released_at = e->now;
      emit_result(e, p, BENCH_SAMPLE_TIMEOUT, BENCH_TIMEOUT_SCHEDULER);
    }
  }

  // 3) Attribution du bus (une seule par arbitre : ATOMICITE).
  for (uint8_t i = 0; i < e->producer_count; i++) {
    bench_arbiter_t *a = e->bus_of[i];
    if (a == NULL || bench_arbiter_busy(a, e->now) || a->count == 0) {
      continue;
    }
    bench_ticks_t wait = 0;
    const uint8_t served =
        bench_arbiter_grant(a, e->now, e->bus_occupancy_ticks, &wait);
    if (served == BENCH_ARB_NO_OWNER) {
      continue;
    }
    bench_producer_t *p = &e->producers[served];
    p->granted_at = e->now;
    p->bus_wait_ticks = wait;
    // Timeout d'ATTENTE DU BUS : cause distincte d'un peripherique muet.
    if (p->profile->timeout_ticks > 0 && wait > p->profile->timeout_ticks) {
      p->released_at = e->now;
      bench_arbiter_release(a, e->now);
      emit_result(e, p, BENCH_SAMPLE_TIMEOUT, BENCH_TIMEOUT_BUS_WAIT);
    }
  }

  // 4) Avance du temps VIRTUEL jusqu'au prochain evenement.
  bench_ticks_t next = 0;
  if (!next_deadline(e, &next)) {
    return 0;  // plus aucun evenement : run termine
  }
  if (next > e->now) {
    e->now = next;
  } else {
    e->now++;  // progression garantie : jamais de blocage du temps
  }
  return 1;
}

uint32_t bench_concurrent_run(bench_concurrent_t *e, uint32_t max_steps) {
  uint32_t steps = 0;
  while (steps < max_steps && bench_concurrent_step(e)) {
    steps++;
  }
  return steps;
}

// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port HORLOGE : le coeur portable n'a AUCUNE horloge propre. Le temps monotone
// "now" (en ticks 64 bits) est fourni par l'adaptateur (HAL) via ce port. La
// largeur du compteur (64 bits) et les comparaisons wrap-safe sont documentees
// dans scheduler.h.
#ifndef BENCH_PORT_CLOCK_H
#define BENCH_PORT_CLOCK_H

#include "events/events.h"  // bench_ticks_t

typedef bench_ticks_t (*bench_now_fn)(void *ctx);

typedef struct {
  void *ctx;
  bench_now_fn now;
} bench_clock_t;

static inline bench_ticks_t bench_clock_now(const bench_clock_t *clock) {
  return clock->now(clock->ctx);
}

#endif  // BENCH_PORT_CLOCK_H

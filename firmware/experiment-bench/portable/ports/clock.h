// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port HORLOGE : le coeur portable n'a AUCUNE horloge propre. Le temps monotone
// "now" (ticks 64 bits) est fourni par l'adaptateur (HAL) ou le lien simule. Un
// "delay" OPTIONNEL modelise l'attente (pacing inter-transaction, budget de
// timeout) : sur materiel il dort, en simulation il avance l'horloge. La largeur
// du compteur (64 bits) et les comparaisons wrap-safe sont documentees dans
// scheduler.h.
#ifndef BENCH_PORT_CLOCK_H
#define BENCH_PORT_CLOCK_H

#include <stddef.h>  // NULL

#include "events/events.h"  // bench_ticks_t

typedef bench_ticks_t (*bench_now_fn)(void *ctx);
typedef void (*bench_delay_fn)(void *ctx, bench_ticks_t ticks);

typedef struct {
  void *ctx;
  bench_now_fn now;
  bench_delay_fn delay;  // peut etre NULL (aucune attente modelisee)
} bench_clock_t;

static inline bench_ticks_t bench_clock_now(const bench_clock_t *clock) {
  return clock->now(clock->ctx);
}

static inline void bench_clock_delay(const bench_clock_t *clock, bench_ticks_t ticks) {
  if (clock->delay != NULL) {
    clock->delay(clock->ctx, ticks);
  }
}

#endif  // BENCH_PORT_CLOCK_H

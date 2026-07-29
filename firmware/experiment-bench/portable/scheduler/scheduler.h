// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Echeances / timeouts ABSTRAITS. Le coeur portable ne depend d'AUCUN scheduler
// reel ni d'aucune horloge : le temps ("now") est fourni par l'appelant (HAL) en
// ticks opaques. Ce module ne fait que calculer et tester des echeances.
#ifndef BENCH_SCHEDULER_H
#define BENCH_SCHEDULER_H

#include <stdbool.h>

#include "events/events.h"  // bench_ticks_t

typedef struct {
  bench_ticks_t deadline;
} bench_deadline_t;

// Cree une echeance a partir d'un instant courant et d'un timeout (en ticks).
bench_deadline_t bench_deadline_make(bench_ticks_t now, bench_ticks_t timeout);

// Vrai si l'echeance est atteinte/depassee a l'instant "now".
bool bench_timeout_expired(bench_deadline_t deadline, bench_ticks_t now);

#endif  // BENCH_SCHEDULER_H

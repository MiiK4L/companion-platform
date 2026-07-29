// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Instrumentation par EVENEMENTS. Le coeur portable N'accede PAS au GPIO/serie :
// il EMET des evenements ; les backends (dans les boards) les traduisent (toggle
// GPIO pour l'analyseur logique, ligne serie, etc.). Aucune dependance plateforme.
#ifndef BENCH_EVENTS_H
#define BENCH_EVENTS_H

#include <stdint.h>

typedef uint64_t bench_ticks_t;

typedef enum {
  BENCH_EV_TX_BEGIN = 0,
  BENCH_EV_TX_END = 1,
  BENCH_EV_IRQ = 2,
  BENCH_EV_TIMEOUT = 3,
  BENCH_EV_CRC_ERROR = 4,
  BENCH_EV_RESET = 5,
} bench_event_type_t;

typedef struct {
  bench_event_type_t type;
  uint32_t seq;        // numero d'evenement (deterministe)
  bench_ticks_t at;    // horodatage abstrait (ticks fournis par l'appelant)
} bench_event_t;

// Puits d'evenements : implemente par un backend (board), jamais par le coeur.
typedef void (*bench_event_sink_t)(void *ctx, const bench_event_t *event);

#endif  // BENCH_EVENTS_H

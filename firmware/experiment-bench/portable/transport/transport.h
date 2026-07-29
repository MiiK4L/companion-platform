// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Transport GENERIQUE (extensible). Le SPI est le premier transport ; le champ
// "kind" et la separation interface/machine d'etat permettent d'en ajouter
// d'autres plus tard sans reecrire le coeur. Machine d'etat PURE : aucune E/S,
// aucun registre ; la progression et le temps ("now") sont fournis par l'appelant.
#ifndef BENCH_TRANSPORT_H
#define BENCH_TRANSPORT_H

#include <stdint.h>

#include "events/events.h"
#include "scheduler/scheduler.h"

typedef enum {
  BENCH_TRANSPORT_SPI = 0,  // seul transport implemente aujourd'hui (extensible)
} bench_transport_kind_t;

typedef enum {
  BENCH_TXN_IDLE = 0,
  BENCH_TXN_ACTIVE = 1,
  BENCH_TXN_DONE = 2,
  BENCH_TXN_TIMEOUT = 3,
} bench_txn_state_t;

typedef struct {
  bench_transport_kind_t kind;
  bench_txn_state_t state;
  bench_deadline_t deadline;
  uint32_t bytes_target;
  uint32_t bytes_done;
  bench_ticks_t started_at;
} bench_txn_t;

// Demarre une transaction de "bytes" octets, avec timeout (ticks) a partir de "now".
void bench_txn_begin(bench_txn_t *txn, bench_transport_kind_t kind, uint32_t bytes,
                     bench_ticks_t now, bench_ticks_t timeout);

// Fait progresser la transaction de "bytes" octets a l'instant "now" ; applique
// le timeout via l'echeance. Retourne l'etat courant.
bench_txn_state_t bench_txn_advance(bench_txn_t *txn, uint32_t bytes,
                                    bench_ticks_t now);

// Latence (ticks) d'une transaction terminee (0 si non terminee).
bench_ticks_t bench_txn_latency(const bench_txn_t *txn, bench_ticks_t now);

#endif  // BENCH_TRANSPORT_H

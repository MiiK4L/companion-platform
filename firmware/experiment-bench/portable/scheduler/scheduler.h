// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Echeances / timeouts ABSTRAITS et WRAP-SAFE. Le coeur ne depend d'aucune
// horloge : le temps "now" est fourni par l'appelant en ticks 64 bits monotones.
//
// LARGEUR DU COMPTEUR : bench_ticks_t = uint64_t. Les comparaisons utilisent la
// soustraction modulaire non signee (now - start), qui reste CORRECTE meme si le
// compteur de ticks reboucle (wrap), tant que la duree ecoulee reelle reste
// inferieure a 2^64 ticks. On ne compare donc jamais deux instants absolus
// directement (ce qui casserait au wrap) : on compare une DUREE ecoulee a un
// timeout.
//
// CAS timeout == 0 : budget temporel nul => l'echeance est consideree comme
// DEJA expiree des le premier test (comportement defini).
#ifndef BENCH_SCHEDULER_H
#define BENCH_SCHEDULER_H

#include <stdbool.h>

#include "events/events.h"  // bench_ticks_t

typedef struct {
  bench_ticks_t start;    // instant de reference
  bench_ticks_t timeout;  // budget en ticks (0 = deja expire)
} bench_deadline_t;

// Duree ecoulee WRAP-SAFE depuis "start" jusqu'a "now" (soustraction modulaire).
bench_ticks_t bench_elapsed(bench_ticks_t start, bench_ticks_t now);

// Cree une echeance a partir d'un instant courant et d'un timeout (en ticks).
bench_deadline_t bench_deadline_make(bench_ticks_t now, bench_ticks_t timeout);

// Vrai si le budget est epuise a l'instant "now" (compare une DUREE, pas un
// instant absolu). Wrap-safe.
bool bench_timeout_expired(bench_deadline_t deadline, bench_ticks_t now);

#endif  // BENCH_SCHEDULER_H

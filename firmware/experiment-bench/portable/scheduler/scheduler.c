// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "scheduler/scheduler.h"

bench_ticks_t bench_elapsed(bench_ticks_t start, bench_ticks_t now) {
  // Soustraction modulaire non signee : wrap-safe. Si le compteur a reboucle
  // (now < start numeriquement), le resultat modulo 2^64 reste la duree reelle
  // tant que celle-ci est inferieure a 2^64 ticks.
  return (bench_ticks_t)(now - start);
}

bench_deadline_t bench_deadline_make(bench_ticks_t now, bench_ticks_t timeout) {
  bench_deadline_t deadline;
  deadline.start = now;
  deadline.timeout = timeout;
  return deadline;
}

bool bench_timeout_expired(bench_deadline_t deadline, bench_ticks_t now) {
  // Compare une DUREE ecoulee a un budget (jamais deux instants absolus).
  // timeout == 0 => deja expire.
  return bench_elapsed(deadline.start, now) >= deadline.timeout;
}

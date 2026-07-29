// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Ordonnanceur : echeances nominales, timeout == 0, et WRAP du compteur de ticks.
#include <stdint.h>

#include "scheduler/scheduler.h"
#include "tests/test_support.h"
#include "tests/tests.h"

void run_scheduler_tests(void) {
  // Nominal : expire a l'echeance, pas avant.
  bench_deadline_t d = bench_deadline_make(100, 10);
  EXPECT(!bench_timeout_expired(d, 109), "pas encore expire a t=109");
  EXPECT(bench_timeout_expired(d, 110), "expire a l'echeance t=110");

  // timeout == 0 : deja expire (comportement defini).
  bench_deadline_t z = bench_deadline_make(100, 0);
  EXPECT(bench_timeout_expired(z, 100), "timeout=0 deja expire");

  // Duree ecoulee wrap-safe autour de la limite 64 bits.
  const bench_ticks_t near_max = UINT64_MAX - 5;
  EXPECT_EQ_INT(bench_elapsed(near_max, 4), 10,
                "elapsed wrap-safe (5 avant + wrap + 5 = 10)");

  // Echeance qui traverse le wrap : start juste avant UINT64_MAX, now apres 0.
  bench_deadline_t w = bench_deadline_make(near_max, 10);
  EXPECT(!bench_timeout_expired(w, 3), "wrap : ecoule 9 < 10, pas expire");
  EXPECT(bench_timeout_expired(w, 4), "wrap : ecoule 10 == 10, expire");
}

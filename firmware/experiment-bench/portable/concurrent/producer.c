// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "concurrent/producer.h"

#include <string.h>

void bench_producer_init(bench_producer_t *p, uint8_t producer_id,
                         const bench_profile_t *profile, int enabled) {
  memset(p, 0, sizeof(*p));
  p->producer_id = producer_id;
  p->profile = profile;
  p->enabled = enabled;
  // Seed PROPRE, derivee du profil : deux producteurs restent independants et
  // chacun reste rejouable a l'identique quelle que soit la topologie.
  bench_profile_seed(profile, &p->rng);
  p->next_due_at = 0;
}

int bench_producer_has_work(const bench_producer_t *p) {
  return p->enabled && p->issued < p->profile->transaction_count;
}

int bench_producer_due(const bench_producer_t *p, bench_ticks_t now) {
  if (!bench_producer_has_work(p) || p->pending) {
    return 0;
  }
  return now >= p->next_due_at;
}

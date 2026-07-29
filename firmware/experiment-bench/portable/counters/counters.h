// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Compteurs BRUTS (verite mesuree) et statistiques DERIVEES clairement separees.
// Le brut est la source de verite ; le derive est recalcule par une fonction pure.
#ifndef BENCH_COUNTERS_H
#define BENCH_COUNTERS_H

#include <stdint.h>

#include "events/events.h"  // bench_ticks_t

// --- Compteurs bruts : verite mesuree, jamais recalculee ---
typedef struct {
  uint32_t tx_ok;
  uint32_t tx_failed;
  uint32_t crc_errors;
  uint32_t timeouts;
  uint32_t irq_count;
  uint32_t resets;
  uint64_t bytes_transferred;
  uint64_t latency_sum_ticks;
  uint64_t latency_max_ticks;
  uint32_t latency_samples;
} bench_counters_t;

void bench_counters_reset(bench_counters_t *counters);
void bench_counters_record_tx(bench_counters_t *counters, int ok, uint32_t bytes,
                              bench_ticks_t latency);
void bench_counters_record_crc_error(bench_counters_t *counters);
void bench_counters_record_timeout(bench_counters_t *counters);
void bench_counters_record_irq(bench_counters_t *counters);
void bench_counters_record_reset(bench_counters_t *counters);

// --- Statistiques derivees : recalculees a la demande (fonction pure) ---
typedef struct {
  double latency_mean_ticks;
  double latency_max_ticks;
  double throughput_bytes_per_tick;
  double tx_success_ratio;
} bench_stats_t;

bench_stats_t bench_counters_derive(const bench_counters_t *counters,
                                    bench_ticks_t elapsed_ticks);

#endif  // BENCH_COUNTERS_H

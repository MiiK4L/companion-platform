// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "counters/counters.h"

void bench_counters_reset(bench_counters_t *counters) {
  *counters = (bench_counters_t){0};
}

void bench_counters_record_tx(bench_counters_t *counters, int ok, uint32_t bytes,
                              bench_ticks_t latency) {
  if (ok) {
    counters->tx_ok++;
  } else {
    counters->tx_failed++;
  }
  counters->bytes_transferred += bytes;
  counters->latency_sum_ticks += latency;
  if (latency > counters->latency_max_ticks) {
    counters->latency_max_ticks = latency;
  }
  counters->latency_samples++;
}

void bench_counters_record_crc_error(bench_counters_t *counters) {
  counters->crc_errors++;
}

void bench_counters_record_timeout(bench_counters_t *counters) {
  counters->timeouts++;
}

void bench_counters_record_irq(bench_counters_t *counters) {
  counters->irq_count++;
}

void bench_counters_record_reset(bench_counters_t *counters) {
  counters->resets++;
}

bench_stats_t bench_counters_derive(const bench_counters_t *counters,
                                    bench_ticks_t elapsed_ticks) {
  bench_stats_t stats = {0};
  if (counters->latency_samples > 0) {
    stats.latency_mean_ticks =
        (double)counters->latency_sum_ticks / (double)counters->latency_samples;
  }
  stats.latency_max_ticks = (double)counters->latency_max_ticks;
  if (elapsed_ticks > 0) {
    stats.throughput_bytes_per_tick =
        (double)counters->bytes_transferred / (double)elapsed_ticks;
  }
  const uint32_t total = counters->tx_ok + counters->tx_failed;
  if (total > 0) {
    stats.tx_success_ratio = (double)counters->tx_ok / (double)total;
  }
  return stats;
}

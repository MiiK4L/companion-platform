// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Compteurs : bruts vs derives, et SATURATION (aucun debordement silencieux).
#include <stdint.h>

#include "counters/counters.h"
#include "tests/test_support.h"
#include "tests/tests.h"

void run_counters_tests(void) {
  bench_counters_t c;
  bench_counters_reset(&c);
  bench_counters_record_tx(&c, 1, 100, 10);
  bench_counters_record_tx(&c, 0, 0, 30);
  bench_counters_record_crc_error(&c);
  EXPECT_EQ_INT(c.tx_ok, 1, "brut tx_ok");
  EXPECT_EQ_INT(c.tx_failed, 1, "brut tx_failed");
  EXPECT_EQ_INT(c.crc_errors, 1, "brut crc_errors");

  bench_stats_t s = bench_counters_derive(&c, 200);
  EXPECT(s.latency_mean_ticks == 20.0, "derive : latence moyenne (10+30)/2");
  EXPECT(s.tx_success_ratio == 0.5, "derive : ratio de succes 1/2");
  EXPECT(s.throughput_bytes_per_tick == 0.5, "derive : debit 100/200");

  // Saturation d'un compteur 32 bits : plafonne a UINT32_MAX, ne reboucle pas.
  bench_counters_t sat;
  bench_counters_reset(&sat);
  sat.crc_errors = UINT32_MAX;
  bench_counters_record_crc_error(&sat);
  EXPECT_EQ_INT(sat.crc_errors, UINT32_MAX, "crc_errors sature (pas de wrap)");

  // Saturation d'un cumul 64 bits d'octets.
  bench_counters_t satb;
  bench_counters_reset(&satb);
  satb.bytes_transferred = UINT64_MAX - 10;
  bench_counters_record_tx(&satb, 1, 100, 0);
  EXPECT(satb.bytes_transferred == UINT64_MAX, "bytes_transferred sature a UINT64_MAX");
}

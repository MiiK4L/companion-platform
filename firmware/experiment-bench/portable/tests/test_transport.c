// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Transport : nominal, timeout, transaction de taille NULLE, progression
// EXCESSIVE (saturation + plafonnement), idempotence en etat terminal.
#include <stdint.h>

#include "tests/test_support.h"
#include "tests/tests.h"
#include "transport/transport.h"

void run_transport_tests(void) {
  // Nominal.
  bench_txn_t txn;
  bench_txn_begin(&txn, BENCH_TRANSPORT_SPI, 10, 0, 100);
  EXPECT_EQ_INT(bench_txn_advance(&txn, 4, 10), BENCH_TXN_ACTIVE, "en cours");
  EXPECT_EQ_INT(bench_txn_advance(&txn, 6, 20), BENCH_TXN_DONE, "termine");
  EXPECT_EQ_INT(bench_txn_latency(&txn, 20), 20, "latence = now - start");

  // Timeout.
  bench_txn_t slow;
  bench_txn_begin(&slow, BENCH_TRANSPORT_SPI, 10, 0, 5);
  EXPECT_EQ_INT(bench_txn_advance(&slow, 1, 6), BENCH_TXN_TIMEOUT, "timeout");
  // Idempotence : un etat terminal ne change plus.
  EXPECT_EQ_INT(bench_txn_advance(&slow, 100, 7), BENCH_TXN_TIMEOUT,
                "timeout terminal idempotent");

  // Transaction de taille nulle : terminee immediatement (defini).
  bench_txn_t empty;
  bench_txn_begin(&empty, BENCH_TRANSPORT_SPI, 0, 0, 100);
  EXPECT_EQ_INT(empty.state, BENCH_TXN_DONE, "bytes_target=0 => DONE immediat");

  // Progression EXCESSIVE : saturation puis plafonnement a la cible.
  bench_txn_t over;
  bench_txn_begin(&over, BENCH_TRANSPORT_SPI, 10, 0, 100);
  EXPECT_EQ_INT(bench_txn_advance(&over, UINT32_MAX, 10), BENCH_TXN_DONE,
                "avance excessive => DONE");
  EXPECT_EQ_INT(over.bytes_done, 10, "bytes_done plafonne a la cible (pas de debordement)");
}

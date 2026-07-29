// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "transport/transport.h"

#include "util/sat.h"

void bench_txn_begin(bench_txn_t *txn, bench_transport_kind_t kind, uint32_t bytes,
                     bench_ticks_t now, bench_ticks_t timeout) {
  txn->kind = kind;
  txn->deadline = bench_deadline_make(now, timeout);
  txn->bytes_target = bytes;
  txn->bytes_done = 0;
  txn->started_at = now;
  // bytes_target == 0 : transaction vide => terminee immediatement (defini).
  txn->state = (bytes == 0) ? BENCH_TXN_DONE : BENCH_TXN_ACTIVE;
}

bench_txn_state_t bench_txn_advance(bench_txn_t *txn, uint32_t bytes,
                                    bench_ticks_t now) {
  if (txn->state != BENCH_TXN_ACTIVE) {
    return txn->state;  // etat terminal : idempotent
  }
  if (bench_timeout_expired(txn->deadline, now)) {
    txn->state = BENCH_TXN_TIMEOUT;
    return txn->state;
  }
  // Addition SATURANTE : une progression excessive ne peut pas deborder.
  txn->bytes_done = bench_sat_add_u32(txn->bytes_done, bytes);
  if (txn->bytes_done >= txn->bytes_target) {
    // Progression excessive => plafonnee a la cible (defini).
    txn->bytes_done = txn->bytes_target;
    txn->state = BENCH_TXN_DONE;
  }
  return txn->state;
}

bench_ticks_t bench_txn_latency(const bench_txn_t *txn, bench_ticks_t now) {
  if (txn->state == BENCH_TXN_DONE || txn->state == BENCH_TXN_TIMEOUT) {
    return bench_elapsed(txn->started_at, now);  // wrap-safe
  }
  return 0;
}

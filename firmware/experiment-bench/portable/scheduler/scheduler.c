// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "scheduler/scheduler.h"

bench_deadline_t bench_deadline_make(bench_ticks_t now, bench_ticks_t timeout) {
  bench_deadline_t deadline;
  deadline.deadline = now + timeout;
  return deadline;
}

bool bench_timeout_expired(bench_deadline_t deadline, bench_ticks_t now) {
  return now >= deadline.deadline;
}

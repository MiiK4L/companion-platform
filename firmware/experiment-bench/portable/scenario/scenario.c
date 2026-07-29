// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "scenario/scenario.h"

uint32_t bench_scenario_transaction_budget(const bench_scenario_t *scenario) {
  return scenario->profile->transaction_count;
}

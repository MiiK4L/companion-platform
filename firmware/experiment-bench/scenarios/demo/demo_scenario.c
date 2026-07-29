// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "scenarios/demo/demo_scenario.h"

#include "scenarios/demo/demo_profile.h"

// Scenario DECLARATIF : compose le profil de demonstration. Partage tel quel par
// l'hote et l'esclave ; aucune logique ici (elle vit dans le moteur portable).
const bench_scenario_t bench_demo_scenario = {
    .scenario_id = "demo",
    .scenario_version = 1,
    .profile = &bench_demo_profile,
    .duration_ticks = 0,  // borne par le profil (transaction_count)
};

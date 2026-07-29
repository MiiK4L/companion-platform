// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "scenarios/l1_spi_bringup/l1_scenario.h"

#include <stddef.h>

#include "scenarios/l1_spi_bringup/l1_profile.h"

const bench_scenario_t l1_screen_scenario = {
    .scenario_id = "l1-screen",
    .scenario_version = 1,
    .profile = &l1_screen_profile,
    .duration_ticks = 0,  // borne par transaction_count du profil
};

const bench_scenario_t l1_module_scenario = {
    .scenario_id = "l1-module",
    .scenario_version = 1,
    .profile = &l1_module_profile,
    .duration_ticks = 0,
};

// Table DECLARATIVE des 6 cas : 2 topologies x 3 modes de charge.
const l1_case_t l1_cases[L1_CASE_COUNT] = {
    {"spi-shared/screen-only", "spi-shared", L1_TOPOLOGY_SPI_SHARED,
     L1_MODE_SCREEN_ONLY, &l1_screen_scenario, NULL},
    {"spi-shared/module-only", "spi-shared", L1_TOPOLOGY_SPI_SHARED,
     L1_MODE_MODULE_ONLY, NULL, &l1_module_scenario},
    {"spi-shared/concurrent", "spi-shared", L1_TOPOLOGY_SPI_SHARED,
     L1_MODE_CONCURRENT, &l1_screen_scenario, &l1_module_scenario},
    {"spi-separated/screen-only", "spi-separated", L1_TOPOLOGY_SPI_SEPARATED,
     L1_MODE_SCREEN_ONLY, &l1_screen_scenario, NULL},
    {"spi-separated/module-only", "spi-separated", L1_TOPOLOGY_SPI_SEPARATED,
     L1_MODE_MODULE_ONLY, NULL, &l1_module_scenario},
    {"spi-separated/concurrent", "spi-separated", L1_TOPOLOGY_SPI_SEPARATED,
     L1_MODE_CONCURRENT, &l1_screen_scenario, &l1_module_scenario},
};

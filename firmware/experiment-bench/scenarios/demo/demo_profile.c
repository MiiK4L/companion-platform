// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "scenarios/demo/demo_profile.h"

// Profil DECLARATIF (donnees uniquement) : identifiant + version + seed rendent
// le scenario parfaitement rejouable. Aucun code, aucune logique ici.
const bench_profile_t bench_demo_profile = {
    .profile_id = "demo",
    .profile_version = 1,
    .seed = 0xC0FFEEull,
    .spi_clock_hz = 1000000,  // 1 MHz (declaratif : timing realise par l'adaptateur)
    .transaction_count = 64,
    .packet_size = 32,
    .inter_delay_ticks = 10,
    .timeout_ticks = 500,
    .payload_pattern = BENCH_PAYLOAD_SEEDED,
    .payload_fill = 0,
    .irq_policy = BENCH_IRQ_PER_TRANSACTION,
    .fault_crc_every = 0,
    .fault_crc_byte = 0,
    .fault_timeout_every = 0,
};

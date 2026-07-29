// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "scenarios/l1_spi_bringup/l1_profile.h"

// --- Trafic de type ECRAN -------------------------------------------------
// Rafales larges et rapprochees (rafraichissement d'afficheur). Aucune IRQ
// montante : un afficheur ne signale rien vers l'hote dans ce modele.
// AUCUNE injection de faute : la campagne mesure un comportement nominal ; la
// chaine de detection CRC est validee separement (gate materiel, cf. B4).
const bench_profile_t l1_screen_profile = {
    .profile_id = "l1-screen",
    .profile_version = 1,
    .seed = 0x1A2B3C4Dull,
    .spi_clock_hz = 20000000,  // 20 MHz — parametre de charge PROPOSE (hypothesis)
    .transaction_count = 512,
    .packet_size = 256,        // rafale large (plafond de payload de la trame)
    .inter_delay_ticks = 2,
    .timeout_ticks = 2000,
    .payload_pattern = BENCH_PAYLOAD_INCREMENT,  // motif reproductible et lisible
    .payload_fill = 0,
    .irq_policy = BENCH_IRQ_NEVER,
    .fault_crc_every = 0,
    .fault_crc_byte = 0,
    .fault_timeout_every = 0,
};

// --- Trafic de type MODULE CX-Bus -----------------------------------------
// Echanges courts periodiques avec IRQ a chaque transaction (le module signale
// la disponibilite d'une reponse).
const bench_profile_t l1_module_profile = {
    .profile_id = "l1-module",
    .profile_version = 1,
    .seed = 0x0BADC0DEull,
    .spi_clock_hz = 8000000,  // 8 MHz — parametre de charge PROPOSE (hypothesis)
    .transaction_count = 512,
    .packet_size = 32,
    .inter_delay_ticks = 20,
    .timeout_ticks = 1000,
    .payload_pattern = BENCH_PAYLOAD_SEEDED,  // contenu pseudo-aleatoire rejouable
    .payload_fill = 0,
    .irq_policy = BENCH_IRQ_PER_TRANSACTION,
    .fault_crc_every = 0,
    .fault_crc_byte = 0,
    .fault_timeout_every = 0,
};

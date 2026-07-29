// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Adaptateur HAL RP2040 (role ESCLAVE : simulateur de module CX-Bus). Expose des
// primitives bas niveau (horloge, reception/emission SPI via PIO, IRQ sortante,
// serie). Le port ISpiSlave (logique) est fourni par le moteur portable.
//
// STATUT : cible candidate de reference ; CONTRAT et SQUELETTE d'integration
// fournis ; implementation materielle (Pico SDK / PIO) NON realisee et NON
// validee ici. NON compile/teste en CI.
#ifndef BENCH_HAL_RP2040_H
#define BENCH_HAL_RP2040_H

#include "hal/common/hal.h"

// Construit la HAL esclave RP2040 (initialiserait PIO/SPI/GPIO/timer en local).
bench_hal_slave_t bench_hal_rp2040(void);

#endif  // BENCH_HAL_RP2040_H

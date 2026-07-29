// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Adaptateur HAL RP2040 (cible ESCLAVE de reference : simulateur de module
// CX-Bus). Couche FINE : horloge, SPI esclave (via PIO), IRQ, serie, backend
// d'evenements. Implementation locale (Pico SDK) — NON compilee/testee en CI.
#ifndef BENCH_HAL_RP2040_H
#define BENCH_HAL_RP2040_H

#include "hal/common/hal.h"

// Construit la HAL RP2040 (initialise PIO/SPI/GPIO/timer cote board).
bench_hal_t bench_hal_rp2040(void);

#endif  // BENCH_HAL_RP2040_H

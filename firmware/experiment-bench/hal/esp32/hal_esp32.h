// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Adaptateur HAL ESP32-S3 (role HOTE du banc). SQUELETTE documente : horloge,
// SPI maitre, IRQ, serie, backend d'evenements. Implementation locale (ESP-IDF)
// — NON compilee/testee en CI. Preserve la portabilite (le coeur est inchange).
#ifndef BENCH_HAL_ESP32_H
#define BENCH_HAL_ESP32_H

#include "hal/common/hal.h"

// Construit la HAL ESP32-S3 (initialise SPI/GPIO/timer cote board).
bench_hal_t bench_hal_esp32(void);

#endif  // BENCH_HAL_ESP32_H

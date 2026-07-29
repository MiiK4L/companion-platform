// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Adaptateur HAL ESP32-S3 (role HOTE du banc). Implemente le port MAITRE SPI et
// lit la ligne IRQ. SQUELETTE : implementation ESP-IDF NON realisee ici, NON
// compilee/testee en CI. Preserve la portabilite (le coeur reste inchange).
#ifndef BENCH_HAL_ESP32_H
#define BENCH_HAL_ESP32_H

#include "hal/common/hal.h"

// Construit la HAL hote ESP32-S3 (initialiserait SPI maitre/GPIO/timer en local).
bench_hal_host_t bench_hal_esp32(void);

#endif  // BENCH_HAL_ESP32_H

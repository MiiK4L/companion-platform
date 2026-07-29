// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// CRC-32 (IEEE 802.3) portable, sans table ni dependance plateforme.
#ifndef BENCH_CRC_H
#define BENCH_CRC_H

#include <stddef.h>
#include <stdint.h>

uint32_t bench_crc32(const uint8_t *data, size_t len);

#endif  // BENCH_CRC_H

// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port IBus — SIGNATURE CANDIDATE (non figée). Accès bus générique (I2C/SPI…)
// abstrait ; aucune dépendance à un pilote concret. Déclaration seule (aucune
// implémentation dans ce squelette de preuve d'architecture).
#ifndef COMPANION_PORTS_IBUS_H
#define COMPANION_PORTS_IBUS_H

#include <stddef.h>
#include <stdint.h>

typedef enum { BUS_OK = 0, BUS_ERR = 1 } bus_status_t;

typedef struct IBus {
  void *self;
  bus_status_t (*transfer)(void *self, uint8_t addr, const uint8_t *tx,
                           size_t tx_len, uint8_t *rx, size_t rx_len);
} IBus;

#endif  // COMPANION_PORTS_IBUS_H

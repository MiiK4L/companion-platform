// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Interfaces HAL/BSP FINES injectees dans le coeur portable. Le coeur ne connait
// QUE ces interfaces (pointeurs de fonctions + contexte opaque) ; les
// implementations concretes vivent dans hal/<cible>/ et boards/<carte>/. Ainsi,
// changer de microcontroleur = reimplementer cette couche fine, sans toucher au
// coeur (protocole, profils, compteurs, machines d'etat).
//
// Cette couche N'EST PAS testee par la CI host ; elle est validee au build local
// et aux essais materiels.
#ifndef BENCH_HAL_H
#define BENCH_HAL_H

#include <stddef.h>
#include <stdint.h>

#include "events/events.h"

// Horloge monotone : fournit "now" en ticks au coeur (aucune horloge dans le coeur).
typedef bench_ticks_t (*bench_hal_now_fn)(void *ctx);

// Transport SPI (maitre pour l'hote, esclave pour le simulateur CX-Bus). Echange
// "len" octets ; retourne le nombre d'octets effectivement transferes.
typedef uint32_t (*bench_hal_spi_xfer_fn)(void *ctx, const uint8_t *tx, uint8_t *rx,
                                          size_t len);

// Ligne IRQ (niveau lu / assertion). Sortie serie brute (dump structure).
typedef int (*bench_hal_irq_get_fn)(void *ctx);
typedef void (*bench_hal_serial_write_fn)(void *ctx, const char *line);

// Agregat HAL injecte dans l'application de banc.
typedef struct {
  void *ctx;
  bench_hal_now_fn now;
  bench_hal_spi_xfer_fn spi_xfer;
  bench_hal_irq_get_fn irq_get;
  bench_hal_serial_write_fn serial_write;
  bench_event_sink_t event_sink;  // backend d'instrumentation (ex. toggle GPIO)
  void *event_ctx;
} bench_hal_t;

#endif  // BENCH_HAL_H

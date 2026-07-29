// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Interfaces HAL/BSP FINES, injectees dans le coeur portable. Elles different
// selon le ROLE, car maitre et esclave n'ont pas le meme cycle de vie :
//
//  - bench_hal_host_t (role HOTE) : l'adaptateur IMPLEMENTE le port MAITRE
//    (bench_spi_master_t) et LIT la ligne IRQ (bench_irq_in_t).
//  - bench_hal_slave_t (role MODULE) : l'adaptateur expose des PRIMITIVES bas
//    niveau (recevoir une transaction / charger une reponse) et EMET la ligne
//    IRQ (bench_irq_out_t). Le port bench_spi_slave_t (la logique) est fourni
//    par le MOTEUR portable, pas par la HAL.
//
// Cette couche N'EST PAS testee par la CI host ; elle est validee au build local
// et aux essais materiels. Le coeur portable ne depend jamais de la plateforme.
#ifndef BENCH_HAL_H
#define BENCH_HAL_H

#include <stddef.h>
#include <stdint.h>

#include "events/events.h"
#include "ports/clock.h"
#include "ports/irq.h"
#include "ports/spi.h"

// Sortie serie brute (dump structure) commune aux deux roles.
typedef void (*bench_hal_serial_write_fn)(void *ctx, const char *line);

// --- HAL role HOTE (maitre) ---
typedef struct {
  bench_clock_t clock;                     // horloge monotone
  bench_spi_master_t spi;                  // port MAITRE implemente par l'adaptateur
  bench_irq_in_t irq;                      // lecture de la ligne IRQ
  bench_hal_serial_write_fn serial_write;  // peut etre NULL
  void *serial_ctx;
  bench_event_sink_t event_sink;           // backend d'instrumentation (peut etre NULL)
  void *event_ctx;
} bench_hal_host_t;

// Primitives bas niveau du role ESCLAVE (cablees a bench_spi_slave_t par le board).
typedef size_t (*bench_hal_slave_recv_fn)(void *ctx, uint8_t *rx, size_t cap);
typedef void (*bench_hal_slave_send_fn)(void *ctx, const uint8_t *tx, size_t len);

// --- HAL role MODULE (esclave) ---
typedef struct {
  bench_clock_t clock;
  void *spi_ctx;
  bench_hal_slave_recv_fn recv;            // attend/lit une transaction (MOSI)
  bench_hal_slave_send_fn send;            // charge la reponse (MISO)
  bench_irq_out_t irq;                     // emet l'IRQ vers l'hote
  bench_hal_serial_write_fn serial_write;
  void *serial_ctx;
  bench_event_sink_t event_sink;
  void *event_ctx;
} bench_hal_slave_t;

#endif  // BENCH_HAL_H

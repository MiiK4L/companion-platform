// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Board application RP2040 (role ESCLAVE = simulateur de module CX-Bus).
//
// SQUELETTE — NON compile ni flashe ici, NON couvert par la CI. Ce fichier ne
// contient AUCUNE logique metier : il ne fait que CABLER les primitives de la
// HAL au port ISpiSlave fourni par le MOTEUR portable. Tout le decodage de
// trames, la verification CRC, le comptage et l'IRQ vivent dans le moteur
// (portable/engine), teste en CI. Aucun contenu de campagne L1.

#include <stddef.h>
#include <stdint.h>

#include "engine/engine.h"
#include "frame/frame.h"
#include "hal/rp2040/hal_rp2040.h"
#include "ports/spi.h"

int main(void) {
  bench_hal_slave_t hal = bench_hal_rp2040();

  // Le MOTEUR portable fournit la logique esclave (port ISpiSlave).
  bench_slave_engine_t engine;
  bench_slave_engine_init(&engine, hal.clock, hal.irq, hal.event_sink,
                          hal.event_ctx);
  bench_spi_slave_t slave = bench_slave_engine_port(&engine);

  uint8_t rx[BENCH_FRAME_MAX_SIZE];
  uint8_t tx[BENCH_FRAME_MAX_SIZE];

  // Boucle de service : pur cablage HAL <-> port. Aucune logique ici.
  for (;;) {
    const size_t n = hal.recv(hal.spi_ctx, rx, sizeof(rx));  // attend une transaction
    slave.on_select(slave.ctx);
    slave.on_transaction(slave.ctx, rx, n);                  // decode + CRC + compteurs
    const size_t m = slave.peek_response(slave.ctx, tx, sizeof(tx));
    hal.send(hal.spi_ctx, tx, m);                            // charge la reponse (MISO)
  }
  return 0;
}

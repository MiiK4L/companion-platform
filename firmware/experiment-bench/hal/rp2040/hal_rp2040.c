// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Adaptateur HAL RP2040 (role ESCLAVE : simulateur de module CX-Bus).
//
// SQUELETTE — implementation materielle NON realisee et NON validee. Les corps
// ci-dessous sont des points d'accroche : l'integration locale (Pico SDK / PIO)
// remplacera chaque TODO par l'appel materiel. Ecrit SANS include Pico SDK
// (aucune signature devinee) ; compile tel quel mais ne fait rien de reel tant
// que l'implementation locale n'est pas faite. Le coeur portable reste inchange.
//
// Prerequis AVANT toute mesure (voir docs/target-comparison.md) : compilation
// Pico SDK reussie, PIO SPI-esclave implemente, boucle locale/Host testee, IRQ
// sortante verifiee, trames CRC reellement echangees.

#include "hal/rp2040/hal_rp2040.h"

#include <stddef.h>

typedef struct {
  int placeholder;  // TODO(local): handles Pico SDK (pio, sm, dma, gpio, timer).
} bench_rp2040_ctx_t;

static bench_rp2040_ctx_t g_ctx;

static bench_ticks_t rp2040_now(void *ctx) {
  (void)ctx;
  // TODO(local): time_us_64() (ou compteur PIO) converti en ticks abstraits.
  return 0;
}

static size_t rp2040_recv(void *ctx, uint8_t *rx, size_t cap) {
  (void)ctx;
  (void)rx;
  (void)cap;
  // TODO(local): attendre CS + cadencer les octets MOSI depuis la FIFO PIO.
  return 0;
}

static void rp2040_send(void *ctx, const uint8_t *tx, size_t len) {
  (void)ctx;
  (void)tx;
  (void)len;
  // TODO(local): charger la FIFO d'emission PIO (octets MISO) pour la reponse.
}

static void rp2040_irq_raise(void *ctx) {
  (void)ctx;
  // TODO(local): asserter la ligne IRQ vers l'hote (GPIO).
}

static void rp2040_irq_clear(void *ctx) {
  (void)ctx;
  // TODO(local): relacher la ligne IRQ.
}

static void rp2040_serial_write(void *ctx, const char *line) {
  (void)ctx;
  (void)line;
  // TODO(local): ecrire la ligne sur l'UART/USB-CDC.
}

bench_hal_slave_t bench_hal_rp2040(void) {
  // TODO(local): initialiser PIO/SPI/GPIO/timer dans g_ctx.
  bench_hal_slave_t hal;
  hal.clock.ctx = &g_ctx;
  hal.clock.now = rp2040_now;
  hal.clock.delay = NULL;  // TODO(local): sleep_us via l'horloge materielle.
  hal.spi_ctx = &g_ctx;
  hal.recv = rp2040_recv;
  hal.send = rp2040_send;
  hal.irq.ctx = &g_ctx;
  hal.irq.raise = rp2040_irq_raise;
  hal.irq.clear = rp2040_irq_clear;
  hal.serial_write = rp2040_serial_write;
  hal.serial_ctx = &g_ctx;
  hal.event_sink = NULL;  // TODO(local): backend GPIO pour analyseur logique.
  hal.event_ctx = &g_ctx;
  return hal;
}

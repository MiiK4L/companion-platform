// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Adaptateur HAL RP2040 (cible ESCLAVE de reference : simulateur CX-Bus).
//
// SQUELETTE — NON compile ni flashe ici, NON couvert par la CI. Les corps
// ci-dessous sont des points d'accroche : l'integration locale (Pico SDK / PIO)
// remplacera chaque TODO par l'appel materiel correspondant. Ecrit SANS include
// Pico SDK pour rester honnete (aucune signature SDK devinee) ; il compile donc
// tel quel mais ne fait rien de reel tant que l'integration locale n'est pas
// faite. Le coeur portable reste inchange quel que soit le contenu de ce fichier.

#include "hal/rp2040/hal_rp2040.h"

#include <stddef.h>

// Contexte materiel de la carte (handles PIO/SPI/GPIO/timer). Rempli localement.
typedef struct {
  int placeholder;  // TODO(local): handles Pico SDK (pio, sm, spi, gpio, timer).
} bench_rp2040_ctx_t;

static bench_rp2040_ctx_t g_rp2040_ctx;

static bench_ticks_t rp2040_now(void *ctx) {
  (void)ctx;
  // TODO(local): retourner time_us_64() (ou compteur PIO) en ticks abstraits.
  return 0;
}

static uint32_t rp2040_spi_xfer(void *ctx, const uint8_t *tx, uint8_t *rx,
                                size_t len) {
  (void)ctx;
  (void)tx;
  (void)rx;
  // TODO(local): servir l'echange SPI cote ESCLAVE via le programme PIO.
  return (uint32_t)len;
}

static int rp2040_irq_get(void *ctx) {
  (void)ctx;
  // TODO(local): lire l'etat de la ligne IRQ (GPIO).
  return 0;
}

static void rp2040_serial_write(void *ctx, const char *line) {
  (void)ctx;
  (void)line;
  // TODO(local): ecrire la ligne sur l'UART/USB-CDC.
}

static void rp2040_event_sink(void *ctx, const bench_event_t *event) {
  (void)ctx;
  (void)event;
  // TODO(local): traduire l'evenement (toggle GPIO pour analyseur, trace).
}

bench_hal_t bench_hal_rp2040(void) {
  // TODO(local): initialiser PIO/SPI/GPIO/timer dans g_rp2040_ctx.
  bench_hal_t hal;
  hal.ctx = &g_rp2040_ctx;
  hal.now = rp2040_now;
  hal.spi_xfer = rp2040_spi_xfer;
  hal.irq_get = rp2040_irq_get;
  hal.serial_write = rp2040_serial_write;
  hal.event_sink = rp2040_event_sink;
  hal.event_ctx = &g_rp2040_ctx;
  return hal;
}

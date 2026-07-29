// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Adaptateur HAL ESP32-S3 (role HOTE du banc : maitre SPI + orchestrateur).
//
// SQUELETTE — NON compile ni flashe ici, NON couvert par la CI. Points
// d'accroche uniquement : l'integration locale (ESP-IDF) remplacera chaque TODO
// par l'appel materiel. Ecrit SANS include ESP-IDF (aucune signature devinee) ;
// preserve la portabilite : le coeur portable ne depend jamais de ce fichier.

#include "hal/esp32/hal_esp32.h"

#include <stddef.h>

typedef struct {
  int placeholder;  // TODO(local): handles ESP-IDF (spi_device, gpio, esp_timer).
} bench_esp32_ctx_t;

static bench_esp32_ctx_t g_esp32_ctx;

static bench_ticks_t esp32_now(void *ctx) {
  (void)ctx;
  // TODO(local): retourner esp_timer_get_time() en ticks abstraits.
  return 0;
}

static uint32_t esp32_spi_xfer(void *ctx, const uint8_t *tx, uint8_t *rx,
                               size_t len) {
  (void)ctx;
  (void)tx;
  (void)rx;
  // TODO(local): echange SPI cote MAITRE via spi_device_transmit().
  return (uint32_t)len;
}

static int esp32_irq_get(void *ctx) {
  (void)ctx;
  // TODO(local): lire la ligne IRQ (gpio_get_level()).
  return 0;
}

static void esp32_serial_write(void *ctx, const char *line) {
  (void)ctx;
  (void)line;
  // TODO(local): ecrire la ligne sur l'UART console.
}

static void esp32_event_sink(void *ctx, const bench_event_t *event) {
  (void)ctx;
  (void)event;
  // TODO(local): traduire l'evenement (toggle GPIO / trace).
}

bench_hal_t bench_hal_esp32(void) {
  // TODO(local): initialiser SPI/GPIO/timer dans g_esp32_ctx.
  bench_hal_t hal;
  hal.ctx = &g_esp32_ctx;
  hal.now = esp32_now;
  hal.spi_xfer = esp32_spi_xfer;
  hal.irq_get = esp32_irq_get;
  hal.serial_write = esp32_serial_write;
  hal.event_sink = esp32_event_sink;
  hal.event_ctx = &g_esp32_ctx;
  return hal;
}

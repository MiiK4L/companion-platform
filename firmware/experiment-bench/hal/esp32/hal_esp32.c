// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Adaptateur HAL ESP32-S3 (role HOTE : maitre SPI + orchestrateur).
//
// SQUELETTE — implementation ESP-IDF NON realisee/validee ici. Points
// d'accroche uniquement (aucune signature ESP-IDF devinee) ; compile tel quel
// mais ne fait rien de reel tant que l'integration locale n'est pas faite.

#include "hal/esp32/hal_esp32.h"

#include <stddef.h>

typedef struct {
  int placeholder;  // TODO(local): handles ESP-IDF (spi_device, gpio, esp_timer).
} bench_esp32_ctx_t;

static bench_esp32_ctx_t g_ctx;

static bench_ticks_t esp32_now(void *ctx) {
  (void)ctx;
  // TODO(local): esp_timer_get_time() converti en ticks abstraits.
  return 0;
}

static bench_spi_status_t esp32_transfer(void *ctx, const uint8_t *tx, uint8_t *rx,
                                         size_t len) {
  (void)ctx;
  (void)tx;
  (void)rx;
  (void)len;
  // TODO(local): echange full-duplex via spi_device_transmit().
  return BENCH_SPI_ERROR;  // non implemente : signale explicitement l'echec
}

static void esp32_cancel(void *ctx) {
  (void)ctx;
  // TODO(local): abandonner un echange en cours.
}

static bench_spi_status_t esp32_status(void *ctx) {
  (void)ctx;
  return BENCH_SPI_ERROR;  // non implemente
}

static int esp32_irq_get(void *ctx) {
  (void)ctx;
  // TODO(local): gpio_get_level() de la ligne IRQ.
  return 0;
}

static void esp32_serial_write(void *ctx, const char *line) {
  (void)ctx;
  (void)line;
  // TODO(local): ecrire la ligne sur l'UART console.
}

bench_hal_host_t bench_hal_esp32(void) {
  // TODO(local): initialiser SPI maitre/GPIO/timer dans g_ctx.
  bench_hal_host_t hal;
  hal.clock.ctx = &g_ctx;
  hal.clock.now = esp32_now;
  hal.clock.delay = NULL;  // TODO(local): esp_rom_delay_us / vTaskDelay.
  hal.spi.ctx = &g_ctx;
  hal.spi.transfer = esp32_transfer;
  hal.spi.cancel = esp32_cancel;
  hal.spi.status = esp32_status;
  hal.irq.ctx = &g_ctx;
  hal.irq.get = esp32_irq_get;
  hal.serial_write = esp32_serial_write;
  hal.serial_ctx = &g_ctx;
  hal.event_sink = NULL;
  hal.event_ctx = &g_ctx;
  return hal;
}

// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Board application XIAO ESP32-S3 (role HOTE = maitre + orchestrateur).
//
// SQUELETTE — NON compile ni flashe ici, NON couvert par la CI. Aucune logique
// metier : cablage de la HAL hote au MOTEUR portable, puis execution du scenario
// partage. Le deroulement (trames CRC, injection de fautes, comptage) vit dans
// portable/engine, teste en CI. Aucun contenu de campagne L1.

#include <stddef.h>
#include <stdint.h>

#include "engine/engine.h"
#include "hal/esp32/hal_esp32.h"
#include "protocol/protocol.h"
#include "scenarios/demo/demo_scenario.h"

int main(void) {
  bench_hal_host_t hal = bench_hal_esp32();

  bench_host_engine_t engine;
  bench_host_engine_init(&engine, &bench_demo_scenario, &hal.spi, hal.irq,
                         hal.clock, hal.event_sink, hal.event_ctx);

  // Le meme SCENARIO est partage avec l'esclave ; aucune logique recopiee ici.
  bench_host_engine_run(&engine);

  // Restitution des compteurs BRUTS via le protocole binaire (sur la serie).
  if (hal.serial_write != NULL) {
    uint8_t out[BENCH_COUNTERS_WIRE_SIZE];
    if (bench_protocol_encode_counters(out, sizeof(out), &engine.counters) > 0) {
      // NOTE: transmission binaire reelle a faire cote adaptateur (hex/CDC).
      hal.serial_write(hal.serial_ctx, "counters ready");
    }
  }
  return 0;
}

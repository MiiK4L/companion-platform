#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0

# Vérifie la règle d'inversion de dépendances du squelette host (Lot 8) :
# les PORTS, SERVICES et le SDK ne doivent contenir AUCUN include vers une
# plateforme concrète (ESP-IDF, FreeRTOS, pilote) ni vers un moteur d'exécution
# concret (Lua/WASM/WAMR). Les détails de plateforme vivent dans les adaptateurs.
set -euo pipefail

ROOT="firmware/host-skeleton"
# Zones "propres" (indépendantes de la plateforme et des moteurs) :
# ports/, services/, models/ du squelette + le SDK (firmware/companion-sdk).
TARGETS=("$ROOT/ports" "$ROOT/services" "$ROOT/models" "firmware/companion-sdk")

# Motif d'include interdit dans ces zones :
# - plateforme : ESP-IDF (esp_*, esp-idf, sdkconfig), FreeRTOS, pilotes (driver/),
#   stockage bas niveau (nvs*, littlefs) ;
# - moteurs/UI concrets : lvgl, lua*, wasm/wamr.
PATTERN='#include[[:space:]]*[<"](esp_|esp-idf|sdkconfig|freertos|freertos/|driver/|nvs|nvs_flash|littlefs|lvgl|lua|lauxlib|lualib|wasm|wamr|wasm_export)'

found=0
for t in "${TARGETS[@]}"; do
  [ -d "$t" ] || continue
  if grep -rniE "$PATTERN" "$t"; then
    found=1
  fi
done

if [ "$found" -ne 0 ]; then
  echo "FAIL: include de plateforme/moteur interdit dans ports/services/models/SDK"
  exit 1
fi
echo "OK: ports/services/models/SDK sans include ESP-IDF/FreeRTOS/pilote/moteur"

# ---------------------------------------------------------------------------
# 2) Sens des dépendances internes : elles POINTENT vers les ports.
#    Ordre des couches (du plus abstrait au plus concret) :
#      ports/models  <-  services  <-  composition/adaptateurs
#    La flèche "<-" se lit « dépend de » : services dépend des ports, composition
#    et adaptateurs dépendent des ports/services — JAMAIS l'inverse. (Ceci décrit
#    la DIRECTION DES DÉPENDANCES, pas un ordre d'exécution.)
# ---------------------------------------------------------------------------
dir=0

# ports/ et models/ ne doivent PAS inclure services/, adapters/ ni composition/.
if grep -rnE '#include[[:space:]]*"(services|adapters|composition)/' \
    "$ROOT/ports" "$ROOT/models"; then
  echo "  ^ ports/ ou models/ dépend d'une couche supérieure (interdit)"
  dir=1
fi

# services/ ne doit PAS inclure adapters/ ni composition/.
if grep -rnE '#include[[:space:]]*"(adapters|composition)/' "$ROOT/services"; then
  echo "  ^ services/ dépend des adaptateurs/composition (interdit)"
  dir=1
fi

# Aucune couche "propre" ne doit inclure directement un fichier adapters/host/.
if grep -rnE '#include[[:space:]]*"adapters/host/' \
    "$ROOT/ports" "$ROOT/models" "$ROOT/services" "firmware/companion-sdk"; then
  echo "  ^ une couche propre inclut un adaptateur host (interdit)"
  dir=1
fi

if [ "$dir" -ne 0 ]; then
  echo "FAIL: dépendances internes mal orientées (elles doivent pointer vers les ports ; ports/models <- services <- composition/adaptateurs)"
  exit 1
fi
echo "OK: dépendances internes orientées vers les ports (ports/models <- services <- composition/adaptateurs)"

# ---------------------------------------------------------------------------
# 3) Cœur PORTABLE du firmware de banc (firmware/experiment-bench/portable/).
#    Il doit rester 100% indépendant de la plateforme : ni include, ni SYMBOLE
#    de plateforme (Pico SDK / PIO, ESP-IDF, FreeRTOS, Arduino, pilote). Le temps
#    réel, le GPIO, le SPI concret vivent dans hal/<cible>/ et boards/<carte>/.
# ---------------------------------------------------------------------------
PORTABLE="firmware/experiment-bench/portable"
port=0

if [ -d "$PORTABLE" ]; then
  # 3a) Includes de plateforme/SDK interdits (insensible à la casse).
  INC_PATTERN='#include[[:space:]]*[<"](pico/|hardware/|pio|tusb|bsp/|esp_|esp-idf|sdkconfig|freertos|driver/|nvs|arduino|freertos/)'
  if grep -rniE "$INC_PATTERN" "$PORTABLE"; then
    echo "  ^ include de plateforme/SDK dans le cœur portable (interdit)"
    port=1
  fi

  # 3b) Symboles de plateforme interdits (SENSIBLE à la casse pour éviter les
  #     faux positifs : BENCH_TRANSPORT_SPI, BENCH_EV_IRQ, irq_count restent OK).
  SYM_PATTERN='(gpio_[a-z]|pio_|pio0|pio1|sio_hw|spi_init|spi_write|spi_read|spi_device|sleep_ms|sleep_us|busy_wait|time_us_|stdio_init_all|pico_|esp_[a-z]|ESP_LOG|ESP_ERROR|esp_timer|esp_rom|gpio_num_t|xTaskCreate|vTaskDelay|pdMS_TO_TICKS|TickType_t|portTICK|ledc_|rmt_)'
  if grep -rnE "$SYM_PATTERN" "$PORTABLE"; then
    echo "  ^ symbole de plateforme dans le cœur portable (interdit)"
    port=1
  fi

  if [ "$port" -ne 0 ]; then
    echo "FAIL: le cœur portable du banc contient une dépendance plateforme (include ou symbole)"
    exit 1
  fi
  echo "OK: cœur portable du banc sans dépendance plateforme (ni include, ni symbole)"
fi

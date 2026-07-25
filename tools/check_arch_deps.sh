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

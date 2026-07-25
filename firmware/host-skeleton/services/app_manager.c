// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "services/app_manager.h"

// Aucun include ESP-IDF / FreeRTOS / pilote : le service ne parle QUE aux ports.
// (Vérifié mécaniquement par tools/check_arch_deps.sh.)

#define AM_MAX_UNIT 4096  // plafond de lecture borné (parser jamais aveugle)

void app_manager_init(AppManager *am, IStorage storage, IRuntime runtime,
                      ILog log) {
  am->storage = storage;
  am->runtime = runtime;
  am->log = log;
}

am_status_t app_manager_launch(AppManager *am, const char *name) {
  uint8_t unit[AM_MAX_UNIT];
  size_t unit_len = 0;

  st_status_t s = am->storage.read(am->storage.self, name, unit, sizeof(unit),
                                   &unit_len);
  if (s != ST_OK) {
    am->log.log(am->log.self, LOG_WARN, "app_manager: storage read failed");
    return AM_ERR_STORAGE;
  }

  if (am->runtime.load(am->runtime.self, unit, unit_len) != RT_OK) {
    am->log.log(am->log.self, LOG_ERROR, "app_manager: runtime load failed");
    return AM_ERR_RUNTIME;
  }
  if (am->runtime.run(am->runtime.self) != RT_OK) {
    am->log.log(am->log.self, LOG_ERROR, "app_manager: runtime run failed");
    return AM_ERR_RUNTIME;
  }
  am->log.log(am->log.self, LOG_INFO, "app_manager: app launched");
  return AM_OK;
}

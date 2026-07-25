// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "services/app_manager.h"

// Aucun include ESP-IDF / FreeRTOS / pilote / adaptateur : le service ne parle
// QU'aux ports (vérifié par tools/check_arch_deps.sh). Aucune allocation, aucun
// tampon, aucune hypothèse de format ou de cycle : pure délégation.

void app_manager_init(AppManager *am, IAppSource source, IRuntime runtime,
                      ILog log) {
  am->source = source;
  am->runtime = runtime;
  am->log = log;
}

am_status_t app_manager_launch(AppManager *am, AppReference reference) {
  AppArtifactView artifact = {0};

  if (am->source.resolve(am->source.self, reference, &artifact) != AS_OK) {
    am->log.log(am->log.self, LOG_WARN, "app_manager: source resolve failed");
    return AM_ERR_SOURCE;
  }

  // La vue est OPAQUE : le service ne l'inspecte pas, il la transmet au moteur.
  if (am->runtime.launch(am->runtime.self, artifact) != RT_OK) {
    am->log.log(am->log.self, LOG_ERROR, "app_manager: runtime launch failed");
    return AM_ERR_RUNTIME;
  }

  am->log.log(am->log.self, LOG_INFO, "app_manager: app launched");
  return AM_OK;
}

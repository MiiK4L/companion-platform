// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Composition root (host) — SEUL endroit qui connaît les implémentations
// concrètes (ici : mocks host). Elle POSSÈDE les adaptateurs (leur durée de vie
// = celle de la composition) et injecte leurs ports dans les services. Aucun
// service/port ne connaît la composition.
#ifndef COMPANION_COMPOSITION_ROOT_H
#define COMPANION_COMPOSITION_ROOT_H

#include "adapters/host/fake_runtime.h"
#include "adapters/host/mock_log.h"
#include "adapters/host/mock_storage.h"
#include "services/app_manager.h"

// Détient les adaptateurs host + le service câblé sur eux.
typedef struct HostComposition {
  FakeRuntime runtime;
  MockStorage storage;
  MockLog log;
  AppManager app_manager;
} HostComposition;

// Construit la composition : initialise les mocks puis injecte leurs ports
// dans l'AppManager. `app_key`/`app_unit` alimentent le MockStorage (non possédé
// par lui). La durée de vie de `c` doit couvrir tout usage de `c->app_manager`.
void host_composition_build(HostComposition *c, const char *app_key,
                            const uint8_t *app_unit, size_t app_len,
                            rt_status_t load_result, rt_status_t run_result);

#endif  // COMPANION_COMPOSITION_ROOT_H

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
#include "adapters/host/mock_app_source.h"
#include "adapters/host/mock_log.h"
#include "services/app_manager.h"

typedef struct HostComposition {
  MockAppSource source;
  FakeRuntime runtime;
  MockLog log;
  AppManager app_manager;
} HostComposition;

// Construit la composition : initialise les mocks/bouchons puis injecte leurs
// ports dans l'AppManager. `known_ref` est une référence OPAQUE empruntée : ses
// octets doivent rester valides tant que `c` est utilisée. La durée de vie de
// `c` doit couvrir tout usage de `c->app_manager`.
void host_composition_build(HostComposition *c, AppReference known_ref,
                            void *fake_handle, rt_status_t launch_result);

#endif  // COMPANION_COMPOSITION_ROOT_H

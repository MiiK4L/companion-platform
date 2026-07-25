// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// FakeRuntime — BOUCHON MINIMAL pour tests host. Renvoie un statut configurable
// (succès/échec) et compte les appels. AUCUNE interprétation de l'artefact,
// AUCUN moteur (ni Lua ni WAMR) : prouve seulement que l'AppManager ne dépend
// que du port IRuntime.
#ifndef COMPANION_ADAPTERS_HOST_FAKE_RUNTIME_H
#define COMPANION_ADAPTERS_HOST_FAKE_RUNTIME_H

#include "ports/iruntime.h"

typedef struct FakeRuntime {
  rt_status_t launch_result;  // statut renvoyé par launch()
  int launch_calls;
  const char *last_artifact_id;  // pour assertion : id de la dernière vue reçue
} FakeRuntime;

void fake_runtime_init(FakeRuntime *fr, rt_status_t launch_result);

IRuntime fake_runtime_port(FakeRuntime *fr);

#endif  // COMPANION_ADAPTERS_HOST_FAKE_RUNTIME_H

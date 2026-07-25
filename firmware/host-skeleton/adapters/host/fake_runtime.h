// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// FakeRuntime — BOUCHON MINIMAL pour tests host. Renvoie un statut
// configurable (succès/échec) et compte les appels. AUCUNE logique
// d'interprétation, AUCUN moteur (ni Lua ni WAMR) : il prouve seulement que
// l'AppManager ne dépend que du port IRuntime.
#ifndef COMPANION_ADAPTERS_HOST_FAKE_RUNTIME_H
#define COMPANION_ADAPTERS_HOST_FAKE_RUNTIME_H

#include "ports/iruntime.h"

typedef struct FakeRuntime {
  rt_status_t load_result;  // statut renvoyé par load()
  rt_status_t run_result;   // statut renvoyé par run()
  int load_calls;
  int run_calls;
} FakeRuntime;

void fake_runtime_init(FakeRuntime *fr, rt_status_t load_result,
                       rt_status_t run_result);

// Construit le port IRuntime pointant sur ce bouchon.
IRuntime fake_runtime_port(FakeRuntime *fr);

#endif  // COMPANION_ADAPTERS_HOST_FAKE_RUNTIME_H

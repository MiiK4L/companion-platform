// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port IRuntime — SIGNATURE CANDIDATE (non figée, révisable). Abstrait le moteur
// d'exécution scripté ; NE dépend d'aucun moteur concret (Lua/WAMR) et
// N'IMPOSE aucun cycle interne (load→run, streaming, mapping…) : c'est un détail
// d'implémentation du moteur (adaptateur) et une décision future (DEC-L8-001).
// L'AppManager ne dépend QUE de cette interface, jamais d'une implémentation.
#ifndef COMPANION_PORTS_IRUNTIME_H
#define COMPANION_PORTS_IRUNTIME_H

#include "models/app_artifact_view.h"

typedef enum {
  RT_OK = 0,
  RT_ERR = 1,
} rt_status_t;

typedef struct IRuntime {
  void *self;
  // Lance l'artefact (vue opaque). La façon d'y accéder/l'exécuter appartient
  // au moteur ; le service ne présume rien.
  rt_status_t (*launch)(void *self, AppArtifactView artifact);
} IRuntime;

#endif  // COMPANION_PORTS_IRUNTIME_H

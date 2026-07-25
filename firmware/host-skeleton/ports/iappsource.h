// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port IAppSource — SIGNATURE CANDIDATE (non figée). Résout une référence
// d'application (opaque) en une vue d'artefact opaque. AUCUNE hypothèse ici sur
// le stockage, l'adressage (nom/hash), le streaming ou le chargement : ce sont
// des détails d'ADAPTATEURS ou des décisions futures (DEC-L8-002).
#ifndef COMPANION_PORTS_IAPPSOURCE_H
#define COMPANION_PORTS_IAPPSOURCE_H

#include "models/app_artifact_view.h"

typedef enum {
  AS_OK = 0,
  AS_NOT_FOUND = 1,
} as_status_t;

typedef struct IAppSource {
  void *self;
  // Résout `app_ref` en une vue opaque `out`. Le service ne connaît QUE cette
  // interface (pas la manière dont la source obtient l'artefact).
  as_status_t (*resolve)(void *self, const char *app_ref, AppArtifactView *out);
} IAppSource;

#endif  // COMPANION_PORTS_IAPPSOURCE_H

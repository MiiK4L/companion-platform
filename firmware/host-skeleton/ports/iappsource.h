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
#include "models/app_reference.h"

typedef enum {
  AS_OK = 0,
  AS_NOT_FOUND = 1,
} as_status_t;

typedef struct IAppSource {
  void *self;
  // Résout la référence OPAQUE `reference` en une vue opaque `out`. Le service
  // ne connaît QUE cette interface (ni le format de la référence, ni la manière
  // dont la source obtient l'artefact). `reference` est EMPRUNTÉE : ses octets
  // ne doivent être supposés valides que pendant l'appel.
  as_status_t (*resolve)(void *self, AppReference reference, AppArtifactView *out);
} IAppSource;

#endif  // COMPANION_PORTS_IAPPSOURCE_H

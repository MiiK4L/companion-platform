// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Modèle AppReference — RÉFÉRENCE OPAQUE d'une application (candidate, non figée).
// Le squelette NE décide PAS si la référence est un nom, un chemin, un hash, un
// UUID binaire, un identifiant numérique ou une poignée vers un paquet déjà
// résolu : ce sont des octets opaques (non interprétés par le service).
//
// Durée de vie : la référence est EMPRUNTÉE. Les octets pointés par `data`
// doivent rester valides pendant l'appel qui la consomme (`resolve`). Aucun
// transfert de propriété.
#ifndef COMPANION_MODELS_APP_REFERENCE_H
#define COMPANION_MODELS_APP_REFERENCE_H

#include <stddef.h>

typedef struct AppReference {
  const void *data;  // octets opaques (nom, hash, UUID, poignée…) — non interprétés
  size_t size;
} AppReference;

#endif  // COMPANION_MODELS_APP_REFERENCE_H

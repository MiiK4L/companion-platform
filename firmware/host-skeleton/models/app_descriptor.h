// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Modèle AppDescriptor — TYPE DE DONNÉES seul (aucune logique).
// Représente ce que l'AppManager manipule ; les champs sont candidats (le
// format réel du Manifest relève du Lot 3, hors périmètre de ce squelette).
#ifndef COMPANION_MODELS_APP_DESCRIPTOR_H
#define COMPANION_MODELS_APP_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>

typedef struct AppDescriptor {
  const char *name;       // identifiant logique de l'app
  const uint8_t *unit;    // unité d'exécution opaque (script/bytecode)
  size_t unit_len;
} AppDescriptor;

#endif  // COMPANION_MODELS_APP_DESCRIPTOR_H

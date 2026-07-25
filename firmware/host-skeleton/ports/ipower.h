// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port IPower — SIGNATURE CANDIDATE (non figée). Gestion d'énergie abstraite
// (la chaîne d'alimentation/estimation relèvent du Lot 6, hors périmètre ici).
// Déclaration seule ; aucune implémentation dans ce squelette.
#ifndef COMPANION_PORTS_IPOWER_H
#define COMPANION_PORTS_IPOWER_H

#include <stdint.h>

typedef struct IPower {
  void *self;
  // Niveau d'énergie estimé, 0..100 (sémantique définie par l'adaptateur/Lot 6).
  uint8_t (*battery_percent)(void *self);
} IPower;

#endif  // COMPANION_PORTS_IPOWER_H

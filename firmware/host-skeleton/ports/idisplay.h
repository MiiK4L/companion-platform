// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port IDisplay — SIGNATURE CANDIDATE (non figée). Sortie d'affichage abstraite
// (le contrôleur/dalle/moteur graphique relèvent du Lot 4, hors périmètre ici).
// Déclaration seule ; aucune implémentation dans ce squelette.
#ifndef COMPANION_PORTS_IDISPLAY_H
#define COMPANION_PORTS_IDISPLAY_H

#include <stddef.h>
#include <stdint.h>

typedef struct IDisplay {
  void *self;
  // Pousse un tampon d'image opaque (format défini par l'adaptateur/Lot 4).
  void (*present)(void *self, const uint8_t *frame, size_t len);
} IDisplay;

#endif  // COMPANION_PORTS_IDISPLAY_H

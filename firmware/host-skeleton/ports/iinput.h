// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port IInput — SIGNATURE CANDIDATE (non figée). Entrée utilisateur abstraite
// (boutons/mouvement). Déclaration seule ; aucune implémentation ici.
#ifndef COMPANION_PORTS_IINPUT_H
#define COMPANION_PORTS_IINPUT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct InputEvent {
  uint16_t code;   // identifiant d'entrée (défini par l'adaptateur)
  bool pressed;
} InputEvent;

typedef struct IInput {
  void *self;
  // Récupère le prochain événement si disponible ; renvoie false sinon.
  bool (*poll)(void *self, InputEvent *out);
} IInput;

#endif  // COMPANION_PORTS_IINPUT_H

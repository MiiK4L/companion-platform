// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port IRuntime — SIGNATURE CANDIDATE (non figée, révisable).
// Abstrait le moteur d'exécution scripté ; NE dépend d'aucun moteur concret
// (Lua/WAMR). Le choix du moteur relève de DEC-L8-001, non de ce port.
// L'AppManager ne dépend QUE de cette interface, jamais d'une implémentation.
#ifndef COMPANION_PORTS_IRUNTIME_H
#define COMPANION_PORTS_IRUNTIME_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
  RT_OK = 0,
  RT_ERR_LOAD = 1,
  RT_ERR_RUN = 2,
} rt_status_t;

// Interface : contexte opaque + pointeurs de fonctions (dependency inversion).
// `self` appartient à l'implémentation (adaptateur), pas à l'appelant.
typedef struct IRuntime {
  void *self;
  rt_status_t (*load)(void *self, const uint8_t *unit, size_t len);
  rt_status_t (*run)(void *self);
} IRuntime;

#endif  // COMPANION_PORTS_IRUNTIME_H

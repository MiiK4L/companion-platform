// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port IClock — SIGNATURE CANDIDATE (non figée). Base de temps monotone
// abstraite ; aucune dépendance matérielle (RTC/ESP-IDF). Voir Lot 7 pour la
// source de temps et la validité de l'heure (hors périmètre de ce squelette).
#ifndef COMPANION_PORTS_ICLOCK_H
#define COMPANION_PORTS_ICLOCK_H

#include <stdint.h>

typedef struct IClock {
  void *self;
  uint64_t (*now_ms)(void *self);
} IClock;

#endif  // COMPANION_PORTS_ICLOCK_H

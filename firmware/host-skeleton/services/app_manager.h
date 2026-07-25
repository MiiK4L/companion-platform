// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Service AppManager — PREUVE D'ARCHITECTURE (câblage minimal, aucune logique
// métier). Il dépend UNIQUEMENT des ports (IStorage, IRuntime, ILog) — jamais
// d'une implémentation concrète (Lua/WAMR/NVS). Démontre l'inversion de
// dépendance : swap d'implémentation sans toucher au service.
#ifndef COMPANION_SERVICES_APP_MANAGER_H
#define COMPANION_SERVICES_APP_MANAGER_H

#include "ports/ilog.h"
#include "ports/iruntime.h"
#include "ports/istorage.h"

typedef enum {
  AM_OK = 0,
  AM_ERR_STORAGE = 1,
  AM_ERR_RUNTIME = 2,
} am_status_t;

// Le service détient des COPIES des ports (petites structures : contexte +
// pointeurs de fonctions). Il NE possède PAS les contextes `self` : leur durée
// de vie appartient à la composition root et doit couvrir celle de l'AppManager.
typedef struct AppManager {
  IStorage storage;
  IRuntime runtime;
  ILog log;
} AppManager;

// Injection par la composition root (ports fournis, non possédés).
void app_manager_init(AppManager *am, IStorage storage, IRuntime runtime,
                      ILog log);

// Câblage minimal : lit l'unité de l'app via le port storage (borné), puis la
// charge et l'exécute via le port runtime. AUCUNE interprétation ici.
am_status_t app_manager_launch(AppManager *am, const char *name);

#endif  // COMPANION_SERVICES_APP_MANAGER_H

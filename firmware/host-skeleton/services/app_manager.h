// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Service AppManager — PREUVE D'ARCHITECTURE : il démontre UNIQUEMENT la
// DÉLÉGATION à des ports injectés. Il n'encode AUCUN modèle de chargement
// (pas de tampon, pas de taille max, pas d'adressage par nom, pas de cycle
// load→run) — ces choix relèvent des adaptateurs et de DEC-L8-001/DEC-L8-002.
// Il dépend UNIQUEMENT des ports (IAppSource, IRuntime, ILog).
#ifndef COMPANION_SERVICES_APP_MANAGER_H
#define COMPANION_SERVICES_APP_MANAGER_H

#include "ports/iappsource.h"
#include "ports/ilog.h"
#include "ports/iruntime.h"

typedef enum {
  AM_OK = 0,
  AM_ERR_SOURCE = 1,   // la source n'a pas résolu la référence
  AM_ERR_RUNTIME = 2,  // le moteur a échoué à lancer l'artefact
} am_status_t;

// Le service détient des COPIES des ports (contexte + pointeurs). Il NE possède
// PAS les contextes `self` : leur durée de vie appartient à la composition root
// et doit couvrir celle de l'AppManager.
typedef struct AppManager {
  IAppSource source;
  IRuntime runtime;
  ILog log;
} AppManager;

void app_manager_init(AppManager *am, IAppSource source, IRuntime runtime,
                      ILog log);

// Délégation pure : résout la référence via la source (vue opaque), puis délègue
// le lancement au runtime. AUCUNE lecture/interprétation d'octets ici.
am_status_t app_manager_launch(AppManager *am, const char *app_ref);

#endif  // COMPANION_SERVICES_APP_MANAGER_H

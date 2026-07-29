// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// SCENARIO : niveau au-dessus du PROFIL. La distinction est volontaire et
// structurante :
//
//   Profile           : parametres DECLARATIFS et rejouables d'un trafic
//                        (paquets, delais, timeouts, injection de fautes).
//   Scenario          : COMPOSITION d'un profil avec des ROLES (Host/Slave),
//                        une version et une duree/orchestration.
//   Board application  : cablage des PORTS aux ADAPTATEURS materiels (main.c).
//
// Un scenario est partage entre l'hote et l'esclave : aucune logique n'est
// recopiee dans les main.c, qui ne font que cabler puis lancer le moteur.
#ifndef BENCH_SCENARIO_H
#define BENCH_SCENARIO_H

#include <stdint.h>

#include "profiles/profile.h"

typedef enum {
  BENCH_ROLE_HOST = 0,   // maitre SPI + orchestrateur
  BENCH_ROLE_SLAVE = 1,  // module CX-Bus simule (esclave)
} bench_role_t;

typedef struct {
  const char *scenario_id;     // identifiant stable du scenario
  uint32_t scenario_version;   // version du scenario
  const bench_profile_t *profile;  // profil de trafic emis par l'hote
  uint32_t duration_ticks;     // budget temporel indicatif (0 = borne par le profil)
} bench_scenario_t;

// Nombre de transactions a executer (borne par le profil).
uint32_t bench_scenario_transaction_budget(const bench_scenario_t *scenario);

#endif  // BENCH_SCENARIO_H

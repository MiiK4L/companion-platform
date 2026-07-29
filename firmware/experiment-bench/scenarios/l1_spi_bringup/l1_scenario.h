// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Scenarios de la campagne EXP-L1-BRINGUP-001 (question DEC-L1-001).
//
// Deux VARIANTES structurantes (topologies) x trois MODES DE CHARGE (controles) :
//
//   variante  : spi-shared | spi-separated        <- axe de variante (variant_id)
//   mode      : screen-only | module-only | concurrent   <- controle, PAS une variante
//
// Les modes isoles fournissent les references necessaires pour interpreter le
// mode concurrent : ils permettent de distinguer un probleme intrinseque a
// l'ecran, un probleme intrinseque au module, et un probleme reellement cause
// par la CONCURRENCE sur le bus partage. La comparaison principale reste
// spi-shared/concurrent vs spi-separated/concurrent.
//
// NOTE DE PERIMETRE (honnetete d'implementation) : ce fichier ne fait que
// DECLARER les cas. Le moteur portable livre en B2 execute UN flux ; savoir si
// le mode "concurrent" (deux flux simultanes) exige une evolution du coeur sera
// tranche en B4, dans une PR d'evolution SEPAREE de la campagne. Aucun cas n'est
// execute ici : ni mesure, ni flash, ni run.
#ifndef BENCH_L1_SCENARIO_H
#define BENCH_L1_SCENARIO_H

#include "scenario/scenario.h"

// Topologie SPI evaluee (axe de variante).
typedef enum {
  L1_TOPOLOGY_SPI_SHARED = 0,     // ecran et CX-Bus sur le meme bus, CS distincts
  L1_TOPOLOGY_SPI_SEPARATED = 1,  // bus SPI distincts
} l1_topology_t;

// Mode de charge (controle applique a CHAQUE topologie).
typedef enum {
  L1_MODE_SCREEN_ONLY = 0,  // ecran actif, simulateur CX-Bus inactif
  L1_MODE_MODULE_ONLY = 1,  // simulateur actif, ecran sans trafic
  L1_MODE_CONCURRENT = 2,   // ecran et module actifs simultanement
} l1_load_mode_t;

// Scenarios elementaires (composition profil + role + duree).
extern const bench_scenario_t l1_screen_scenario;
extern const bench_scenario_t l1_module_scenario;

// Cas de campagne = topologie x mode. Un pointeur NULL signifie "source de
// charge inactive dans ce mode" (donnee declarative, aucune logique).
typedef struct {
  const char *case_id;             // ex. "spi-shared/concurrent"
  const char *variant_id;          // "spi-shared" | "spi-separated"
  l1_topology_t topology;
  l1_load_mode_t mode;
  const bench_scenario_t *screen;  // NULL si l'ecran n'emet pas de trafic
  const bench_scenario_t *module;  // NULL si le simulateur est inactif
} l1_case_t;

#define L1_CASE_COUNT 6u
extern const l1_case_t l1_cases[L1_CASE_COUNT];

#endif  // BENCH_L1_SCENARIO_H

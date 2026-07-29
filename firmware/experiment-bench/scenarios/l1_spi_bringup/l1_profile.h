// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Profils de charge de la campagne EXP-L1-BRINGUP-001 (question DEC-L1-001).
// 100% DECLARATIFS : ces structures ne contiennent que des DONNEES ; le moteur
// portable les execute sans qu'aucun parametre ne soit code en dur.
//
// PERIMETRE : definition seule. Aucune mesure, aucun flash, aucune donnee [M].
// Les valeurs ci-dessous sont des PARAMETRES DE CHARGE proposes (classe de
// confiance "hypothesis" dans la baseline brouillon) ; elles seront revues et
// figees en B4, apres verification des capacites reelles du banc.
//
// Aucun composant ni MPN n'est retenu : "ecran" designe un trafic de TYPE
// afficheur (rafales larges), "module" un trafic de TYPE module CX-Bus
// (echanges courts + IRQ). Rien n'est fige.
#ifndef BENCH_L1_PROFILE_H
#define BENCH_L1_PROFILE_H

#include "profiles/profile.h"

// Trafic de type ECRAN : rafales larges, sans IRQ montante.
extern const bench_profile_t l1_screen_profile;

// Trafic de type MODULE CX-Bus : echanges courts periodiques, IRQ par transaction.
extern const bench_profile_t l1_module_profile;

#endif  // BENCH_L1_PROFILE_H

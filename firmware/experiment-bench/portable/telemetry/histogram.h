// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Histogramme de latence : VUE DERIVEE, bornee, OPTIONNELLE et DESACTIVEE PAR
// DEFAUT. Il sert au diagnostic direct et aux contraintes memoire ; il ne
// remplace JAMAIS la serie brute.
//
// Regles (contractuelles) :
//  - la SERIE BRUTE reste prioritaire et autoritaire ;
//  - les bornes sont DECLARATIVES et VERSIONNEES (profil) : deux histogrammes ne
//    sont comparables qu'a version de bornes identique ;
//  - accumulation SATURANTE (aucun debordement silencieux) ;
//  - underflow / overflow / sample_count sont OBLIGATOIRES ;
//  - aucun P95/P99 precis ne peut en etre tire : au mieux un INTERVALLE de
//    classe, qui ne peut satisfaire aucun critere de verdict exigeant une
//    valeur precise ;
//  - lorsqu'il est active, il est COMPARE au recalcul de l'outillage ; toute
//    divergence est un defaut d'instrumentation ou d'implementation ;
//  - s'il SATURE, il le signale (champ ``saturated``) et cesse d'etre
//    reconciliable : plus aucune comparaison stricte ni verdict possible.
//
// Convention de classe : [edges[i], edges[i+1]) — borne basse INCLUSE, borne
// haute EXCLUE. Une valeur < edges[0] compte en underflow ; une valeur
// >= edges[bin_count] compte en overflow.
#ifndef BENCH_TELEMETRY_HISTOGRAM_H
#define BENCH_TELEMETRY_HISTOGRAM_H

#include <stdint.h>

#include "events/events.h"  // bench_ticks_t

// Borne le nombre de classes pour tenir dans une trame (payload <= 256 octets).
#define BENCH_HISTOGRAM_MAX_BINS 16u

typedef struct {
  const bench_ticks_t *edges;  // bin_count + 1 bornes, STRICTEMENT croissantes
  uint32_t bin_count;
  uint32_t *counts;  // bin_count compteurs, fournis par l'appelant
  uint32_t underflow;
  uint32_t overflow;
  uint32_t sample_count;
  uint32_t version;  // version des bornes (declaree par le profil)
  // Vrai des qu'une accumulation AURAIT depasse la capacite d'un compteur.
  // Un histogramme sature reste utile au diagnostic, mais l'identite
  //   sample_count = somme(bin_counts) + underflow + overflow
  // n'est plus verifiable : il ne peut donc plus etre compare strictement au
  // recalcul de l'outillage, ni alimenter le moindre verdict.
  int saturated;
} bench_histogram_t;

// Initialise (remet les compteurs a zero). Retourne 0 si la configuration est
// invalide (bin_count nul ou > MAX, bornes non croissantes) : dans ce cas
// l'histogramme est inutilisable et doit rester desactive.
int bench_histogram_init(bench_histogram_t *h, const bench_ticks_t *edges,
                         uint32_t bin_count, uint32_t *counts, uint32_t version);

// Ajoute une valeur (accumulation saturante).
void bench_histogram_add(bench_histogram_t *h, bench_ticks_t value);

#endif  // BENCH_TELEMETRY_HISTOGRAM_H

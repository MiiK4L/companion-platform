// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Tampon circulaire BORNE d'echantillons. Il absorbe les variations temporaires
// du puits ; il n'est JAMAIS l'archive principale. La memoire est fournie par
// l'appelant (aucune allocation) : l'empreinte est O(capacite), independante du
// nombre de transactions.
//
// NON BLOQUANT par contrat : si le tampon est plein, l'echantillon est perdu
// AVANT serialisation (producer_drop, compteur SATURANT) et le moteur poursuit.
// Il n'attend jamais le puits.
//
// Une perte laisse une trace EXPLICITE : les pertes consecutives sont cumulees
// dans "pending_gap", que le drainage transforme en MARQUEUR DE LACUNE emis a
// sa position exacte dans le flux. Aucune perte silencieuse.
//
// Distinction essentielle, jamais fusionnee :
//   producer_drop : perdu AVANT serialisation (ce tampon)
//   transport_gap : trame serialisee mais absente/rejetee cote capture
#ifndef BENCH_TELEMETRY_RING_H
#define BENCH_TELEMETRY_RING_H

#include <stdint.h>

#include "telemetry/record.h"

typedef struct {
  bench_sample_t *slots;  // memoire fournie par l'appelant
  uint32_t capacity;
  uint32_t head;           // index de lecture
  uint32_t count;          // elements presents
  uint32_t producer_drop;  // total saturant de pertes avant serialisation
  uint32_t pending_gap;    // pertes non encore signalees par un marqueur
  uint32_t last_seq;       // dernier sequence_id accepte (pour situer la lacune)
  int has_last_seq;
} bench_ring_t;

void bench_ring_init(bench_ring_t *ring, bench_sample_t *slots, uint32_t capacity);

// Depose un echantillon. Retourne 1 s'il est stocke, 0 s'il est perdu
// (producer_drop et pending_gap incrementes). N'attend JAMAIS.
int bench_ring_push(bench_ring_t *ring, const bench_sample_t *sample);

// Retire le plus ancien echantillon. Retourne 1 si un echantillon a ete rendu.
int bench_ring_pop(bench_ring_t *ring, bench_sample_t *out);

// Recupere et remet a zero le compte de pertes en attente de signalement.
// "after_seq" recoit le dernier sequence_id accepte avant la lacune.
uint32_t bench_ring_take_gap(bench_ring_t *ring, uint32_t *after_seq);

#endif  // BENCH_TELEMETRY_RING_H

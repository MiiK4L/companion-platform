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
//
// ORDRE DES LACUNES. Une perte survient forcement APRES tous les echantillons
// deja presents dans le tampon. Un marqueur ne peut donc pas etre emis avant
// eux : chaque lacune memorise le nombre total d'echantillons ACCEPTES a
// l'instant de la perte (``after_pushed_total``) et n'est ECHUE que lorsque
// autant d'echantillons ont ete depiles. Exemple, capacite 4 :
//
//   tampon : seq 0,1,2,3   puis pertes seq 4,5
//   flux   : SAMPLE(0..3) PUIS GAP(2, apres seq 3)     <- et non l'inverse
//
// CONSOMMATION TRANSACTIONNELLE. peek/commit sont separes : le marqueur n'est
// retire qu'APRES acceptation complete de sa trame. Un refus du puits ne detruit
// donc jamais l'information de perte, qui sera reemise au drainage suivant.
//
// Distinction essentielle, jamais fusionnee :
//   producer_drop : perdu AVANT serialisation (ce tampon)
//   transport_gap : trame serialisee mais absente/rejetee cote capture
#ifndef BENCH_TELEMETRY_RING_H
#define BENCH_TELEMETRY_RING_H

#include <stdint.h>

#include "telemetry/record.h"

// Aucun echantillon accepte avant la lacune (perte des le premier depot).
#define BENCH_RING_NO_SEQ 0xFFFFFFFFu

// Une lacune conserve l'IDENTITE des producteurs touches : une perte dans le
// flux global ne doit jamais effacer QUI a perdu des echantillons.
#define BENCH_GAP_MAX_PRODUCERS 4u

typedef struct {
  uint32_t lost_count;          // total de pertes consecutives de cette plage
  uint32_t lost_by_producer[BENCH_GAP_MAX_PRODUCERS];  // ventilation exacte
  uint32_t after_global_seq;    // dernier global_event_seq accepte
  uint64_t after_pushed_total;  // echantillons acceptes avant la plage
} bench_gap_record_t;

typedef struct {
  bench_sample_t *slots;
  uint32_t capacity;
  uint32_t head;
  uint32_t count;
  uint64_t pushed_total;  // echantillons ACCEPTES depuis l'init
  uint64_t popped_total;  // echantillons DEPILES depuis l'init
  uint32_t producer_drop;  // total saturant de pertes avant serialisation

  // File bornee de plages de pertes DISTINCTES, en ordre chronologique.
  bench_gap_record_t *gaps;
  uint32_t gap_capacity;
  uint32_t gap_head;
  uint32_t gap_count;
  uint32_t gap_records_merged;  // plages fusionnees faute de place (saturant)

  uint32_t last_global_seq;
  int has_last_seq;
} bench_ring_t;

// "gaps" doit pouvoir contenir les plages en attente. Une capacite de
// (capacity + 1) garantit qu'aucune fusion n'est necessaire ; en deca, les
// plages excedentaires sont FUSIONNEES dans la derniere et signalees par
// gap_records_merged (jamais perdues silencieusement).
void bench_ring_init(bench_ring_t *ring, bench_sample_t *slots, uint32_t capacity,
                     bench_gap_record_t *gaps, uint32_t gap_capacity);

// Depose un echantillon. Retourne 1 s'il est stocke, 0 s'il est perdu.
// N'attend JAMAIS.
int bench_ring_push(bench_ring_t *ring, const bench_sample_t *sample);

// Retire le plus ancien echantillon. Retourne 1 si un echantillon a ete rendu.
int bench_ring_pop(bench_ring_t *ring, bench_sample_t *out);

// Consulte la plage de pertes la plus ancienne SI elle est echue, c'est-a-dire
// si tous les echantillons qui la precedent ont deja ete depiles. Retourne 1 et
// remplit "out" dans ce cas ; 0 sinon. NE consomme PAS la plage.
int bench_ring_peek_gap(const bench_ring_t *ring, bench_gap_record_t *out);

// Retire la plage la plus ancienne. A n'appeler qu'APRES emission acceptee.
void bench_ring_commit_gap(bench_ring_t *ring);

#endif  // BENCH_TELEMETRY_RING_H

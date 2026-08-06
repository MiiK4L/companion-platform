// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// MOTEUR CONCURRENT : execute plusieurs producteurs independants au-dessus d'un
// ou plusieurs bus, en TEMPS VIRTUEL et de facon EVENEMENTIELLE.
//
// Aucun parallelisme reel : le temps avance d'evenement en evenement. A seeds,
// politique et topologie fixees, l'entrelacement est donc EXACTEMENT
// reproductible — c'est ce qui rend les scenarios d'interleaving deterministes.
//
// TOPOLOGIES. La SEULE variable entre les deux montages est le nombre de bus :
//   spi-shared    : 1 arbitre pour les deux producteurs (contention possible)
//   spi-separated : 1 arbitre PAR producteur (aucune attente possible)
// Profils, seeds, politique, duree d'occupation et cadence sont IDENTIQUES.
//
// MODES ISOLES. On n'utilise PAS de profil simplifie : ce sont les MEMES
// producteurs, le second etant simplement desactive. `shared/screen-only` et
// `separated/screen-only` doivent donc produire des resultats identiques.
//
// PERIMETRE. Ce moteur valide l'arbitrage et l'instrumentation. Il ne produit
// AUCUNE donnee [M], aucune preuve experimentale, et ne permet d'arbitrer
// aucune decision : c'est une simulation.
#ifndef BENCH_CONCURRENT_H
#define BENCH_CONCURRENT_H

#include <stdint.h>

#include "arbiter/arbiter.h"
#include "concurrent/producer.h"
#include "counters/counters.h"
#include "events/events.h"
#include "telemetry/telemetry.h"

#define BENCH_CONC_MAX_PRODUCERS BENCH_MAX_PRODUCERS

// Resultat terminal d'une transaction concurrente (une seule cause).
typedef struct {
  uint8_t producer_id;
  uint32_t local_sequence;   // sequence PROPRE au producteur
  uint32_t global_order;     // rang dans l'ORDRE GLOBAL du flux
  bench_ticks_t requested_at;
  bench_ticks_t granted_at;
  bench_ticks_t released_at;
  bench_ticks_t bus_wait_ticks;
  uint8_t status;            // bench_sample_status_t
  uint8_t timeout_cause;     // bench_timeout_cause_t
} bench_conc_result_t;

// Rappel invoque a chaque transaction terminee (l'appelant l'archive).
typedef void (*bench_conc_sink_t)(void *ctx, const bench_conc_result_t *result);

typedef struct {
  bench_producer_t producers[BENCH_CONC_MAX_PRODUCERS];
  uint8_t producer_count;

  // 1 arbitre (partage) ou N arbitres (separe) : c'est LA variable de topologie.
  bench_arbiter_t *bus_of[BENCH_CONC_MAX_PRODUCERS];
  int shared_topology;  // 1 = tous les producteurs pointent le meme arbitre

  bench_ticks_t now;         // temps VIRTUEL
  uint32_t global_order;     // ordre global, strictement monotone
  bench_ticks_t bus_occupancy_ticks;  // duree d'occupation par transaction

  bench_conc_sink_t sink;
  void *sink_ctx;

  // Compteurs par producteur (bruts).
  bench_counters_t counters[BENCH_CONC_MAX_PRODUCERS];
  bench_ticks_t total_bus_wait[BENCH_CONC_MAX_PRODUCERS];
  uint32_t timeout_by_cause[5];
} bench_concurrent_t;

// Prepare le moteur. "bus_of" associe a chaque producteur son arbitre : passer
// le MEME pointeur partout = topologie partagee ; un pointeur distinct par
// producteur = topologie separee.
void bench_concurrent_init(bench_concurrent_t *e, bench_ticks_t occupancy_ticks,
                           bench_conc_sink_t sink, void *sink_ctx);

// Ajoute un producteur (au plus BENCH_CONC_MAX_PRODUCERS). "enabled" a 0 le
// desactive sans changer son profil : c'est ainsi qu'on obtient un mode isole.
void bench_concurrent_add_producer(bench_concurrent_t *e,
                                   const bench_profile_t *profile, int enabled,
                                   bench_arbiter_t *bus);

// Avance d'UN evenement. Retourne 1 s'il reste du travail, 0 si tout est fini.
int bench_concurrent_step(bench_concurrent_t *e);

// Deroule jusqu'a epuisement (borne par "max_steps" pour ne jamais boucler).
uint32_t bench_concurrent_run(bench_concurrent_t *e, uint32_t max_steps);

// --- Pont vers la TELEMETRIE v4 --------------------------------------------
// Convertit chaque resultat terminal en enregistrement v4 et le depose dans le
// tampon borne. Les deux numerotations et les quatre instants de bus sont
// transportes tels quels ; les durees en seront DERIVEES par l'outillage.
//
// Le pont compte les issues AU POINT D'ARCHIVAGE : un echantillon dont
// l'enregistrement est PERDU a pour statut terminal `producer_drop`, et son
// resultat n'est PAS aussi compte en ok/timeout. C'est ce qui rend l'identite
//   issued = ok + timeout + rejected + unpaired + duplicate + out_of_order
//            + producer_drop
// vraie PAR PRODUCTEUR : un enregistrement perdu a un resultat inconnu de
// l'archive, il ne peut pas etre compte deux fois.
typedef struct {
  bench_telemetry_t *tm;
  uint32_t issued[BENCH_CONC_MAX_PRODUCERS];
  uint32_t ok[BENCH_CONC_MAX_PRODUCERS];
  uint32_t timeout[BENCH_CONC_MAX_PRODUCERS];
  uint32_t rejected[BENCH_CONC_MAX_PRODUCERS];
  uint32_t producer_drop[BENCH_CONC_MAX_PRODUCERS];
  uint32_t timeout_by_cause[BENCH_CONC_MAX_PRODUCERS][5];
  uint32_t drain_budget;  // 0 = ne draine pas (drainage differe)
} bench_conc_telemetry_t;

void bench_conc_telemetry_init(bench_conc_telemetry_t *b, bench_telemetry_t *tm,
                               uint32_t drain_budget);
void bench_conc_telemetry_sink(void *ctx, const bench_conc_result_t *result);

// Renseigne un bilan PAR PRODUCTEUR : issues archivees depuis le pont, file et
// famine depuis les arbitres.
void bench_concurrent_fill_summary(const bench_concurrent_t *e,
                                   const bench_conc_telemetry_t *bridge,
                                   bench_telemetry_summary_t *out,
                                   bench_ticks_t now);

#endif  // BENCH_CONCURRENT_H

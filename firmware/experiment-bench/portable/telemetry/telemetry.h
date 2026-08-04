// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// FLUX de telemetrie : le coeur produit des enregistrements vers un PUITS
// ABSTRAIT. Aucune dependance a un port serie, a un OS ou a un transport
// particulier : les adaptateurs materiels viendront plus tard, sans toucher a
// ce module.
//
// Le flux est une suite de TRAMES B2 (magic/version/seq/len/payload/crc32) :
//  - le CRC rend chaque trame verifiable ;
//  - le champ "seq" de la trame est un NUMERO MONOTONE de flux : une
//    discontinuite cote capture revele un TRANSPORT_GAP.
//
// Deux pertes, jamais fusionnees :
//   producer_drop : echantillon perdu AVANT serialisation (ring sature)
//   transport_gap : trame serialisee mais absente/rejetee cote capture
//                   (deduite des numeros de sequence, pas comptee ici)
//
// Le puits est NON BLOQUANT et ATOMIQUE PAR TRAME : write_frame accepte la
// trame entiere (retourne 1) ou la refuse entierement (retourne 0). Il ne doit
// jamais accepter une trame partielle, qui corromprait le flux. Une trame
// refusee est comptee (frames_refused) et se manifestera cote capture comme un
// transport_gap. Le moteur n'attend JAMAIS le puits.
#ifndef BENCH_TELEMETRY_H
#define BENCH_TELEMETRY_H

#include <stddef.h>
#include <stdint.h>

#include "telemetry/histogram.h"
#include "telemetry/record.h"
#include "telemetry/ring.h"

// Version du FORMAT de flux (independante des versions de profil/scenario).
#define BENCH_TELEMETRY_STREAM_VERSION 3u

// Types de message (premier octet du payload de trame).
typedef enum {
  BENCH_TM_HEADER = 1,
  BENCH_TM_SAMPLE = 2,
  BENCH_TM_GAP = 3,
  BENCH_TM_SUMMARY = 4,
  BENCH_TM_HISTOGRAM = 5,
  BENCH_TM_FOOTER = 6,
} bench_tm_type_t;

// Politique de rebouclage du compteur de ticks (documentee dans l'en-tete).
#define BENCH_WRAP_POLICY_MODULAR 0u

// Puits abstrait : ecriture ATOMIQUE par trame.
typedef struct {
  void *ctx;
  int (*write_frame)(void *ctx, const uint8_t *frame, size_t len);
} bench_telemetry_sink_t;

// Metadonnees d'horloge et de run, emises une seule fois en tete de flux.
typedef struct {
  uint8_t clock_id;         // identifiant de l'horloge utilisee
  uint8_t tick_width_bits;  // largeur du compteur (64 pour bench_ticks_t)
  uint8_t wrap_policy;      // BENCH_WRAP_POLICY_MODULAR
  uint64_t tick_hz;         // frequence du tick (0 si inconnue a ce stade)
  uint32_t ring_capacity;
  uint8_t histogram_enabled;
  uint16_t histogram_version;
  const char *profile_id;  // peut etre NULL
  const char *variant;     // peut etre NULL
  const char *mode;        // peut etre NULL
} bench_telemetry_header_t;

// Bilan de fin de run : statuts TERMINAUX, un seul par transaction.
typedef struct {
  uint32_t issued;
  uint32_t ok;
  uint32_t timeout;
  uint32_t rejected;
  uint32_t unpaired;
  uint32_t duplicate;
  uint32_t out_of_order;
  uint32_t producer_drop;
  // Localisation des pertes : si des plages ont du etre FUSIONNEES faute de
  // place, le TOTAL des pertes reste exact mais leur POSITION ne l'est plus.
  // De meme si aucune place n'est declaree alors que des pertes surviennent.
  // Ces deux champs permettent a l'outillage de le savoir plutot que de croire
  // a une localisation exacte.
  uint32_t gap_records_merged;
  uint32_t gap_capacity;
  uint64_t timeout_budget_ticks;
} bench_telemetry_summary_t;

// Cloture AUTORITAIRE du flux. Sans elle, une perte des DERNIERES trames est
// indetectable : rien de posterieur ne revele leur absence. Son absence rend
// donc la capture INCOMPLETE par definition (y compris si le footer lui-meme
// est perdu).
typedef struct {
  uint32_t last_stream_seq;    // numero de sequence du footer lui-meme
  uint32_t frames_attempted;   // trames tentees, footer inclus
  uint32_t frames_accepted;    // trames acceptees AVANT le footer
  uint32_t frames_refused;     // trames refusees AVANT le footer
  uint32_t samples_attempted;  // echantillons deposes (acceptes + perdus)
} bench_telemetry_footer_t;

typedef struct {
  bench_telemetry_sink_t sink;
  bench_ring_t *ring;
  uint32_t stream_seq;       // numero monotone de trame (avance meme si refus)
  uint32_t frames_accepted;  // saturant
  uint32_t frames_refused;   // saturant
} bench_telemetry_t;

// Longueur maximale d'une chaine d'en-tete. Au-dela, l'en-tete est REJETE :
// jamais de troncature ni d'omission silencieuse.
#define BENCH_TELEMETRY_STR_MAX 48u

void bench_telemetry_init(bench_telemetry_t *tm, bench_telemetry_sink_t sink,
                          bench_ring_t *ring);

// Emet l'en-tete de flux. Retourne 1 si la trame a ete acceptee, 0 sinon.
// REJETTE (sans emettre aucune trame) si une chaine depasse
// BENCH_TELEMETRY_STR_MAX, contient un caractere non imprimable ASCII, ou si
// l'ensemble ne tient pas dans une trame.
int bench_telemetry_emit_header(bench_telemetry_t *tm,
                                const bench_telemetry_header_t *header);

// Draine au plus "max_messages" messages du ring vers le puits, en respectant
// l'ORDRE CHRONOLOGIQUE : un marqueur de lacune n'est emis qu'une fois depiles
// tous les echantillons qui le precedent. Sa consommation est TRANSACTIONNELLE
// (peek puis commit) : un refus du puits ne detruit pas l'information de perte,
// qui sera reemise au drainage suivant. Travail BORNE : jamais de blocage.
// Retourne le nombre de messages ACCEPTES par le puits.
uint32_t bench_telemetry_drain(bench_telemetry_t *tm, uint32_t max_messages);

// Emet le bilan. Les champs gap_records_merged et gap_capacity sont RENSEIGNES
// depuis le tampon lorsqu'il est attache : ils ne peuvent pas etre oublies.
int bench_telemetry_emit_summary(bench_telemetry_t *tm,
                                 const bench_telemetry_summary_t *summary);

// N'emet rien et retourne 0 si l'histogramme n'est pas initialise (desactive).
int bench_telemetry_emit_histogram(bench_telemetry_t *tm,
                                   const bench_histogram_t *histogram);

// Emet la CLOTURE du flux. A appeler en dernier. "samples_attempted" est le
// nombre total d'echantillons deposes dans le tampon (acceptes + perdus).
int bench_telemetry_emit_footer(bench_telemetry_t *tm, uint32_t samples_attempted);

#endif  // BENCH_TELEMETRY_H

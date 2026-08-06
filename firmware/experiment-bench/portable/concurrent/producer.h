// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// PRODUCTEUR autonome de charge. Chaque producteur est ENTIEREMENT responsable
// de sa cadence, de sa seed, de ses payloads et de ses IRQ. L'arbitre ne fait
// qu'accorder le bus ; il ne touche a rien de tout cela.
//
// Deux numerotations coexistent et ne se remplacent JAMAIS :
//   - la SEQUENCE LOCALE (ce fichier) : continue et propre au producteur ;
//   - l'ORDRE GLOBAL du flux (telemetrie) : entrelacement reel des deux sources.
#ifndef BENCH_PRODUCER_H
#define BENCH_PRODUCER_H

#include <stdint.h>

#include "events/events.h"
#include "profiles/profile.h"

// Causes de timeout EXPLICITES. Un timeout du a l'attente du bus ne doit jamais
// etre assimile a un peripherique silencieux : ce sont deux diagnostics
// opposes, l'un d'arbitrage, l'autre de peripherique.
typedef enum {
  BENCH_TIMEOUT_NONE = 0,
  BENCH_TIMEOUT_BUS_WAIT = 1,             // budget epuise EN ATTENTE du bus
  BENCH_TIMEOUT_PERIPHERAL_RESPONSE = 2,  // bus obtenu, peripherique muet
  BENCH_TIMEOUT_TRANSPORT = 3,            // echec du transport lui-meme
  BENCH_TIMEOUT_SCHEDULER = 4,            // budget epuise avant meme la demande
} bench_timeout_cause_t;

typedef struct {
  uint8_t producer_id;
  const bench_profile_t *profile;  // profil DECLARATIF, jamais modifie
  int enabled;                     // 0 = producteur desactive (mode isole)

  uint64_t rng;              // generateur PROPRE au producteur
  uint32_t local_sequence;   // sequence LOCALE, continue
  uint32_t issued;           // transactions emises
  bench_ticks_t next_due_at; // prochaine echeance de cadence

  // Etat de la transaction en cours.
  int pending;                    // une demande est en file
  bench_ticks_t requested_at;
  bench_ticks_t granted_at;
  bench_ticks_t released_at;
  bench_ticks_t bus_wait_ticks;   // cumul de la transaction courante
} bench_producer_t;

// Initialise un producteur a partir de son profil (seed derivee du profil).
void bench_producer_init(bench_producer_t *p, uint8_t producer_id,
                         const bench_profile_t *profile, int enabled);

// Vrai si le producteur a encore des transactions a emettre.
int bench_producer_has_work(const bench_producer_t *p);

// Vrai si le producteur doit emettre une demande a l'instant "now".
int bench_producer_due(const bench_producer_t *p, bench_ticks_t now);

#endif  // BENCH_PRODUCER_H

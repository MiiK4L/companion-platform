// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// ARBITRE d'une ressource de bus partagee.
//
// SEPARATION STRICTE generation / arbitrage. Les producteurs restent seuls
// responsables de leur cadence, de leurs seeds, de leurs payloads et de leurs
// IRQ. L'arbitre n'ACCORDE que le bus : il ne modifie JAMAIS le comportement
// interne d'un producteur.
//
// INVARIANT D'ATOMICITE (regle du moteur, pas simple hypothese documentaire) :
// une transaction SPI est ATOMIQUE. Le bus est acquis AVANT l'assertion du CS,
// reste detenu pendant toute la transaction, et n'est libere qu'apres sa fin
// complete. Aucune preemption au milieu d'une transaction ; la preemption ne
// peut intervenir qu'ENTRE deux transactions successives. L'arbitre refuse donc
// toute attribution tant que le bus est detenu.
//
// TOPOLOGIES. Un arbitre = UN bus. La variante `spi-shared` en instancie UN
// pour les deux producteurs ; `spi-separated` en instancie DEUX, un par
// producteur, qui progressent dans le meme temps virtuel. En topologie separee,
// aucune attente n'est possible : bus_wait_ticks == 0 (hors faute injectee).
#ifndef BENCH_ARBITER_H
#define BENCH_ARBITER_H

#include <stdint.h>

#include "events/events.h"  // bench_ticks_t

// Nombre maximal de producteurs geres par un arbitre (borne d'instrumentation).
#define BENCH_MAX_PRODUCERS 4u
// Aucun proprietaire courant.
#define BENCH_ARB_NO_OWNER 0xFFu

// Politiques d'arbitrage, DECLARATIVES (portees par le scenario).
typedef enum {
  // Politiques de REFERENCE de la campagne L1.
  BENCH_ARB_FIFO = 0,         // ordre d'arrivee, stable
  BENCH_ARB_ROUND_ROBIN = 1,  // alternance quand plusieurs attendent
  // Mode de STRESS : sert uniquement a valider la detection de famine et les
  // compteurs associes. Ce n'est PAS la politique de reference de la campagne.
  BENCH_ARB_FIXED_PRIORITY = 2,
} bench_arb_policy_t;

typedef struct {
  uint8_t producer_id;
  bench_ticks_t requested_at;
  uint32_t arrival;  // rang d'arrivee GLOBAL : depart le tie-break, stable
} bench_bus_request_t;

typedef struct {
  bench_arb_policy_t policy;
  uint8_t priority_of[BENCH_MAX_PRODUCERS];  // priorite fixe : 0 = plus fort

  // File d'attente BORNEE (memoire fournie par l'appelant).
  bench_bus_request_t *slots;
  uint32_t capacity;
  uint32_t head;
  uint32_t count;
  uint32_t arrival_counter;

  // Etat de possession (invariant d'atomicite).
  uint8_t owner;             // BENCH_ARB_NO_OWNER si libre
  bench_ticks_t owned_since;
  bench_ticks_t owned_until;  // fin de la transaction en cours
  uint8_t last_granted;       // pour le tourniquet

  // Instrumentation de file.
  uint32_t depth_by_producer[BENCH_MAX_PRODUCERS];
  uint32_t overflow_by_producer[BENCH_MAX_PRODUCERS];
  uint32_t max_depth_total;

  // Famine, par producteur.
  bench_ticks_t max_bus_wait_ticks[BENCH_MAX_PRODUCERS];
  uint32_t requests_over_starvation_threshold[BENCH_MAX_PRODUCERS];
  bench_ticks_t starvation_threshold_ticks;  // declaratif (0 = desactive)
} bench_arbiter_t;

void bench_arbiter_init(bench_arbiter_t *arb, bench_arb_policy_t policy,
                        bench_bus_request_t *slots, uint32_t capacity,
                        bench_ticks_t starvation_threshold_ticks);

// Fixe la priorite d'un producteur (mode de stress uniquement ; 0 = plus fort).
void bench_arbiter_set_priority(bench_arbiter_t *arb, uint8_t producer_id,
                                uint8_t priority);

// Depose une demande de bus. Retourne 1 si mise en file, 0 si la file est PLEINE
// (debordement compte par producteur, jamais silencieux).
int bench_arbiter_request(bench_arbiter_t *arb, uint8_t producer_id,
                          bench_ticks_t now);

// Tente d'accorder le bus a l'instant "now" pour une transaction de duree
// "duration". Retourne l'identifiant du producteur servi, ou BENCH_ARB_NO_OWNER
// si le bus est DETENU (atomicite) ou si la file est vide.
uint8_t bench_arbiter_grant(bench_arbiter_t *arb, bench_ticks_t now,
                            bench_ticks_t duration, bench_ticks_t *bus_wait_out);

// Libere le bus. Sans effet s'il est deja libre.
void bench_arbiter_release(bench_arbiter_t *arb, bench_ticks_t now);

// Vrai si le bus est detenu a l'instant "now".
int bench_arbiter_busy(const bench_arbiter_t *arb, bench_ticks_t now);

// Profondeur totale courante et age de la plus ancienne requete en attente.
uint32_t bench_arbiter_depth(const bench_arbiter_t *arb);
bench_ticks_t bench_arbiter_oldest_pending_age(const bench_arbiter_t *arb,
                                               bench_ticks_t now);

#endif  // BENCH_ARBITER_H

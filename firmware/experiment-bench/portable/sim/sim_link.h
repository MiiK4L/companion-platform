// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// LIEN SIMULE reliant un port MAITRE a un port ESCLAVE en memoire, avec une
// ligne IRQ partagee et une HORLOGE partagee. Ce n'est PAS un bus parfait : il
// peut injecter des comportements REALISTES par transaction (latence, CS relache
// prematurement, reponse tronquee, reponse perdue, timeout/erreur force, IRQ
// concurrente). Les tests host couvrent ainsi les problemes de SYNCHRONISATION,
// pas seulement le nominal. Modele requete -> reponse synchrone (simplification
// vs. full-duplex reel).
#ifndef BENCH_SIM_LINK_H
#define BENCH_SIM_LINK_H

#include <stddef.h>
#include <stdint.h>

#include "events/events.h"  // bench_ticks_t
#include "ports/clock.h"
#include "ports/irq.h"
#include "ports/spi.h"

// Comportement injecte pour UNE transaction. Valeurs par defaut = "bus parfait"
// (voir bench_sim_step_default) ; un hook peut les modifier par index.
typedef struct {
  bench_ticks_t added_latency_ticks;  // latence ajoutee (avance l'horloge simulee)
  size_t deliver_bytes;   // octets MOSI livres a l'esclave (< len => CS relache tot)
  size_t response_cap;    // plafond d'octets de reponse (< taille => reponse tronquee)
  int drop_response;      // 1 => aucune reponse (rx a zero)
  int preassert_irq;      // 1 => IRQ deja haute AVANT la transaction (concurrente)
  bench_spi_status_t force_status;  // statut retourne au maitre (TIMEOUT/ERROR possibles)
} bench_sim_step_t;

// Programme le comportement de la transaction "index" (step pre-initialise aux
// defauts "bus parfait"). NULL = aucun defaut injecte.
typedef void (*bench_sim_behavior_fn)(void *ctx, uint32_t index, bench_sim_step_t *step);

typedef struct {
  bench_spi_slave_t slave;         // port esclave connecte
  int irq_line;                    // ligne IRQ partagee (0/1)
  bench_ticks_t now;               // horloge simulee partagee
  uint32_t index;                  // compteur de transactions
  bench_sim_behavior_fn behavior;  // hook de faute (peut etre NULL)
  void *behavior_ctx;
} bench_sim_link_t;

void bench_sim_link_init(bench_sim_link_t *link, bench_spi_slave_t slave);
void bench_sim_link_set_behavior(bench_sim_link_t *link, bench_sim_behavior_fn fn,
                                 void *ctx);

// Port MAITRE, horloge partagee (now + delay), IRQ entrante (Host) et sortante
// (module) — tous adosses au meme lien.
bench_spi_master_t bench_sim_link_master(bench_sim_link_t *link);
bench_clock_t bench_sim_link_clock(bench_sim_link_t *link);
bench_irq_in_t bench_sim_link_irq_in(bench_sim_link_t *link);
bench_irq_out_t bench_sim_link_irq_out(bench_sim_link_t *link);

#endif  // BENCH_SIM_LINK_H

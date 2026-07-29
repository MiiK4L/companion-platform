// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// MOTEUR de banc portable, partage entre les roles Host et Slave. Toute la
// logique de deroulement (encodage de trames CRC, injection de fautes reelles,
// comptage, evenements) vit ICI ; les board applications ne font que cabler les
// ports aux adaptateurs materiels puis lancer le moteur. Aucune dependance
// plateforme.
//
// Flux par transaction :
//   Host  : encode une trame (seq, payload) -> [injection de faute reelle] ->
//           transfer() via le port MAITRE -> decode la reponse -> met a jour
//           compteurs/evenements.
//   Slave : on_transaction() decode la trame recue -> verifie le CRC -> met a
//           jour ses compteurs -> prepare une reponse (ACK) -> leve l'IRQ.
#ifndef BENCH_ENGINE_H
#define BENCH_ENGINE_H

#include <stddef.h>
#include <stdint.h>

#include "counters/counters.h"
#include "events/events.h"
#include "frame/frame.h"
#include "ports/clock.h"
#include "ports/irq.h"
#include "ports/spi.h"
#include "scenario/scenario.h"

// Acquittement transporte dans le payload de la reponse de l'esclave (1 octet).
typedef enum {
  BENCH_ACK_OK = 0,
  BENCH_ACK_CRC_ERROR = 1,  // trame recue rejetee (CRC/format invalide)
} bench_ack_t;

// --- Moteur ESCLAVE (module CX-Bus simule) ---
typedef struct {
  bench_counters_t counters;
  bench_irq_out_t irq;      // ligne IRQ sortante (raise/clear ; champs NULL = ignores)
  bench_irq_policy_t irq_policy;  // politique IRQ DECLAREE par le profil
  bench_event_sink_t sink;  // puits d'evenements (peut etre NULL)
  void *sink_ctx;
  bench_clock_t clock;      // horloge (now NULL -> 0)
  uint32_t event_seq;
  bench_slave_txn_status_t status;
  uint8_t response[BENCH_FRAME_MAX_SIZE];  // FIFO d'emission (octets MISO)
  size_t response_len;
} bench_slave_engine_t;

// La politique IRQ vient du PROFIL (scenario partage) : le moteur ne decide rien.
void bench_slave_engine_init(bench_slave_engine_t *engine, bench_clock_t clock,
                             bench_irq_out_t irq, bench_event_sink_t sink,
                             void *sink_ctx, bench_irq_policy_t irq_policy);

// Port ISpiSlave lie a ce moteur (a fournir au bus / lien simule).
bench_spi_slave_t bench_slave_engine_port(bench_slave_engine_t *engine);

// --- Moteur HOTE (maitre) ---
typedef struct {
  const bench_scenario_t *scenario;
  bench_spi_master_t *spi;
  bench_irq_in_t irq;       // lecture de la ligne IRQ (get NULL = ignore)
  bench_clock_t clock;
  bench_event_sink_t sink;
  void *sink_ctx;
  bench_counters_t counters;
  uint64_t rng;
  uint32_t event_seq;
} bench_host_engine_t;

void bench_host_engine_init(bench_host_engine_t *engine,
                            const bench_scenario_t *scenario,
                            bench_spi_master_t *spi, bench_irq_in_t irq,
                            bench_clock_t clock, bench_event_sink_t sink,
                            void *sink_ctx);

// Execute la transaction d'index "index". Retourne 0 (nominal) ou -1 (echec
// transport). Met a jour compteurs et evenements de l'hote.
int bench_host_engine_step(bench_host_engine_t *engine, uint32_t index);

// Execute tout le scenario (budget de transactions du profil).
void bench_host_engine_run(bench_host_engine_t *engine);

#endif  // BENCH_ENGINE_H

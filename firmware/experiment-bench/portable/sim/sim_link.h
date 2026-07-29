// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// LIEN SIMULE reliant un port MAITRE a un port ESCLAVE en memoire, plus une
// ligne IRQ partagee. C'est un "transport simule" portable : il permet de
// derouler le flux complet (trames CRC, injection de fautes, IRQ) NATIVEMENT en
// CI, sans materiel.
//
// Modele : requete -> reponse SYNCHRONE (simplification vs. full-duplex reel).
// Sur transfer(), l'esclave recoit d'abord la requete (on_transaction), MET A
// JOUR sa reponse, puis les octets de reponse (peek_response) sont cadences vers
// le maitre. La ligne IRQ est en lecture-et-effacement (modele sur front).
#ifndef BENCH_SIM_LINK_H
#define BENCH_SIM_LINK_H

#include "ports/irq.h"
#include "ports/spi.h"

typedef struct {
  bench_spi_slave_t slave;  // port esclave connecte
  int irq_line;             // ligne IRQ partagee (0/1)
} bench_sim_link_t;

void bench_sim_link_init(bench_sim_link_t *link, bench_spi_slave_t slave);

// Port MAITRE vu par l'hote.
bench_spi_master_t bench_sim_link_master(bench_sim_link_t *link);
// IRQ entrante (Host) : lecture-et-effacement de la ligne.
bench_irq_in_t bench_sim_link_irq_in(bench_sim_link_t *link);
// IRQ sortante (module) : assertion / relachement de la ligne.
bench_irq_out_t bench_sim_link_irq_out(bench_sim_link_t *link);

#endif  // BENCH_SIM_LINK_H

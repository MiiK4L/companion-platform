// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port IRQ, avec DIRECTION explicite. La ligne d'interruption n'est pas
// symetrique entre les deux roles :
//
//  - Cote HOST (entree) : on LIT l'etat de la ligne levee par le module.
//  - Cote MODULE esclave (sortie) : on EMET (assertion) puis on RELACHE la ligne
//    vers l'hote.
//
// L'ancienne primitive unique irq_get() ne convenait qu'a l'hote ; le module qui
// doit EMETTRE l'IRQ dispose desormais de son propre contrat.
#ifndef BENCH_PORT_IRQ_H
#define BENCH_PORT_IRQ_H

// --- Entree (Host) : lecture de la ligne IRQ ---
typedef int (*bench_irq_get_fn)(void *ctx);

typedef struct {
  void *ctx;
  bench_irq_get_fn get;
} bench_irq_in_t;

// --- Sortie (module esclave) : assertion / relachement de la ligne IRQ ---
typedef void (*bench_irq_raise_fn)(void *ctx);
typedef void (*bench_irq_clear_fn)(void *ctx);

typedef struct {
  void *ctx;
  bench_irq_raise_fn raise;
  bench_irq_clear_fn clear;
} bench_irq_out_t;

#endif  // BENCH_PORT_IRQ_H

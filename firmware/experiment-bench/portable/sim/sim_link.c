// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "sim/sim_link.h"

#include <string.h>

static void step_default(bench_sim_step_t *step) {
  step->added_latency_ticks = 0;
  step->deliver_bytes = SIZE_MAX;   // livre tous les octets (pas de CS premature)
  step->response_cap = SIZE_MAX;    // reponse complete
  step->drop_response = 0;
  step->preassert_irq = 0;
  step->force_status = BENCH_SPI_OK;
}

static bench_spi_status_t sim_transfer(void *ctx, const uint8_t *tx, uint8_t *rx,
                                       size_t len) {
  bench_sim_link_t *link = (bench_sim_link_t *)ctx;
  const uint32_t idx = link->index++;

  bench_sim_step_t step;
  step_default(&step);
  if (link->behavior != NULL) {
    link->behavior(link->behavior_ctx, idx, &step);
  }

  if (step.preassert_irq) {
    link->irq_line = 1;  // IRQ concurrente : deja haute avant traitement
  }

  // CS relache prematurement : l'esclave ne recoit qu'une partie des octets.
  const size_t deliver = (step.deliver_bytes < len) ? step.deliver_bytes : len;
  link->slave.on_select(link->slave.ctx);
  link->slave.on_transaction(link->slave.ctx, tx, deliver);

  // Latence : l'horloge partagee avance (le maitre la lira apres l'echange).
  link->now += step.added_latency_ticks;

  memset(rx, 0, len);
  if (!step.drop_response) {
    // Reponse tronquee si response_cap < taille reelle de la reponse.
    const size_t cap = (step.response_cap < len) ? step.response_cap : len;
    (void)link->slave.peek_response(link->slave.ctx, rx, cap);
  }

  return step.force_status;  // OK par defaut ; permet TIMEOUT/ERROR
}

static void sim_cancel(void *ctx) { (void)ctx; }

static bench_spi_status_t sim_status(void *ctx) {
  (void)ctx;
  return BENCH_SPI_OK;
}

static bench_ticks_t sim_now(void *ctx) {
  const bench_sim_link_t *link = (const bench_sim_link_t *)ctx;
  return link->now;
}

static void sim_delay(void *ctx, bench_ticks_t ticks) {
  bench_sim_link_t *link = (bench_sim_link_t *)ctx;
  link->now += ticks;  // "attente" = avance de l'horloge simulee
}

static void sim_irq_raise(void *ctx) {
  bench_sim_link_t *link = (bench_sim_link_t *)ctx;
  link->irq_line = 1;
}

static void sim_irq_clear(void *ctx) {
  bench_sim_link_t *link = (bench_sim_link_t *)ctx;
  link->irq_line = 0;
}

static int sim_irq_get(void *ctx) {
  bench_sim_link_t *link = (bench_sim_link_t *)ctx;
  const int level = link->irq_line;
  link->irq_line = 0;  // lecture-et-effacement (modele sur front)
  return level;
}

void bench_sim_link_init(bench_sim_link_t *link, bench_spi_slave_t slave) {
  link->slave = slave;
  link->irq_line = 0;
  link->now = 0;
  link->index = 0;
  link->behavior = NULL;
  link->behavior_ctx = NULL;
}

void bench_sim_link_set_behavior(bench_sim_link_t *link, bench_sim_behavior_fn fn,
                                 void *ctx) {
  link->behavior = fn;
  link->behavior_ctx = ctx;
}

bench_spi_master_t bench_sim_link_master(bench_sim_link_t *link) {
  bench_spi_master_t m;
  m.ctx = link;
  m.transfer = sim_transfer;
  m.cancel = sim_cancel;
  m.status = sim_status;
  return m;
}

bench_clock_t bench_sim_link_clock(bench_sim_link_t *link) {
  bench_clock_t c;
  c.ctx = link;
  c.now = sim_now;
  c.delay = sim_delay;
  return c;
}

bench_irq_in_t bench_sim_link_irq_in(bench_sim_link_t *link) {
  bench_irq_in_t in;
  in.ctx = link;
  in.get = sim_irq_get;
  return in;
}

bench_irq_out_t bench_sim_link_irq_out(bench_sim_link_t *link) {
  bench_irq_out_t out;
  out.ctx = link;
  out.raise = sim_irq_raise;
  out.clear = sim_irq_clear;
  return out;
}

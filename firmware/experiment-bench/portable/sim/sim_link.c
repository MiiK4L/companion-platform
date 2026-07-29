// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "sim/sim_link.h"

#include <string.h>

static bench_spi_status_t sim_transfer(void *ctx, const uint8_t *tx, uint8_t *rx,
                                       size_t len) {
  bench_sim_link_t *link = (bench_sim_link_t *)ctx;
  link->slave.on_select(link->slave.ctx);
  link->slave.on_transaction(link->slave.ctx, tx, len);
  memset(rx, 0, len);  // bourrage full-duplex par defaut
  (void)link->slave.peek_response(link->slave.ctx, rx, len);
  return BENCH_SPI_OK;
}

static void sim_cancel(void *ctx) { (void)ctx; }

static bench_spi_status_t sim_status(void *ctx) {
  (void)ctx;
  return BENCH_SPI_OK;
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
}

bench_spi_master_t bench_sim_link_master(bench_sim_link_t *link) {
  bench_spi_master_t m;
  m.ctx = link;
  m.transfer = sim_transfer;
  m.cancel = sim_cancel;
  m.status = sim_status;
  return m;
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

// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Moteur de bout en bout (Host + Slave sur lien simule) : trames CRC reelles,
// injection CRC/timeout demontree sur le FLUX (pas seulement un compteur),
// IRQ entrante/sortante, et ORDRE attendu des evenements.
#include <stddef.h>
#include <stdint.h>

#include "engine/engine.h"
#include "events/events.h"
#include "profiles/profile.h"
#include "scenario/scenario.h"
#include "sim/sim_link.h"
#include "tests/test_support.h"
#include "tests/tests.h"

typedef struct {
  bench_event_type_t types[256];
  uint32_t seqs[256];
  size_t n;
} rec_t;

static void rec_sink(void *ctx, const bench_event_t *ev) {
  rec_t *r = (rec_t *)ctx;
  if (r->n < 256) {
    r->types[r->n] = ev->type;
    r->seqs[r->n] = ev->seq;
    r->n++;
  }
}

typedef struct {
  bench_ticks_t t;
} clk_t;

static bench_ticks_t clk_now(void *ctx) {
  clk_t *c = (clk_t *)ctx;
  return c->t++;
}

// Deroule un scenario complet sur le lien simule ; restitue les compteurs des
// deux roles et la trace d'evenements de l'hote.
static void run_case(const bench_profile_t *prof, bench_counters_t *host_out,
                     bench_counters_t *slave_out, rec_t *rec) {
  const bench_scenario_t sc = {"t", 1, prof, 0};
  clk_t sclk = {1000};
  clk_t hclk = {2000};
  const bench_clock_t sclock = {&sclk, clk_now};
  const bench_clock_t hclock = {&hclk, clk_now};

  bench_sim_link_t link;
  const bench_irq_out_t irq_out = bench_sim_link_irq_out(&link);

  bench_slave_engine_t slave;
  bench_slave_engine_init(&slave, sclock, irq_out, NULL, NULL);
  const bench_spi_slave_t slave_port = bench_slave_engine_port(&slave);
  bench_sim_link_init(&link, slave_port);

  bench_spi_master_t master = bench_sim_link_master(&link);
  const bench_irq_in_t irq_in = bench_sim_link_irq_in(&link);

  bench_host_engine_t host;
  bench_host_engine_init(&host, &sc, &master, irq_in, hclock, rec_sink, rec);
  bench_host_engine_run(&host);

  *host_out = host.counters;
  *slave_out = slave.counters;
}

void run_engine_tests(void) {
  // --- Cas nominal (aucune faute) : tout passe, CRC verifie de bout en bout. ---
  {
    const bench_profile_t nominal = {"nom", 1, 7, 4, 8, 0, 500, 0, 0};
    bench_counters_t h;
    bench_counters_t s;
    rec_t rec = {{0}, {0}, 0};
    run_case(&nominal, &h, &s, &rec);
    EXPECT_EQ_INT(h.tx_ok, 4, "host : 4 transactions reussies");
    EXPECT_EQ_INT(h.tx_failed, 0, "host : aucune echouee");
    EXPECT_EQ_INT(h.crc_errors, 0, "host : aucun CRC error");
    EXPECT_EQ_INT(h.irq_count, 4, "host : 4 IRQ lues");
    EXPECT_EQ_INT(h.bytes_transferred, 32, "host : 4x8 octets");
    EXPECT_EQ_INT(s.tx_ok, 4, "slave : 4 trames valides recues");
    EXPECT_EQ_INT(s.crc_errors, 0, "slave : aucun rejet");
    EXPECT_EQ_INT(s.irq_count, 4, "slave : 4 IRQ emises");
  }

  // --- Injection CRC REELLE : le slave rejette, l'hote le constate. ---
  {
    // fault_crc_every=2 sur 4 tx -> indices 1 et 3 corrompus (2 fautes).
    const bench_profile_t crc = {"crc", 1, 7, 4, 8, 0, 500, 2, 0};
    bench_counters_t h;
    bench_counters_t s;
    rec_t rec = {{0}, {0}, 0};
    run_case(&crc, &h, &s, &rec);
    EXPECT_EQ_INT(s.crc_errors, 2, "slave : 2 trames corrompues DETECTEES (flux reel)");
    EXPECT_EQ_INT(s.tx_ok, 2, "slave : 2 trames valides");
    EXPECT_EQ_INT(h.crc_errors, 2, "host : 2 rejets rapportes par le slave");
    EXPECT_EQ_INT(h.tx_ok, 2, "host : 2 reussies");
    EXPECT_EQ_INT(h.tx_failed, 2, "host : 2 echouees");
  }

  // --- Injection TIMEOUT REELLE via l'ordonnanceur. ---
  {
    // fault_timeout_every=2 sur 4 tx -> indices 1 et 3 en timeout.
    const bench_profile_t to = {"to", 1, 7, 4, 8, 0, 100, 0, 2};
    bench_counters_t h;
    bench_counters_t s;
    rec_t rec = {{0}, {0}, 0};
    run_case(&to, &h, &s, &rec);
    EXPECT_EQ_INT(h.timeouts, 2, "host : 2 timeouts");
    EXPECT_EQ_INT(h.tx_ok, 2, "host : 2 reussies");
    EXPECT_EQ_INT(h.tx_failed, 2, "host : 2 echouees (timeout)");
    EXPECT_EQ_INT(s.tx_ok, 2, "slave : seules les 2 non-timeout arrivent");
  }

  // --- Ordre des evenements : 1 tx nominale -> TX_BEGIN, IRQ, TX_END. ---
  {
    const bench_profile_t one = {"one", 1, 7, 1, 8, 0, 500, 0, 0};
    bench_counters_t h;
    bench_counters_t s;
    rec_t rec = {{0}, {0}, 0};
    run_case(&one, &h, &s, &rec);
    EXPECT_EQ_INT(rec.n, 3, "3 evenements pour une transaction nominale");
    EXPECT_EQ_INT(rec.types[0], BENCH_EV_TX_BEGIN, "1er : TX_BEGIN");
    EXPECT_EQ_INT(rec.types[1], BENCH_EV_IRQ, "2e : IRQ");
    EXPECT_EQ_INT(rec.types[2], BENCH_EV_TX_END, "3e : TX_END");
    EXPECT_EQ_INT(rec.seqs[0], 0, "seq 0");
    EXPECT_EQ_INT(rec.seqs[1], 1, "seq 1");
    EXPECT_EQ_INT(rec.seqs[2], 2, "seq 2 (monotone)");
  }

  // --- Ordre des evenements : 1 tx en timeout -> TX_BEGIN, TIMEOUT, TX_END. ---
  {
    const bench_profile_t one_to = {"1to", 1, 7, 1, 8, 0, 100, 0, 1};
    bench_counters_t h;
    bench_counters_t s;
    rec_t rec = {{0}, {0}, 0};
    run_case(&one_to, &h, &s, &rec);
    EXPECT_EQ_INT(rec.n, 3, "3 evenements pour une transaction en timeout");
    EXPECT_EQ_INT(rec.types[0], BENCH_EV_TX_BEGIN, "1er : TX_BEGIN");
    EXPECT_EQ_INT(rec.types[1], BENCH_EV_TIMEOUT, "2e : TIMEOUT");
    EXPECT_EQ_INT(rec.types[2], BENCH_EV_TX_END, "3e : TX_END");
  }
}

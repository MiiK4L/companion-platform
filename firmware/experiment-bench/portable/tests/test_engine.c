// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Moteur de bout en bout (Host + Slave sur lien simule) : trames CRC reelles,
// injection CRC/timeout demontree sur le FLUX, IRQ selon la POLITIQUE declaree,
// et ordre attendu des evenements. Le moteur n'execute que ce que le profil
// decrit (aucun parametre de scenario code en dur).
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

// Deroule un scenario complet sur le lien simule (horloge partagee) ; restitue
// les compteurs des deux roles et la trace d'evenements de l'hote.
static void run_case(const bench_profile_t *prof, bench_counters_t *host_out,
                     bench_counters_t *slave_out, rec_t *rec) {
  const bench_scenario_t sc = {
      .scenario_id = "t", .scenario_version = 1, .profile = prof};

  bench_sim_link_t link;
  const bench_irq_out_t irq_out = bench_sim_link_irq_out(&link);
  const bench_clock_t clock = bench_sim_link_clock(&link);

  bench_slave_engine_t slave;
  bench_slave_engine_init(&slave, clock, irq_out, NULL, NULL, prof->irq_policy);
  const bench_spi_slave_t slave_port = bench_slave_engine_port(&slave);
  bench_sim_link_init(&link, slave_port);

  bench_spi_master_t master = bench_sim_link_master(&link);
  const bench_irq_in_t irq_in = bench_sim_link_irq_in(&link);

  bench_host_engine_t host;
  bench_host_engine_init(&host, &sc, &master, irq_in, clock, rec_sink, rec);
  bench_host_engine_run(&host);

  *host_out = host.counters;
  *slave_out = slave.counters;
}

void run_engine_tests(void) {
  // --- Nominal : tout passe, CRC verifie de bout en bout, IRQ par transaction. ---
  {
    const bench_profile_t nominal = {
        .profile_id = "nom", .seed = 7, .transaction_count = 4,
        .packet_size = 8, .timeout_ticks = 500,
        .irq_policy = BENCH_IRQ_PER_TRANSACTION};
    bench_counters_t h;
    bench_counters_t s;
    rec_t rec = {{0}, {0}, 0};
    run_case(&nominal, &h, &s, &rec);
    EXPECT_EQ_INT(h.tx_ok, 4, "host : 4 reussies");
    EXPECT_EQ_INT(h.tx_failed, 0, "host : aucune echouee");
    EXPECT_EQ_INT(h.crc_errors, 0, "host : aucun CRC error");
    EXPECT_EQ_INT(h.irq_count, 4, "host : 4 IRQ lues");
    EXPECT_EQ_INT(h.bytes_transferred, 32, "host : 4x8 octets");
    EXPECT_EQ_INT(s.tx_ok, 4, "slave : 4 trames valides");
    EXPECT_EQ_INT(s.crc_errors, 0, "slave : aucun rejet");
    EXPECT_EQ_INT(s.irq_count, 4, "slave : 4 IRQ emises");
  }

  // --- Injection CRC REELLE : le slave rejette, l'hote le constate. ---
  {
    const bench_profile_t crc = {
        .profile_id = "crc", .seed = 7, .transaction_count = 4,
        .packet_size = 8, .timeout_ticks = 500,
        .irq_policy = BENCH_IRQ_PER_TRANSACTION,
        .fault_crc_every = 2, .fault_crc_byte = 3};
    bench_counters_t h;
    bench_counters_t s;
    rec_t rec = {{0}, {0}, 0};
    run_case(&crc, &h, &s, &rec);
    EXPECT_EQ_INT(s.crc_errors, 2, "slave : 2 corruptions DETECTEES (flux reel)");
    EXPECT_EQ_INT(s.tx_ok, 2, "slave : 2 trames valides");
    EXPECT_EQ_INT(h.crc_errors, 2, "host : 2 rejets rapportes");
    EXPECT_EQ_INT(h.tx_ok, 2, "host : 2 reussies");
    EXPECT_EQ_INT(h.tx_failed, 2, "host : 2 echouees");
  }

  // --- Timeout DECLARE par le profil (budget epuise). ---
  {
    const bench_profile_t to = {
        .profile_id = "to", .seed = 7, .transaction_count = 4,
        .packet_size = 8, .timeout_ticks = 100, .fault_timeout_every = 2};
    bench_counters_t h;
    bench_counters_t s;
    rec_t rec = {{0}, {0}, 0};
    run_case(&to, &h, &s, &rec);
    EXPECT_EQ_INT(h.timeouts, 2, "host : 2 timeouts");
    EXPECT_EQ_INT(h.tx_ok, 2, "host : 2 reussies");
    EXPECT_EQ_INT(h.tx_failed, 2, "host : 2 echouees (timeout)");
    EXPECT_EQ_INT(s.tx_ok, 2, "slave : seules les 2 non-timeout arrivent");
  }

  // --- Politique IRQ NEVER : le slave n'emet aucune IRQ. ---
  {
    const bench_profile_t noirq = {
        .profile_id = "noirq", .seed = 7, .transaction_count = 3,
        .packet_size = 8, .timeout_ticks = 500, .irq_policy = BENCH_IRQ_NEVER};
    bench_counters_t h;
    bench_counters_t s;
    rec_t rec = {{0}, {0}, 0};
    run_case(&noirq, &h, &s, &rec);
    EXPECT_EQ_INT(s.irq_count, 0, "slave : aucune IRQ (policy NEVER)");
    EXPECT_EQ_INT(h.irq_count, 0, "host : aucune IRQ lue");
    EXPECT_EQ_INT(h.tx_ok, 3, "host : transactions reussies malgre policy NEVER");
  }

  // --- Politique IRQ ON_ERROR : IRQ seulement sur rejet. ---
  {
    const bench_profile_t onerr = {
        .profile_id = "onerr", .seed = 7, .transaction_count = 4,
        .packet_size = 8, .timeout_ticks = 500, .irq_policy = BENCH_IRQ_ON_ERROR,
        .fault_crc_every = 2, .fault_crc_byte = 0};
    bench_counters_t h;
    bench_counters_t s;
    rec_t rec = {{0}, {0}, 0};
    run_case(&onerr, &h, &s, &rec);
    EXPECT_EQ_INT(s.irq_count, 2, "slave : IRQ seulement sur les 2 rejets");
  }

  // --- Ordre des evenements : 1 tx nominale -> TX_BEGIN, IRQ, TX_END. ---
  {
    const bench_profile_t one = {
        .profile_id = "one", .seed = 7, .transaction_count = 1,
        .packet_size = 8, .timeout_ticks = 500,
        .irq_policy = BENCH_IRQ_PER_TRANSACTION};
    bench_counters_t h;
    bench_counters_t s;
    rec_t rec = {{0}, {0}, 0};
    run_case(&one, &h, &s, &rec);
    EXPECT_EQ_INT(rec.n, 3, "3 evenements (nominal)");
    EXPECT_EQ_INT(rec.types[0], BENCH_EV_TX_BEGIN, "1er : TX_BEGIN");
    EXPECT_EQ_INT(rec.types[1], BENCH_EV_IRQ, "2e : IRQ");
    EXPECT_EQ_INT(rec.types[2], BENCH_EV_TX_END, "3e : TX_END");
    EXPECT_EQ_INT(rec.seqs[0], 0, "seq 0");
    EXPECT_EQ_INT(rec.seqs[2], 2, "seq monotone");
  }

  // --- Ordre des evenements : 1 tx en timeout -> TX_BEGIN, TIMEOUT, TX_END. ---
  {
    const bench_profile_t one_to = {
        .profile_id = "1to", .seed = 7, .transaction_count = 1,
        .packet_size = 8, .timeout_ticks = 100, .fault_timeout_every = 1};
    bench_counters_t h;
    bench_counters_t s;
    rec_t rec = {{0}, {0}, 0};
    run_case(&one_to, &h, &s, &rec);
    EXPECT_EQ_INT(rec.n, 3, "3 evenements (timeout)");
    EXPECT_EQ_INT(rec.types[0], BENCH_EV_TX_BEGIN, "1er : TX_BEGIN");
    EXPECT_EQ_INT(rec.types[1], BENCH_EV_TIMEOUT, "2e : TIMEOUT");
    EXPECT_EQ_INT(rec.types[2], BENCH_EV_TX_END, "3e : TX_END");
  }
}

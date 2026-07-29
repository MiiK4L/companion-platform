// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Le lien simule injecte des comportements REALISTES : latence (=> timeout),
// reponse tronquee, CS relache prematurement, reponse perdue, statut force et
// IRQ concurrente. Ces tests couvrent la SYNCHRONISATION, pas seulement le
// nominal, et verifient que le moteur/l'hote reagissent correctement.
#include <stddef.h>
#include <stdint.h>

#include "engine/engine.h"
#include "profiles/profile.h"
#include "scenario/scenario.h"
#include "sim/sim_link.h"
#include "tests/test_support.h"
#include "tests/tests.h"

typedef enum {
  MODE_LATENCY_TIMEOUT,
  MODE_TRUNCATE_RESPONSE,
  MODE_PREMATURE_CS,
  MODE_DROP_RESPONSE,
  MODE_FORCE_TIMEOUT_STATUS,
  MODE_CONCURRENT_IRQ,
} fault_mode_t;

typedef struct {
  fault_mode_t mode;
  uint32_t target;  // index de transaction cible
} fault_cfg_t;

static void behavior(void *ctx, uint32_t index, bench_sim_step_t *step) {
  const fault_cfg_t *cfg = (const fault_cfg_t *)ctx;
  if (index != cfg->target) {
    return;  // les autres transactions restent nominales
  }
  switch (cfg->mode) {
    case MODE_LATENCY_TIMEOUT: step->added_latency_ticks = 1000; break;
    case MODE_TRUNCATE_RESPONSE: step->response_cap = 5; break;   // < taille d'un ACK
    case MODE_PREMATURE_CS: step->deliver_bytes = 4; break;       // CS relache tot
    case MODE_DROP_RESPONSE: step->drop_response = 1; break;
    case MODE_FORCE_TIMEOUT_STATUS: step->force_status = BENCH_SPI_TIMEOUT; break;
    case MODE_CONCURRENT_IRQ: step->preassert_irq = 1; break;
  }
}

static void run_faulted(const bench_profile_t *prof, fault_cfg_t *cfg,
                        bench_counters_t *host_out, bench_counters_t *slave_out) {
  const bench_scenario_t sc = {
      .scenario_id = "t", .scenario_version = 1, .profile = prof};

  bench_sim_link_t link;
  const bench_irq_out_t irq_out = bench_sim_link_irq_out(&link);
  const bench_clock_t clock = bench_sim_link_clock(&link);

  bench_slave_engine_t slave;
  bench_slave_engine_init(&slave, clock, irq_out, NULL, NULL, prof->irq_policy);
  const bench_spi_slave_t slave_port = bench_slave_engine_port(&slave);
  bench_sim_link_init(&link, slave_port);
  bench_sim_link_set_behavior(&link, behavior, cfg);

  bench_spi_master_t master = bench_sim_link_master(&link);
  const bench_irq_in_t irq_in = bench_sim_link_irq_in(&link);

  bench_host_engine_t host;
  bench_host_engine_init(&host, &sc, &master, irq_in, clock, NULL, NULL);
  bench_host_engine_run(&host);

  *host_out = host.counters;
  *slave_out = slave.counters;
}

void run_sim_tests(void) {
  const bench_profile_t base = {
      .profile_id = "sim", .seed = 7, .transaction_count = 2, .packet_size = 8,
      .timeout_ticks = 100, .irq_policy = BENCH_IRQ_PER_TRANSACTION};

  bench_counters_t h;
  bench_counters_t s;

  // Latence injectee (1000 >= timeout 100) => timeout ENVIRONNEMENTAL detecte.
  {
    fault_cfg_t cfg = {MODE_LATENCY_TIMEOUT, 0};
    run_faulted(&base, &cfg, &h, &s);
    EXPECT_EQ_INT(h.timeouts, 1, "latence injectee => 1 timeout");
    EXPECT_EQ_INT(h.tx_ok, 1, "l'autre transaction reussit");
  }

  // Reponse tronquee => l'hote ne peut pas valider l'ACK.
  {
    fault_cfg_t cfg = {MODE_TRUNCATE_RESPONSE, 0};
    run_faulted(&base, &cfg, &h, &s);
    EXPECT(h.crc_errors >= 1, "reponse tronquee detectee par l'hote");
    EXPECT_EQ_INT(h.tx_failed, 1, "transaction tronquee comptee echouee");
  }

  // CS relache prematurement => l'esclave recoit une trame incomplete.
  {
    fault_cfg_t cfg = {MODE_PREMATURE_CS, 0};
    run_faulted(&base, &cfg, &h, &s);
    EXPECT(s.crc_errors >= 1, "trame incomplete rejetee par l'esclave");
    EXPECT(h.crc_errors >= 1, "rejet rapporte a l'hote");
  }

  // Reponse perdue => decodage impossible cote hote.
  {
    fault_cfg_t cfg = {MODE_DROP_RESPONSE, 0};
    run_faulted(&base, &cfg, &h, &s);
    EXPECT(h.crc_errors >= 1, "reponse perdue detectee");
    EXPECT_EQ_INT(h.tx_failed, 1, "transaction sans reponse comptee echouee");
  }

  // Statut TIMEOUT force par le lien.
  {
    fault_cfg_t cfg = {MODE_FORCE_TIMEOUT_STATUS, 0};
    run_faulted(&base, &cfg, &h, &s);
    EXPECT_EQ_INT(h.timeouts, 1, "statut TIMEOUT force compte");
  }

  // IRQ CONCURRENTE : ligne haute meme avec une politique esclave NEVER.
  {
    const bench_profile_t noirq = {
        .profile_id = "cc", .seed = 7, .transaction_count = 1, .packet_size = 8,
        .timeout_ticks = 100, .irq_policy = BENCH_IRQ_NEVER};
    fault_cfg_t cfg = {MODE_CONCURRENT_IRQ, 0};
    run_faulted(&noirq, &cfg, &h, &s);
    EXPECT_EQ_INT(s.irq_count, 0, "esclave n'emet pas (policy NEVER)");
    EXPECT(h.irq_count >= 1, "hote voit l'IRQ concurrente");
  }

  // Rafale : transactions successives sans delai, toutes traitees.
  {
    const bench_profile_t burst = {
        .profile_id = "burst", .seed = 7, .transaction_count = 16, .packet_size = 8,
        .inter_delay_ticks = 0, .timeout_ticks = 500,
        .irq_policy = BENCH_IRQ_PER_TRANSACTION};
    bench_sim_link_t link;
    const bench_irq_out_t irq_out = bench_sim_link_irq_out(&link);
    const bench_clock_t clock = bench_sim_link_clock(&link);
    bench_slave_engine_t slave;
    bench_slave_engine_init(&slave, clock, irq_out, NULL, NULL, burst.irq_policy);
    bench_sim_link_init(&link, bench_slave_engine_port(&slave));
    bench_spi_master_t master = bench_sim_link_master(&link);
    const bench_irq_in_t irq_in = bench_sim_link_irq_in(&link);
    const bench_scenario_t sc = {
        .scenario_id = "b", .scenario_version = 1, .profile = &burst};
    bench_host_engine_t host;
    bench_host_engine_init(&host, &sc, &master, irq_in, clock, NULL, NULL);
    bench_host_engine_run(&host);
    EXPECT_EQ_INT(host.counters.tx_ok, 16, "rafale : 16 transactions reussies");
  }
}

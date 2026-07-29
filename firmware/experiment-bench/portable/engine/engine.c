// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "engine/engine.h"

#include <string.h>

#include "profiles/profile.h"
#include "transport/transport.h"

static bench_ticks_t clock_now_or_zero(const bench_clock_t *clock) {
  return (clock->now != NULL) ? bench_clock_now(clock) : 0;
}

static void emit(bench_event_sink_t sink, void *ctx, uint32_t *seq,
                 bench_event_type_t type, bench_ticks_t at) {
  bench_event_t ev = {type, (*seq)++, at};
  if (sink != NULL) {
    sink(ctx, &ev);
  }
}

// --------------------------------------------------------------------------
// Moteur ESCLAVE
// --------------------------------------------------------------------------
static void slave_prepare_response(void *ctx, const uint8_t *resp, size_t len) {
  bench_slave_engine_t *e = (bench_slave_engine_t *)ctx;
  if (len > sizeof(e->response)) {
    len = sizeof(e->response);
  }
  memcpy(e->response, resp, len);
  e->response_len = len;
}

static void slave_on_select(void *ctx) {
  bench_slave_engine_t *e = (bench_slave_engine_t *)ctx;
  e->status = BENCH_SLAVE_SELECTED;
}

static void slave_set_ack(bench_slave_engine_t *e, uint32_t seq, bench_ack_t ack) {
  const uint8_t payload[1] = {(uint8_t)ack};
  const int n = bench_frame_encode(e->response, sizeof(e->response), seq, payload, 1);
  e->response_len = (n > 0) ? (size_t)n : 0;
}

static size_t slave_on_transaction(void *ctx, const uint8_t *rx, size_t len) {
  bench_slave_engine_t *e = (bench_slave_engine_t *)ctx;
  e->status = BENCH_SLAVE_ACTIVE;

  bench_frame_t f;
  size_t consumed = 0;
  const bench_frame_result_t r = bench_frame_decode_prefix(rx, len, &f, &consumed);
  const bench_ticks_t at = clock_now_or_zero(&e->clock);

  if (r == BENCH_FRAME_OK) {
    bench_counters_record_tx(&e->counters, 1, (uint32_t)f.payload_len, 0);
    slave_set_ack(e, f.seq, BENCH_ACK_OK);
  } else {
    // Corruption REELLE detectee (CRC/format) : erreur comptee ET tx echouee.
    bench_counters_record_crc_error(&e->counters);
    bench_counters_record_tx(&e->counters, 0, 0, 0);
    slave_set_ack(e, 0, BENCH_ACK_CRC_ERROR);
  }

  // Signale la completion : leve l'IRQ vers l'hote + evenement.
  if (e->irq.raise != NULL) {
    e->irq.raise(e->irq.ctx);
  }
  bench_counters_record_irq(&e->counters);
  emit(e->sink, e->sink_ctx, &e->event_seq, BENCH_EV_IRQ, at);
  e->status = BENCH_SLAVE_COMPLETE;
  return len;
}

static size_t slave_peek_response(void *ctx, uint8_t *out, size_t cap) {
  bench_slave_engine_t *e = (bench_slave_engine_t *)ctx;
  const size_t n = (e->response_len < cap) ? e->response_len : cap;
  memcpy(out, e->response, n);
  return n;
}

static bench_slave_txn_status_t slave_txn_status(void *ctx) {
  const bench_slave_engine_t *e = (const bench_slave_engine_t *)ctx;
  return e->status;
}

void bench_slave_engine_init(bench_slave_engine_t *engine, bench_clock_t clock,
                             bench_irq_out_t irq, bench_event_sink_t sink,
                             void *sink_ctx) {
  memset(engine, 0, sizeof(*engine));
  bench_counters_reset(&engine->counters);
  engine->clock = clock;
  engine->irq = irq;
  engine->sink = sink;
  engine->sink_ctx = sink_ctx;
  engine->status = BENCH_SLAVE_IDLE;
}

bench_spi_slave_t bench_slave_engine_port(bench_slave_engine_t *engine) {
  bench_spi_slave_t port;
  port.ctx = engine;
  port.prepare_response = slave_prepare_response;
  port.on_select = slave_on_select;
  port.on_transaction = slave_on_transaction;
  port.peek_response = slave_peek_response;
  port.transaction_status = slave_txn_status;
  return port;
}

// --------------------------------------------------------------------------
// Moteur HOTE
// --------------------------------------------------------------------------
void bench_host_engine_init(bench_host_engine_t *engine,
                            const bench_scenario_t *scenario,
                            bench_spi_master_t *spi, bench_irq_in_t irq,
                            bench_clock_t clock, bench_event_sink_t sink,
                            void *sink_ctx) {
  engine->scenario = scenario;
  engine->spi = spi;
  engine->irq = irq;
  engine->clock = clock;
  engine->sink = sink;
  engine->sink_ctx = sink_ctx;
  bench_counters_reset(&engine->counters);
  bench_profile_seed(scenario->profile, &engine->rng);
  engine->event_seq = 0;
}

int bench_host_engine_step(bench_host_engine_t *e, uint32_t index) {
  const bench_profile_t *prof = e->scenario->profile;
  const bench_ticks_t now0 = clock_now_or_zero(&e->clock);
  uint32_t psize = prof->packet_size;
  if (psize > BENCH_FRAME_MAX_PAYLOAD) {
    psize = BENCH_FRAME_MAX_PAYLOAD;
  }

  bench_txn_t txn;
  bench_txn_begin(&txn, BENCH_TRANSPORT_SPI, psize, now0, prof->timeout_ticks);
  emit(e->sink, e->sink_ctx, &e->event_seq, BENCH_EV_TX_BEGIN, now0);

  // Injection de TIMEOUT reelle, via l'ordonnanceur wrap-safe.
  if (bench_profile_fault_timeout(prof, index)) {
    const bench_ticks_t late = now0 + prof->timeout_ticks + 1;
    (void)bench_txn_advance(&txn, 0, late);  // -> BENCH_TXN_TIMEOUT
    bench_counters_record_timeout(&e->counters);
    bench_counters_record_tx(&e->counters, 0, 0, bench_txn_latency(&txn, late));
    emit(e->sink, e->sink_ctx, &e->event_seq, BENCH_EV_TIMEOUT, late);
    emit(e->sink, e->sink_ctx, &e->event_seq, BENCH_EV_TX_END, late);
    return 0;
  }

  // Trame de donnees deterministe (contenu derive du generateur seede).
  uint8_t payload[BENCH_FRAME_MAX_PAYLOAD];
  for (uint32_t i = 0; i < psize; i++) {
    payload[i] = (uint8_t)(bench_profile_next(&e->rng) & 0xFFu);
  }
  uint8_t tx[BENCH_FRAME_MAX_SIZE];
  const int enc = bench_frame_encode(tx, sizeof(tx), index, payload, psize);
  if (enc <= 0) {
    return -1;  // ne devrait pas arriver : psize est borne a MAX_PAYLOAD
  }

  // Injection de CRC reelle : corrompt un octet AVANT l'envoi.
  if (bench_profile_fault_crc(prof, index)) {
    if (psize > 0) {
      (void)bench_frame_corrupt_payload(tx, (size_t)enc, 0);
    } else {
      (void)bench_frame_corrupt_crc(tx, (size_t)enc);
    }
  }

  uint8_t rx[BENCH_FRAME_MAX_SIZE];
  memset(rx, 0, sizeof(rx));
  const bench_spi_status_t st = e->spi->transfer(e->spi->ctx, tx, rx, (size_t)enc);
  const bench_ticks_t now1 = clock_now_or_zero(&e->clock);
  if (st != BENCH_SPI_OK) {
    (void)bench_txn_advance(&txn, 0, now1);
    bench_counters_record_tx(&e->counters, 0, 0, bench_txn_latency(&txn, now1));
    emit(e->sink, e->sink_ctx, &e->event_seq, BENCH_EV_TX_END, now1);
    return -1;
  }

  // Decode la reponse (ACK) et verifie son integrite.
  bench_frame_t resp;
  size_t consumed = 0;
  const bench_frame_result_t rr =
      bench_frame_decode_prefix(rx, (size_t)enc, &resp, &consumed);
  int ok = 0;
  if (rr == BENCH_FRAME_OK && resp.payload_len >= 1 &&
      resp.payload[0] == (uint8_t)BENCH_ACK_OK) {
    ok = 1;
  } else {
    bench_counters_record_crc_error(&e->counters);  // rejet cote esclave ou reponse corrompue
  }

  (void)bench_txn_advance(&txn, psize, now1);
  const bench_ticks_t lat = bench_txn_latency(&txn, now1);
  bench_counters_record_tx(&e->counters, ok, psize, lat);

  if (e->irq.get != NULL && e->irq.get(e->irq.ctx)) {
    bench_counters_record_irq(&e->counters);
    emit(e->sink, e->sink_ctx, &e->event_seq, BENCH_EV_IRQ, now1);
  }
  emit(e->sink, e->sink_ctx, &e->event_seq, BENCH_EV_TX_END, now1);
  return 0;
}

void bench_host_engine_run(bench_host_engine_t *e) {
  const uint32_t budget = bench_scenario_transaction_budget(e->scenario);
  for (uint32_t i = 0; i < budget; i++) {
    (void)bench_host_engine_step(e, i);
  }
}

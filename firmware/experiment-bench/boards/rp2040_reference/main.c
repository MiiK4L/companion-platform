// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Croquis d'INTEGRATION du banc, cible RP2040 (ESCLAVE = simulateur CX-Bus).
//
// SQUELETTE — NON compile ni flashe ici, NON couvert par la CI. Il montre le
// CABLAGE : injection de la HAL dans le coeur PORTABLE, boucle de service pilotee
// par un profil DECLARATIF, comptage BRUT, emission d'EVENEMENTS, reponse au
// protocole de controle. Toute la logique vient de portable/ (testee en CI) ;
// seule la HAL (hal/rp2040) touche le materiel. Aucun contenu de campagne L1.

#include <stddef.h>
#include <stdint.h>

#include "counters/counters.h"
#include "hal/rp2040/hal_rp2040.h"
#include "profiles/profile.h"
#include "protocol/protocol.h"
#include "scenarios/demo/demo_profile.h"
#include "transport/transport.h"

// Emet un evenement horodate via le puits fourni par la HAL.
static void emit(const bench_hal_t *hal, bench_event_type_t type, uint32_t seq) {
  if (hal->event_sink == NULL) {
    return;
  }
  bench_event_t ev = {type, seq, hal->now(hal->ctx)};
  hal->event_sink(hal->event_ctx, &ev);
}

// Execute une transaction du profil ; met a jour les compteurs BRUTS. Toute la
// decision (faute injectee, timeout) vient du coeur portable deterministe.
static void run_transaction(const bench_hal_t *hal, const bench_profile_t *profile,
                            bench_counters_t *counters, uint32_t index,
                            uint32_t *seq) {
  bench_ticks_t start = hal->now(hal->ctx);
  bench_txn_t txn;
  bench_txn_begin(&txn, BENCH_TRANSPORT_SPI, profile->packet_size, start,
                  profile->timeout_ticks);
  emit(hal, BENCH_EV_TX_BEGIN, (*seq)++);

  if (bench_profile_fault_timeout(profile, index)) {
    // Faute deterministe : on n'avance pas, l'echeance expire.
    (void)bench_txn_advance(&txn, 0, start + profile->timeout_ticks + 1);
    bench_counters_record_timeout(counters);
    emit(hal, BENCH_EV_TIMEOUT, (*seq)++);
    return;
  }

  uint8_t tx[256];
  uint8_t rx[256];
  uint32_t chunk = profile->packet_size <= sizeof(tx) ? profile->packet_size
                                                      : (uint32_t)sizeof(tx);
  uint32_t moved = hal->spi_xfer(hal->ctx, tx, rx, chunk);
  bench_ticks_t nowt = hal->now(hal->ctx);
  bench_txn_state_t st = bench_txn_advance(&txn, moved, nowt);

  if (bench_profile_fault_crc(profile, index)) {
    bench_counters_record_crc_error(counters);
    emit(hal, BENCH_EV_CRC_ERROR, (*seq)++);
    bench_counters_record_tx(counters, 0, moved, bench_txn_latency(&txn, nowt));
  } else {
    int ok = (st == BENCH_TXN_DONE);
    bench_counters_record_tx(counters, ok, moved, bench_txn_latency(&txn, nowt));
  }
  emit(hal, BENCH_EV_TX_END, (*seq)++);
}

int main(void) {
  bench_hal_t hal = bench_hal_rp2040();
  const bench_profile_t *profile = &bench_demo_profile;  // remplace par SELECT

  bench_counters_t counters;
  bench_counters_reset(&counters);
  uint64_t rng;
  bench_profile_seed(profile, &rng);
  uint32_t seq = 0;

  // Boucle de service : execute le profil declaratif de maniere rejouable.
  for (uint32_t i = 0; i < profile->transaction_count; ++i) {
    if (hal.irq_get(hal.ctx)) {
      bench_counters_record_irq(&counters);
      emit(&hal, BENCH_EV_IRQ, seq++);
    }
    run_transaction(&hal, profile, &counters, i, &seq);
    (void)bench_profile_next(&rng);  // avance le flux pseudo-aleatoire seede
  }

  // Restitution des compteurs BRUTS via le protocole (ligne stable).
  char line[256];
  if (bench_protocol_format_counters(line, sizeof(line), &counters) >= 0) {
    hal.serial_write(hal.ctx, line);
  }
  return 0;
}

// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Enregistrement de TELEMETRIE par transaction (format de flux v4).
//
// DEUX NUMEROTATIONS COEXISTENT, et aucune n'est reconstruite depuis l'autre :
//   - global_event_seq    : ordre GLOBAL du flux, strictement monotone, tous
//                           producteurs confondus ; il capture l'ENTRELACEMENT ;
//   - producer_sequence_id: sequence PROPRE au producteur, continue ; elle
//                           survit a l'entrelacement et aux pertes.
// Deduire l'une de l'autre serait faux des qu'il y a deux producteurs ou une
// lacune : les deux sont donc transportees explicitement.
//
// QUATRE INSTANTS de bus sont transportes en TICKS BRUTS :
//   t_request : demande du bus        t_grant  : obtention du bus
//   t_release : liberation du bus     t_end    : fin de la transaction
// Les durees en sont DERIVEES par l'outillage, jamais transportees :
//   bus_wait_ticks = t_grant   - t_request
//   bus_hold_ticks = t_release - t_grant
// Transporter une duree redondante autoriserait une contradiction avec ses
// entrees ; les invariants ci-dessus sont verifiables cote analyse.
//
// Un echantillon porte UN SEUL statut terminal ET une CAUSE de timeout
// explicite, afin qu'une attente de bus ne soit jamais confondue avec un
// peripherique muet.
#ifndef BENCH_TELEMETRY_RECORD_H
#define BENCH_TELEMETRY_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "events/events.h"  // bench_ticks_t

// Statut TERMINAL d'un echantillon (un seul par transaction).
typedef enum {
  BENCH_SAMPLE_OK = 0,
  BENCH_SAMPLE_TIMEOUT = 1,
  BENCH_SAMPLE_REJECTED = 2,
  BENCH_SAMPLE_UNPAIRED = 3,
  BENCH_SAMPLE_DUPLICATE = 4,
  BENCH_SAMPLE_OUT_OF_ORDER = 5,
} bench_sample_status_t;

#define BENCH_SAMPLE_FLAG_FAULT_CRC 0x01u
#define BENCH_SAMPLE_FLAG_FAULT_TIMEOUT 0x02u

typedef struct {
  uint8_t producer_id;
  uint32_t producer_sequence_id;  // sequence LOCALE, continue
  uint32_t global_event_seq;      // ordre GLOBAL, strictement monotone
  bench_ticks_t t_request;
  bench_ticks_t t_grant;
  bench_ticks_t t_release;
  bench_ticks_t t_end;
  uint8_t status;         // bench_sample_status_t
  uint8_t timeout_cause;  // bench_timeout_cause_t
  uint8_t flags;
} bench_sample_t;

// Taille encodee, FIXE : u8 + u32 + u32 + 4 x u64 + u8 + u8 + u8.
#define BENCH_SAMPLE_WIRE_SIZE 44u

size_t bench_sample_encode(uint8_t *out, size_t cap, const bench_sample_t *s);
size_t bench_sample_decode(const uint8_t *in, size_t len, bench_sample_t *out);

#endif  // BENCH_TELEMETRY_RECORD_H

// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "telemetry/telemetry.h"

#include <string.h>

#include "frame/frame.h"
#include "util/sat.h"
#include "util/wire.h"

#define BENCH_RING_NO_SEQ 0xFFFFFFFFu

// Encapsule un payload de message dans une trame B2 et le pousse au puits.
// Retourne 1 si la trame a ete acceptee ENTIEREMENT, 0 sinon (refus compte).
static int emit_frame(bench_telemetry_t *tm, const uint8_t *payload, size_t len) {
  uint8_t frame[BENCH_FRAME_MAX_SIZE];
  const int n = bench_frame_encode(frame, sizeof(frame), tm->stream_seq, payload, len);
  if (n <= 0) {
    bench_sat_inc_u32(&tm->frames_refused);
    return 0;
  }
  // Le numero de sequence avance MEME si le puits refuse : la discontinuite
  // reste ainsi detectable cote capture (transport_gap).
  tm->stream_seq++;
  if (tm->sink.write_frame == NULL ||
      !tm->sink.write_frame(tm->sink.ctx, frame, (size_t)n)) {
    bench_sat_inc_u32(&tm->frames_refused);
    return 0;
  }
  return 1;
}

// Ecrit une chaine bornee : longueur (u8) puis octets. NULL => longueur 0.
static size_t put_str(uint8_t *buf, size_t off, const char *s, size_t cap) {
  size_t n = 0;
  if (s != NULL) {
    while (s[n] != '\0' && n < 255u) {
      n++;
    }
  }
  if (off + 1 + n > cap) {
    return off;  // ne deborde jamais ; la chaine est omise
  }
  buf[off++] = (uint8_t)n;
  if (n > 0) {
    memcpy(buf + off, s, n);
    off += n;
  }
  return off;
}

void bench_telemetry_init(bench_telemetry_t *tm, bench_telemetry_sink_t sink,
                          bench_ring_t *ring) {
  memset(tm, 0, sizeof(*tm));
  tm->sink = sink;
  tm->ring = ring;
  tm->stream_seq = 0;
}

int bench_telemetry_emit_header(bench_telemetry_t *tm,
                                const bench_telemetry_header_t *h) {
  uint8_t body[BENCH_FRAME_MAX_PAYLOAD];
  size_t off = 0;
  body[off++] = (uint8_t)BENCH_TM_HEADER;
  bench_wire_put_u16(body + off, (uint16_t)BENCH_TELEMETRY_STREAM_VERSION);
  off += 2;
  body[off++] = h->clock_id;
  body[off++] = h->tick_width_bits;
  body[off++] = h->wrap_policy;
  bench_wire_put_u64(body + off, h->tick_hz);
  off += 8;
  bench_wire_put_u32(body + off, h->ring_capacity);
  off += 4;
  body[off++] = h->histogram_enabled;
  bench_wire_put_u16(body + off, h->histogram_version);
  off += 2;
  off = put_str(body, off, h->profile_id, sizeof(body));
  off = put_str(body, off, h->variant, sizeof(body));
  off = put_str(body, off, h->mode, sizeof(body));
  return emit_frame(tm, body, off);
}

uint32_t bench_telemetry_drain(bench_telemetry_t *tm, uint32_t max_messages) {
  uint32_t emitted = 0;
  if (tm->ring == NULL) {
    return 0;
  }
  while (emitted < max_messages) {
    // 1) Toute lacune en attente est signalee AVANT les echantillons suivants,
    //    pour qu'elle soit situee a sa position exacte dans le flux.
    uint32_t after_seq = BENCH_RING_NO_SEQ;
    const uint32_t lost = bench_ring_take_gap(tm->ring, &after_seq);
    if (lost > 0) {
      uint8_t body[9];
      body[0] = (uint8_t)BENCH_TM_GAP;
      bench_wire_put_u32(body + 1, lost);
      bench_wire_put_u32(body + 5, after_seq);
      (void)emit_frame(tm, body, sizeof(body));
      emitted++;
      continue;
    }
    // 2) Puis un echantillon.
    bench_sample_t s;
    if (!bench_ring_pop(tm->ring, &s)) {
      break;  // plus rien a emettre
    }
    uint8_t body[1 + BENCH_SAMPLE_WIRE_SIZE];
    body[0] = (uint8_t)BENCH_TM_SAMPLE;
    (void)bench_sample_encode(body + 1, BENCH_SAMPLE_WIRE_SIZE, &s);
    (void)emit_frame(tm, body, sizeof(body));
    emitted++;
  }
  return emitted;
}

int bench_telemetry_emit_summary(bench_telemetry_t *tm,
                                 const bench_telemetry_summary_t *s) {
  uint8_t body[1 + 8 * 4 + 8];
  size_t off = 0;
  body[off++] = (uint8_t)BENCH_TM_SUMMARY;
  bench_wire_put_u32(body + off, s->issued); off += 4;
  bench_wire_put_u32(body + off, s->ok); off += 4;
  bench_wire_put_u32(body + off, s->timeout); off += 4;
  bench_wire_put_u32(body + off, s->rejected); off += 4;
  bench_wire_put_u32(body + off, s->unpaired); off += 4;
  bench_wire_put_u32(body + off, s->duplicate); off += 4;
  bench_wire_put_u32(body + off, s->out_of_order); off += 4;
  bench_wire_put_u32(body + off, s->producer_drop); off += 4;
  bench_wire_put_u64(body + off, s->timeout_budget_ticks); off += 8;
  return emit_frame(tm, body, off);
}

int bench_telemetry_emit_histogram(bench_telemetry_t *tm,
                                   const bench_histogram_t *h) {
  if (h == NULL || h->edges == NULL || h->counts == NULL || h->bin_count == 0) {
    return 0;  // desactive : on n'emet rien plutot que d'inventer un contenu
  }
  uint8_t body[BENCH_FRAME_MAX_PAYLOAD];
  size_t off = 0;
  body[off++] = (uint8_t)BENCH_TM_HISTOGRAM;
  bench_wire_put_u32(body + off, h->version); off += 4;
  bench_wire_put_u32(body + off, h->bin_count); off += 4;
  bench_wire_put_u32(body + off, h->underflow); off += 4;
  bench_wire_put_u32(body + off, h->overflow); off += 4;
  bench_wire_put_u32(body + off, h->sample_count); off += 4;
  const size_t need = off + (size_t)(h->bin_count + 1) * 8u + (size_t)h->bin_count * 4u;
  if (need > sizeof(body)) {
    return 0;  // configuration hors gabarit de trame
  }
  for (uint32_t i = 0; i <= h->bin_count; i++) {
    bench_wire_put_u64(body + off, h->edges[i]);
    off += 8;
  }
  for (uint32_t i = 0; i < h->bin_count; i++) {
    bench_wire_put_u32(body + off, h->counts[i]);
    off += 4;
  }
  return emit_frame(tm, body, off);
}

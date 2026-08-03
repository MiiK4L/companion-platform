// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "telemetry/telemetry.h"

#include <string.h>

#include "frame/frame.h"
#include "util/sat.h"
#include "util/wire.h"

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
  bench_sat_inc_u32(&tm->frames_accepted);
  return 1;
}

// Longueur d'une chaine si elle est VALIDE (ASCII imprimable et bornee).
// Retourne -1 si elle doit faire REJETER l'en-tete. NULL vaut chaine vide.
static int checked_str_len(const char *s) {
  if (s == NULL) {
    return 0;
  }
  size_t n = 0;
  while (s[n] != '\0') {
    const unsigned char c = (unsigned char)s[n];
    if (c < 0x20u || c > 0x7Eu) {
      return -1;  // caractere non imprimable : rejet explicite
    }
    n++;
    if (n > BENCH_TELEMETRY_STR_MAX) {
      return -1;  // trop longue : rejet, jamais de troncature
    }
  }
  return (int)n;
}

static size_t put_str(uint8_t *buf, size_t off, const char *s, int len) {
  buf[off++] = (uint8_t)len;
  if (len > 0) {
    memcpy(buf + off, s, (size_t)len);
    off += (size_t)len;
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
  // Validation AVANT toute ecriture : aucune trame partielle, aucune omission.
  const int lp = checked_str_len(h->profile_id);
  const int lv = checked_str_len(h->variant);
  const int lm = checked_str_len(h->mode);
  if (lp < 0 || lv < 0 || lm < 0) {
    return 0;
  }
  const size_t fixed = 1 + 2 + 1 + 1 + 1 + 8 + 4 + 1 + 2;
  const size_t need = fixed + 3 + (size_t)lp + (size_t)lv + (size_t)lm;
  if (need > BENCH_FRAME_MAX_PAYLOAD) {
    return 0;  // ne tient pas dans une trame : rejet explicite
  }

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
  off = put_str(body, off, h->profile_id, lp);
  off = put_str(body, off, h->variant, lv);
  off = put_str(body, off, h->mode, lm);
  return emit_frame(tm, body, off);
}

uint32_t bench_telemetry_drain(bench_telemetry_t *tm, uint32_t max_messages) {
  uint32_t emitted = 0;
  if (tm->ring == NULL) {
    return 0;
  }
  while (emitted < max_messages) {
    // 1) Une lacune n'est emise que lorsqu'elle est ECHUE, c'est-a-dire une fois
    //    depiles tous les echantillons qui la precedent chronologiquement.
    bench_gap_record_t gap;
    if (bench_ring_peek_gap(tm->ring, &gap)) {
      uint8_t body[9];
      body[0] = (uint8_t)BENCH_TM_GAP;
      bench_wire_put_u32(body + 1, gap.lost_count);
      bench_wire_put_u32(body + 5, gap.after_seq);
      if (!emit_frame(tm, body, sizeof(body))) {
        break;  // refus : on NE consomme PAS, la lacune sera reemise
      }
      bench_ring_commit_gap(tm->ring);  // consommation APRES acceptation
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
    if (!emit_frame(tm, body, sizeof(body))) {
      continue;  // trame perdue : detectable cote capture par la sequence
    }
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
  body[off++] = (uint8_t)(h->saturated ? 1 : 0);
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

int bench_telemetry_emit_footer(bench_telemetry_t *tm, uint32_t samples_attempted) {
  uint8_t body[1 + 5 * 4];
  size_t off = 0;
  body[off++] = (uint8_t)BENCH_TM_FOOTER;
  // Le footer porte SON PROPRE numero de sequence : le parseur en deduit le
  // nombre exact de trames attendues, y compris celles perdues en fin de flux.
  bench_wire_put_u32(body + off, tm->stream_seq); off += 4;
  bench_wire_put_u32(body + off, tm->stream_seq + 1u); off += 4;  // frames_attempted
  bench_wire_put_u32(body + off, tm->frames_accepted); off += 4;
  bench_wire_put_u32(body + off, tm->frames_refused); off += 4;
  bench_wire_put_u32(body + off, samples_attempted); off += 4;
  return emit_frame(tm, body, off);
}

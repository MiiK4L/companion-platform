// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Telemetrie : codec, tampon borne (ORDRE des lacunes, consommation
// TRANSACTIONNELLE), histogramme optionnel (convention, saturation), flux
// (trames CRC, sequence monotone, refus du puits, cloture autoritaire).
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "frame/frame.h"
#include "scheduler/scheduler.h"
#include "telemetry/histogram.h"
#include "telemetry/record.h"
#include "telemetry/ring.h"
#include "telemetry/telemetry.h"
#include "tests/test_support.h"
#include "tests/tests.h"

// --- Puits de test : capture les trames, refus programmable -----------------
typedef struct {
  uint8_t buf[8192];
  size_t len;
  uint32_t frames;
  int refuse_from;   // -1 : accepte tout ; sinon refuse a partir de cet index
  int refuse_count;  // nombre de refus restants (-1 = illimite)
} cap_t;

static int cap_write(void *ctx, const uint8_t *frame, size_t len) {
  cap_t *c = (cap_t *)ctx;
  if (c->refuse_from >= 0 && (int)c->frames >= c->refuse_from &&
      c->refuse_count != 0) {
    c->frames++;
    if (c->refuse_count > 0) {
      c->refuse_count--;
    }
    return 0;  // refus ATOMIQUE : rien n'est ecrit
  }
  if (c->len + len > sizeof(c->buf)) {
    return 0;
  }
  memcpy(c->buf + c->len, frame, len);
  c->len += len;
  c->frames++;
  return 1;
}

static void cap_init(cap_t *c) {
  memset(c, 0, sizeof(*c));
  c->refuse_from = -1;
  c->refuse_count = -1;
}

// Retourne le type du n-ieme message du flux capture (-1 si absent).
static int msg_type_at(const cap_t *c, uint32_t index) {
  size_t off = 0;
  uint32_t i = 0;
  while (off < c->len) {
    bench_frame_t f;
    size_t consumed = 0;
    if (bench_frame_decode_prefix(c->buf + off, c->len - off, &f, &consumed) !=
        BENCH_FRAME_OK) {
      return -1;
    }
    if (i == index) {
      return f.payload_len > 0 ? (int)f.payload[0] : -1;
    }
    i++;
    off += consumed;
  }
  return -1;
}

static uint32_t msg_u32_at(const cap_t *c, uint32_t index, size_t field_off) {
  size_t off = 0;
  uint32_t i = 0;
  while (off < c->len) {
    bench_frame_t f;
    size_t consumed = 0;
    if (bench_frame_decode_prefix(c->buf + off, c->len - off, &f, &consumed) !=
        BENCH_FRAME_OK) {
      return 0;
    }
    if (i == index) {
      return ((uint32_t)f.payload[field_off] << 24) |
             ((uint32_t)f.payload[field_off + 1] << 16) |
             ((uint32_t)f.payload[field_off + 2] << 8) |
             (uint32_t)f.payload[field_off + 3];
    }
    i++;
    off += consumed;
  }
  return 0;
}

static void test_record_codec(void) {
  const bench_sample_t s = {42, 1000, 1250, BENCH_SAMPLE_OK,
                            BENCH_SAMPLE_FLAG_FAULT_CRC};
  uint8_t buf[BENCH_SAMPLE_WIRE_SIZE];
  EXPECT_EQ_INT(bench_sample_encode(buf, sizeof(buf), &s), BENCH_SAMPLE_WIRE_SIZE,
                "taille d'encodage fixe");
  EXPECT_EQ_INT(bench_sample_encode(buf, 4, &s), 0, "capacite insuffisante refusee");

  bench_sample_t out;
  EXPECT_EQ_INT(bench_sample_decode(buf, sizeof(buf), &out), BENCH_SAMPLE_WIRE_SIZE,
                "decodage");
  EXPECT_EQ_INT(out.sequence_id, 42, "sequence restituee");
  EXPECT_EQ_INT((long long)out.t_start, 1000, "t_start restitue");
  EXPECT_EQ_INT((long long)out.t_end, 1250, "t_end restitue");
  EXPECT_EQ_INT(out.flags, BENCH_SAMPLE_FLAG_FAULT_CRC, "drapeaux restitues");
  EXPECT_EQ_INT(bench_sample_decode(buf, 3, &out), 0, "decodage tronque refuse");
}

// --- Point 1 : la lacune suit chronologiquement les echantillons ------------
static void test_ordre_de_la_lacune(void) {
  bench_sample_t slots[4];
  bench_gap_record_t gaps[5];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 4, gaps, 5);

  bench_sample_t s = {0, 0, 10, BENCH_SAMPLE_OK, 0};
  for (uint32_t i = 0; i < 4; i++) {  // remplit
    s.sequence_id = i;
    EXPECT_EQ_INT(bench_ring_push(&ring, &s), 1, "stockage");
  }
  for (uint32_t i = 4; i < 6; i++) {  // deborde : pertes 4 et 5
    s.sequence_id = i;
    EXPECT_EQ_INT(bench_ring_push(&ring, &s), 0, "perte");
  }
  EXPECT_EQ_INT(ring.producer_drop, 2, "2 pertes");

  // La lacune n'est PAS echue tant que 0..3 n'ont pas ete depiles.
  bench_gap_record_t g;
  EXPECT_EQ_INT(bench_ring_peek_gap(&ring, &g), 0, "lacune non echue avant depilage");

  bench_sample_t got;
  for (uint32_t i = 0; i < 4; i++) {
    EXPECT_EQ_INT(bench_ring_pop(&ring, &got), 1, "depilage");
    EXPECT_EQ_INT(got.sequence_id, i, "ordre FIFO");
    if (i < 3) {
      EXPECT_EQ_INT(bench_ring_peek_gap(&ring, &g), 0, "toujours pas echue");
    }
  }
  EXPECT_EQ_INT(bench_ring_peek_gap(&ring, &g), 1, "echue apres le dernier depilage");
  EXPECT_EQ_INT(g.lost_count, 2, "2 pertes dans la plage");
  EXPECT_EQ_INT(g.after_seq, 3, "lacune situee APRES la sequence 3");
}

// Le flux doit porter SAMPLE(0..3) PUIS GAP, jamais l'inverse.
static void test_ordre_dans_le_flux(void) {
  cap_t cap;
  cap_init(&cap);
  bench_sample_t slots[4];
  bench_gap_record_t gaps[5];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 4, gaps, 5);
  bench_telemetry_sink_t sink = {&cap, cap_write};
  bench_telemetry_t tm;
  bench_telemetry_init(&tm, sink, &ring);

  bench_sample_t s = {0, 0, 10, BENCH_SAMPLE_OK, 0};
  for (uint32_t i = 0; i < 6; i++) {
    s.sequence_id = i;
    (void)bench_ring_push(&ring, &s);
  }
  EXPECT_EQ_INT(bench_telemetry_drain(&tm, 64), 5, "4 echantillons + 1 lacune");
  for (uint32_t i = 0; i < 4; i++) {
    EXPECT_EQ_INT(msg_type_at(&cap, i), BENCH_TM_SAMPLE, "echantillon avant la lacune");
  }
  EXPECT_EQ_INT(msg_type_at(&cap, 4), BENCH_TM_GAP, "lacune APRES les echantillons");
  EXPECT_EQ_INT(msg_u32_at(&cap, 4, 1), 2, "2 pertes");
  EXPECT_EQ_INT(msg_u32_at(&cap, 4, 5), 3, "after_sequence_id = 3");
}

// Drain partiel, reprise, puis nouvelles transactions apres la lacune.
static void test_drain_partiel_et_reprise(void) {
  cap_t cap;
  cap_init(&cap);
  bench_sample_t slots[4];
  bench_gap_record_t gaps[5];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 4, gaps, 5);
  bench_telemetry_sink_t sink = {&cap, cap_write};
  bench_telemetry_t tm;
  bench_telemetry_init(&tm, sink, &ring);

  bench_sample_t s = {0, 0, 10, BENCH_SAMPLE_OK, 0};
  for (uint32_t i = 0; i < 6; i++) {  // 0..3 stockes, 4 et 5 perdus
    s.sequence_id = i;
    (void)bench_ring_push(&ring, &s);
  }
  EXPECT_EQ_INT(bench_telemetry_drain(&tm, 2), 2, "drain partiel : 2 messages");
  EXPECT_EQ_INT(msg_type_at(&cap, 0), BENCH_TM_SAMPLE, "toujours des echantillons");
  EXPECT_EQ_INT(msg_type_at(&cap, 1), BENCH_TM_SAMPLE, "toujours des echantillons");

  // Nouvelles transactions APRES la lacune : elles ne doivent pas la devancer.
  for (uint32_t i = 6; i < 8; i++) {
    s.sequence_id = i;
    EXPECT_EQ_INT(bench_ring_push(&ring, &s), 1, "depot apres liberation");
  }
  EXPECT_EQ_INT(bench_telemetry_drain(&tm, 64), 5, "2 restants + lacune + 2 nouveaux");
  EXPECT_EQ_INT(msg_type_at(&cap, 2), BENCH_TM_SAMPLE, "seq 2");
  EXPECT_EQ_INT(msg_type_at(&cap, 3), BENCH_TM_SAMPLE, "seq 3");
  EXPECT_EQ_INT(msg_type_at(&cap, 4), BENCH_TM_GAP, "lacune a sa place");
  EXPECT_EQ_INT(msg_type_at(&cap, 5), BENCH_TM_SAMPLE, "seq 6 apres la lacune");
  EXPECT_EQ_INT(msg_type_at(&cap, 6), BENCH_TM_SAMPLE, "seq 7 apres la lacune");
}

// Plusieurs plages de pertes DISTINCTES doivent rester distinctes.
static void test_plages_multiples(void) {
  cap_t cap;
  cap_init(&cap);
  bench_sample_t slots[2];
  bench_gap_record_t gaps[4];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 2, gaps, 4);
  bench_telemetry_sink_t sink = {&cap, cap_write};
  bench_telemetry_t tm;
  bench_telemetry_init(&tm, sink, &ring);

  bench_sample_t s = {0, 0, 10, BENCH_SAMPLE_OK, 0};
  // Plage A : remplit (0,1) puis perd 2.
  for (uint32_t i = 0; i < 3; i++) {
    s.sequence_id = i;
    (void)bench_ring_push(&ring, &s);
  }
  bench_sample_t got;
  (void)bench_ring_pop(&ring, &got);  // libere une place
  s.sequence_id = 10;
  EXPECT_EQ_INT(bench_ring_push(&ring, &s), 1, "nouveau depot accepte");
  s.sequence_id = 11;
  EXPECT_EQ_INT(bench_ring_push(&ring, &s), 0, "plage B : nouvelle perte");

  EXPECT_EQ_INT(ring.gap_count, 2, "deux plages DISTINCTES conservees");
  EXPECT_EQ_INT(gaps[0].after_seq, 1, "plage A apres seq 1");
  EXPECT_EQ_INT(gaps[0].lost_count, 1, "plage A : 1 perte");
  EXPECT_EQ_INT(gaps[1].after_seq, 10, "plage B apres seq 10");
  EXPECT_EQ_INT(gaps[1].lost_count, 1, "plage B : 1 perte");
}

// Point 3 : un refus du puits ne detruit pas le marqueur ; il est reemis.
static void test_refus_du_marqueur_puis_reemission(void) {
  cap_t cap;
  cap_init(&cap);
  bench_sample_t slots[2];
  bench_gap_record_t gaps[3];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 2, gaps, 3);
  bench_telemetry_sink_t sink = {&cap, cap_write};
  bench_telemetry_t tm;
  bench_telemetry_init(&tm, sink, &ring);

  bench_sample_t s = {0, 0, 10, BENCH_SAMPLE_OK, 0};
  for (uint32_t i = 0; i < 4; i++) {  // 0,1 stockes ; 2,3 perdus
    s.sequence_id = i;
    (void)bench_ring_push(&ring, &s);
  }
  // Refuse exactement la 3e trame (index 2) : ce sera le marqueur.
  cap.refuse_from = 2;
  cap.refuse_count = 1;
  EXPECT_EQ_INT(bench_telemetry_drain(&tm, 64), 2, "2 echantillons, marqueur refuse");
  EXPECT_EQ_INT(ring.gap_count, 1, "la lacune n'est PAS consommee apres un refus");

  // Nouveau drainage : la lacune doit etre REEMISE.
  cap.refuse_from = -1;
  EXPECT_EQ_INT(bench_telemetry_drain(&tm, 64), 1, "marqueur reemis");
  EXPECT_EQ_INT(ring.gap_count, 0, "lacune consommee apres acceptation");
  EXPECT_EQ_INT(msg_type_at(&cap, 2), BENCH_TM_GAP, "marqueur present dans le flux");
  EXPECT_EQ_INT(msg_u32_at(&cap, 2, 1), 2, "2 pertes conservees");
}

static void test_histogramme(void) {
  const bench_ticks_t edges[4] = {10, 20, 30, 40};
  uint32_t counts[3];
  bench_histogram_t h;
  EXPECT_EQ_INT(bench_histogram_init(&h, edges, 3, counts, 7), 1, "init valide");

  bench_histogram_add(&h, 9);
  bench_histogram_add(&h, 10);
  bench_histogram_add(&h, 19);
  bench_histogram_add(&h, 20);
  bench_histogram_add(&h, 39);
  bench_histogram_add(&h, 40);
  bench_histogram_add(&h, 100);

  EXPECT_EQ_INT(h.underflow, 1, "underflow");
  EXPECT_EQ_INT(h.overflow, 2, "overflow");
  EXPECT_EQ_INT(h.sample_count, 7, "sample_count");
  EXPECT_EQ_INT(counts[0], 2, "classe [10,20)");
  EXPECT_EQ_INT(counts[1], 1, "classe [20,30)");
  EXPECT_EQ_INT(counts[2], 1, "classe [30,40)");
  EXPECT_EQ_INT(h.saturated, 0, "non sature");

  const bench_ticks_t bad[3] = {10, 10, 20};
  bench_histogram_t hb;
  EXPECT_EQ_INT(bench_histogram_init(&hb, bad, 2, counts, 1), 0,
                "bornes non croissantes refusees");
  EXPECT_EQ_INT(bench_histogram_init(&hb, edges, 0, counts, 1), 0, "zero classe");
  EXPECT_EQ_INT(bench_histogram_init(&hb, edges, BENCH_HISTOGRAM_MAX_BINS + 1,
                                     counts, 1),
                0, "trop de classes");
  bench_histogram_add(&hb, 15);
  EXPECT_EQ_INT(hb.sample_count, 0, "histogramme desactive : aucun comptage");
}

// Point 9 : la saturation doit etre SIGNALEE, pas seulement plafonnee.
static void test_histogramme_sature(void) {
  bench_histogram_t hs;
  uint32_t c1[1];
  const bench_ticks_t e2[2] = {0, 100};
  EXPECT_EQ_INT(bench_histogram_init(&hs, e2, 1, c1, 1), 1, "init");
  hs.counts[0] = UINT32_MAX;
  bench_histogram_add(&hs, 50);
  EXPECT_EQ_INT(hs.counts[0], UINT32_MAX, "comptage plafonne");
  EXPECT_EQ_INT(hs.saturated, 1, "saturation SIGNALEE");

  bench_histogram_t hu;
  uint32_t c2[1];
  EXPECT_EQ_INT(bench_histogram_init(&hu, e2, 1, c2, 1), 1, "init");
  hu.underflow = UINT32_MAX;
  bench_histogram_add(&hu, 0);  // dans la classe, mais sample_count ok
  EXPECT_EQ_INT(hu.saturated, 0, "pas de saturation sans depassement");
  hu.sample_count = UINT32_MAX;
  bench_histogram_add(&hu, 50);
  EXPECT_EQ_INT(hu.saturated, 1, "saturation de sample_count signalee");
}

// Point 8 : les chaines d'en-tete ne peuvent pas etre omises silencieusement.
static void test_entete_chaines_rejetees(void) {
  cap_t cap;
  cap_init(&cap);
  bench_sample_t slots[2];
  bench_gap_record_t gaps[3];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 2, gaps, 3);
  bench_telemetry_sink_t sink = {&cap, cap_write};
  bench_telemetry_t tm;
  bench_telemetry_init(&tm, sink, &ring);

  static char toolong[BENCH_TELEMETRY_STR_MAX + 10];
  memset(toolong, 'x', sizeof(toolong) - 1);
  toolong[sizeof(toolong) - 1] = '\0';

  bench_telemetry_header_t hdr = {.clock_id = 1, .tick_width_bits = 64,
                                  .wrap_policy = 0, .tick_hz = 1,
                                  .ring_capacity = 2, .profile_id = toolong};
  EXPECT_EQ_INT(bench_telemetry_emit_header(&tm, &hdr), 0, "chaine trop longue rejetee");
  EXPECT_EQ_INT((long long)cap.len, 0, "AUCUNE trame partielle emise");

  const char bad[] = {'a', 0x01, 'b', '\0'};  // caractere non imprimable
  hdr.profile_id = bad;
  EXPECT_EQ_INT(bench_telemetry_emit_header(&tm, &hdr), 0, "ASCII invalide rejete");
  EXPECT_EQ_INT((long long)cap.len, 0, "toujours aucune trame");

  hdr.profile_id = "ok";
  EXPECT_EQ_INT(bench_telemetry_emit_header(&tm, &hdr), 1, "en-tete valide accepte");
}

// Point 2 : la cloture rend detectable une perte des DERNIERES trames.
static void test_cloture_du_flux(void) {
  cap_t cap;
  cap_init(&cap);
  bench_sample_t slots[4];
  bench_gap_record_t gaps[5];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 4, gaps, 5);
  bench_telemetry_sink_t sink = {&cap, cap_write};
  bench_telemetry_t tm;
  bench_telemetry_init(&tm, sink, &ring);

  const bench_telemetry_header_t hdr = {.clock_id = 1, .tick_width_bits = 64,
                                        .wrap_policy = 0, .tick_hz = 1000,
                                        .ring_capacity = 4, .profile_id = "p"};
  (void)bench_telemetry_emit_header(&tm, &hdr);
  bench_sample_t s = {1, 0, 10, BENCH_SAMPLE_OK, 0};
  (void)bench_ring_push(&ring, &s);
  (void)bench_telemetry_drain(&tm, 8);
  const bench_telemetry_summary_t sum = {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 100};
  (void)bench_telemetry_emit_summary(&tm, &sum);
  EXPECT_EQ_INT(bench_telemetry_emit_footer(&tm, 1), 1, "cloture emise");

  // Le footer porte son propre numero et le total de trames tentees.
  const uint32_t last_seq = msg_u32_at(&cap, 3, 1);
  const uint32_t attempted = msg_u32_at(&cap, 3, 5);
  EXPECT_EQ_INT(msg_type_at(&cap, 3), BENCH_TM_FOOTER, "dernier message = cloture");
  EXPECT_EQ_INT(last_seq, 3, "numero de sequence du footer");
  EXPECT_EQ_INT(attempted, 4, "4 trames tentees, footer inclus");
  EXPECT_EQ_INT(msg_u32_at(&cap, 3, 13), 0, "aucun refus");
}

static void test_refus_du_puits(void) {
  cap_t cap;
  cap_init(&cap);
  cap.refuse_from = 1;
  bench_sample_t slots[4];
  bench_gap_record_t gaps[5];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 4, gaps, 5);
  bench_telemetry_sink_t sink = {&cap, cap_write};
  bench_telemetry_t tm;
  bench_telemetry_init(&tm, sink, &ring);

  const bench_telemetry_header_t hdr = {.clock_id = 1, .tick_width_bits = 64,
                                        .wrap_policy = 0, .tick_hz = 1,
                                        .ring_capacity = 4, .profile_id = "p"};
  EXPECT_EQ_INT(bench_telemetry_emit_header(&tm, &hdr), 1, "en-tete accepte");
  bench_sample_t s = {1, 0, 10, BENCH_SAMPLE_OK, 0};
  (void)bench_ring_push(&ring, &s);
  (void)bench_telemetry_drain(&tm, 4);

  EXPECT(tm.frames_refused >= 1, "refus du puits compte");
  EXPECT(tm.stream_seq >= 2, "sequence avancee malgre le refus");
  EXPECT_EQ_INT(ring.producer_drop, 0, "un refus de puits n'est PAS un producer_drop");
  EXPECT_EQ_INT(tm.frames_accepted, 1, "une seule trame acceptee");
}

// Golden INTER-LANGAGE : reproduction octet a octet du flux de reference.
static void test_golden_inter_langage(void) {
#ifndef BENCH_GOLDEN_TELEMETRY_HEX
  EXPECT(0, "chemin du golden non defini par CMake");
#else
  FILE *f = fopen(BENCH_GOLDEN_TELEMETRY_HEX, "r");
  if (f == NULL) {
    EXPECT(0, "golden de telemetrie introuvable");
    return;
  }
  static uint8_t expected[8192];
  size_t exp_len = 0;
  int c;
  int hi = -1;
  while ((c = fgetc(f)) != EOF) {
    int v;
    if (c >= '0' && c <= '9') {
      v = c - '0';
    } else if (c >= 'a' && c <= 'f') {
      v = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
      v = c - 'A' + 10;
    } else {
      continue;
    }
    if (hi < 0) {
      hi = v;
    } else {
      if (exp_len < sizeof(expected)) {
        expected[exp_len++] = (uint8_t)((hi << 4) | v);
      }
      hi = -1;
    }
  }
  fclose(f);
  EXPECT(exp_len > 0, "golden non vide");

  cap_t cap;
  cap_init(&cap);
  bench_sample_t slots[4];
  bench_gap_record_t gaps[5];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 4, gaps, 5);
  bench_telemetry_sink_t sink = {&cap, cap_write};
  bench_telemetry_t tm;
  bench_telemetry_init(&tm, sink, &ring);

  const bench_ticks_t edges[5] = {0, 15, 25, 35, 45};
  uint32_t counts[4];
  bench_histogram_t h;
  EXPECT_EQ_INT(bench_histogram_init(&h, edges, 4, counts, 7), 1, "histogramme golden");

  const bench_telemetry_header_t hdr = {
      .clock_id = 1, .tick_width_bits = 64, .wrap_policy = BENCH_WRAP_POLICY_MODULAR,
      .tick_hz = 1000000, .ring_capacity = 4, .histogram_enabled = 1,
      .histogram_version = 7, .profile_id = "golden", .variant = "spi-shared",
      .mode = "module-only"};
  (void)bench_telemetry_emit_header(&tm, &hdr);

  const bench_ticks_t starts[6] = {100, 200, 300, 400, 500, 600};
  const bench_ticks_t ends[6] = {110, 220, 330, 440, 550, 700};
  const uint8_t sts[6] = {BENCH_SAMPLE_OK, BENCH_SAMPLE_OK, BENCH_SAMPLE_OK,
                          BENCH_SAMPLE_OK, BENCH_SAMPLE_OK, BENCH_SAMPLE_TIMEOUT};
  for (uint32_t i = 0; i < 6; i++) {
    bench_sample_t s = {i, starts[i], ends[i], sts[i], 0};
    if (bench_ring_push(&ring, &s) && sts[i] == BENCH_SAMPLE_OK) {
      bench_histogram_add(&h, bench_elapsed(starts[i], ends[i]));
    }
  }
  (void)bench_telemetry_drain(&tm, 64);
  bench_sample_t s6 = {6, 700, 705, BENCH_SAMPLE_TIMEOUT, 0};
  (void)bench_ring_push(&ring, &s6);
  bench_sample_t s7 = {7, 800, 860, BENCH_SAMPLE_REJECTED, 0};
  (void)bench_ring_push(&ring, &s7);
  (void)bench_telemetry_drain(&tm, 64);

  const bench_telemetry_summary_t sum = {8, 4, 1, 1, 0, 0, 0, 2, 0, 0, 500};
  (void)bench_telemetry_emit_summary(&tm, &sum);
  (void)bench_telemetry_emit_histogram(&tm, &h);
  (void)bench_telemetry_emit_footer(&tm, 8);

  EXPECT_EQ_INT((long long)cap.len, (long long)exp_len, "taille du flux golden");
  EXPECT(cap.len == exp_len && memcmp(cap.buf, expected, exp_len) == 0,
         "flux identique au golden, octet a octet");
#endif
}

// Point 4 : faute de place, les plages FUSIONNENT et le signalent ; le total
// reste exact mais la localisation ne l'est plus.
static void test_fusion_de_plages(void) {
  bench_sample_t slots[1];
  bench_gap_record_t gaps[1];  // une seule plage memorisable
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 1, gaps, 1);

  bench_sample_t s = {0, 0, 10, BENCH_SAMPLE_OK, 0};
  (void)bench_ring_push(&ring, &s);  // remplit
  s.sequence_id = 1;
  (void)bench_ring_push(&ring, &s);  // plage A
  bench_sample_t got;
  (void)bench_ring_pop(&ring, &got);
  s.sequence_id = 2;
  (void)bench_ring_push(&ring, &s);  // accepte
  s.sequence_id = 3;
  (void)bench_ring_push(&ring, &s);  // plage B : plus de place -> FUSION

  EXPECT_EQ_INT(ring.producer_drop, 2, "total de pertes exact");
  EXPECT_EQ_INT(ring.gap_count, 1, "une seule plage memorisee");
  EXPECT(ring.gap_records_merged >= 1, "fusion SIGNALEE");
}

// Aucune place declaree : les pertes sont comptees mais non localisables.
static void test_pertes_sans_capacite_de_localisation(void) {
  bench_sample_t slots[1];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 1, NULL, 0);

  bench_sample_t s = {0, 0, 10, BENCH_SAMPLE_OK, 0};
  (void)bench_ring_push(&ring, &s);
  s.sequence_id = 1;
  (void)bench_ring_push(&ring, &s);
  EXPECT_EQ_INT(ring.producer_drop, 1, "perte comptee");
  EXPECT_EQ_INT(ring.gap_count, 0, "aucune plage localisee");
  EXPECT_EQ_INT(ring.gap_capacity, 0, "capacite nulle exposee au bilan");
}

void run_telemetry_tests(void) {
  test_record_codec();
  test_ordre_de_la_lacune();
  test_ordre_dans_le_flux();
  test_drain_partiel_et_reprise();
  test_plages_multiples();
  test_fusion_de_plages();
  test_pertes_sans_capacite_de_localisation();
  test_refus_du_marqueur_puis_reemission();
  test_histogramme();
  test_histogramme_sature();
  test_entete_chaines_rejetees();
  test_cloture_du_flux();
  test_refus_du_puits();
  test_golden_inter_langage();
}

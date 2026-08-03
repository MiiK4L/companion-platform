// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Telemetrie : codec d'echantillon, tampon borne (non bloquant, pertes comptees
// et LOCALISEES par marqueur), histogramme optionnel (convention de classe,
// saturation), et flux (trames CRC, sequence monotone, refus du puits).
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

// --- Puits de test : capture les trames en memoire, refus programmable ------
typedef struct {
  uint8_t buf[8192];
  size_t len;
  uint32_t frames;
  int refuse_from;  // -1 : accepte tout ; sinon refuse a partir de cet index
} cap_t;

static int cap_write(void *ctx, const uint8_t *frame, size_t len) {
  cap_t *c = (cap_t *)ctx;
  if (c->refuse_from >= 0 && (int)c->frames >= c->refuse_from) {
    c->frames++;
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
  EXPECT_EQ_INT(out.status, BENCH_SAMPLE_OK, "statut restitue");
  EXPECT_EQ_INT(out.flags, BENCH_SAMPLE_FLAG_FAULT_CRC, "drapeaux restitues");
  EXPECT_EQ_INT(bench_sample_decode(buf, 3, &out), 0, "decodage tronque refuse");
}

static void test_ring_borne_et_pertes(void) {
  bench_sample_t slots[4];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 4);

  bench_sample_t s = {0, 0, 10, BENCH_SAMPLE_OK, 0};
  for (uint32_t i = 0; i < 4; i++) {
    s.sequence_id = i;
    EXPECT_EQ_INT(bench_ring_push(&ring, &s), 1, "stockage dans la capacite");
  }
  // Debordement : NON bloquant, perte comptee.
  s.sequence_id = 4;
  EXPECT_EQ_INT(bench_ring_push(&ring, &s), 0, "tampon plein : perte, pas d'attente");
  s.sequence_id = 5;
  EXPECT_EQ_INT(bench_ring_push(&ring, &s), 0, "seconde perte");
  EXPECT_EQ_INT(ring.producer_drop, 2, "producer_drop exact");

  // La lacune est LOCALISEE : elle suit le dernier sequence_id accepte (3).
  uint32_t after = 0;
  EXPECT_EQ_INT(bench_ring_take_gap(&ring, &after), 2, "lacune en attente = 2");
  EXPECT_EQ_INT(after, 3, "lacune situee apres la sequence 3");
  EXPECT_EQ_INT(bench_ring_take_gap(&ring, &after), 0, "lacune consommee une seule fois");
  EXPECT_EQ_INT(ring.producer_drop, 2, "total de pertes conserve");

  // Ordre FIFO preserve.
  bench_sample_t got;
  for (uint32_t i = 0; i < 4; i++) {
    EXPECT_EQ_INT(bench_ring_pop(&ring, &got), 1, "depilage");
    EXPECT_EQ_INT(got.sequence_id, i, "ordre FIFO");
  }
  EXPECT_EQ_INT(bench_ring_pop(&ring, &got), 0, "tampon vide");

  // Capacite nulle : tout est perdu, jamais de blocage ni d'ecriture.
  bench_ring_t zero;
  bench_ring_init(&zero, NULL, 0);
  EXPECT_EQ_INT(bench_ring_push(&zero, &s), 0, "capacite nulle : perte");
  EXPECT_EQ_INT(zero.producer_drop, 1, "perte comptee malgre capacite nulle");
}

static void test_histogramme(void) {
  const bench_ticks_t edges[4] = {10, 20, 30, 40};  // 3 classes
  uint32_t counts[3];
  bench_histogram_t h;
  EXPECT_EQ_INT(bench_histogram_init(&h, edges, 3, counts, 7), 1, "init valide");
  EXPECT_EQ_INT(h.version, 7, "version des bornes conservee");

  bench_histogram_add(&h, 9);   // underflow
  bench_histogram_add(&h, 10);  // classe 0 (borne basse INCLUSE)
  bench_histogram_add(&h, 19);  // classe 0
  bench_histogram_add(&h, 20);  // classe 1 (borne haute EXCLUE de la classe 0)
  bench_histogram_add(&h, 39);  // classe 2
  bench_histogram_add(&h, 40);  // overflow (borne haute exclue)
  bench_histogram_add(&h, 100); // overflow

  EXPECT_EQ_INT(h.underflow, 1, "underflow");
  EXPECT_EQ_INT(h.overflow, 2, "overflow");
  EXPECT_EQ_INT(h.sample_count, 7, "sample_count compte TOUT");
  EXPECT_EQ_INT(counts[0], 2, "classe [10,20)");
  EXPECT_EQ_INT(counts[1], 1, "classe [20,30)");
  EXPECT_EQ_INT(counts[2], 1, "classe [30,40)");
  EXPECT_EQ_INT(counts[0] + counts[1] + counts[2] + h.underflow + h.overflow,
                (int)h.sample_count, "identite de comptage");

  // Configurations invalides : refusees, jamais silencieusement acceptees.
  const bench_ticks_t bad[3] = {10, 10, 20};  // non strictement croissantes
  bench_histogram_t hb;
  EXPECT_EQ_INT(bench_histogram_init(&hb, bad, 2, counts, 1), 0,
                "bornes non croissantes refusees");
  EXPECT_EQ_INT(bench_histogram_init(&hb, edges, 0, counts, 1), 0,
                "zero classe refusee");
  EXPECT_EQ_INT(bench_histogram_init(&hb, edges, BENCH_HISTOGRAM_MAX_BINS + 1,
                                     counts, 1),
                0, "trop de classes refusees");
  // Un histogramme non initialise n'invente aucun chiffre.
  bench_histogram_add(&hb, 15);
  EXPECT_EQ_INT(hb.sample_count, 0, "histogramme desactive : aucun comptage");

  // Saturation d'une classe.
  bench_histogram_t hs;
  uint32_t c1[1];
  const bench_ticks_t e2[2] = {0, 100};
  EXPECT_EQ_INT(bench_histogram_init(&hs, e2, 1, c1, 1), 1, "init 1 classe");
  hs.counts[0] = UINT32_MAX;
  bench_histogram_add(&hs, 50);
  EXPECT_EQ_INT(hs.counts[0], UINT32_MAX, "comptage saturant (pas de wrap)");
}

static void test_flux_trames(void) {
  cap_t cap;
  cap_init(&cap);
  bench_sample_t slots[8];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 8);

  bench_telemetry_sink_t sink = {&cap, cap_write};
  bench_telemetry_t tm;
  bench_telemetry_init(&tm, sink, &ring);

  const bench_telemetry_header_t hdr = {
      .clock_id = 1, .tick_width_bits = 64, .wrap_policy = BENCH_WRAP_POLICY_MODULAR,
      .tick_hz = 1000000, .ring_capacity = 8, .histogram_enabled = 0,
      .histogram_version = 0, .profile_id = "l1-module", .variant = "spi-shared",
      .mode = "module-only"};
  EXPECT_EQ_INT(bench_telemetry_emit_header(&tm, &hdr), 1, "en-tete emis");

  bench_sample_t s = {0, 100, 150, BENCH_SAMPLE_OK, 0};
  for (uint32_t i = 0; i < 3; i++) {
    s.sequence_id = i;
    (void)bench_ring_push(&ring, &s);
  }
  EXPECT_EQ_INT(bench_telemetry_drain(&tm, 16), 3, "3 echantillons draines");

  const bench_telemetry_summary_t sum = {.issued = 3, .ok = 3, .timeout = 0,
                                         .rejected = 0, .unpaired = 0,
                                         .duplicate = 0, .out_of_order = 0,
                                         .producer_drop = 0,
                                         .timeout_budget_ticks = 500};
  EXPECT_EQ_INT(bench_telemetry_emit_summary(&tm, &sum), 1, "bilan emis");

  // Le flux est une suite de trames CRC valides, a sequence MONOTONE.
  size_t off = 0;
  uint32_t expected_seq = 0;
  uint32_t decoded = 0;
  while (off < cap.len) {
    bench_frame_t f;
    size_t consumed = 0;
    EXPECT_EQ_INT(bench_frame_decode_prefix(cap.buf + off, cap.len - off, &f,
                                            &consumed),
                  BENCH_FRAME_OK, "trame valide dans le flux");
    EXPECT_EQ_INT(f.seq, expected_seq, "sequence de flux monotone");
    expected_seq++;
    decoded++;
    off += consumed;
  }
  EXPECT_EQ_INT(decoded, 5, "en-tete + 3 echantillons + bilan");

  // Un octet corrompu rend la trame concernee detectable.
  cap.buf[0] ^= 0xFF;
  bench_frame_t f0;
  size_t c0 = 0;
  EXPECT(bench_frame_decode_prefix(cap.buf, cap.len, &f0, &c0) != BENCH_FRAME_OK,
         "corruption du flux detectee");
}

static void test_marqueur_de_lacune_dans_le_flux(void) {
  cap_t cap;
  cap_init(&cap);
  bench_sample_t slots[2];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 2);

  bench_telemetry_sink_t sink = {&cap, cap_write};
  bench_telemetry_t tm;
  bench_telemetry_init(&tm, sink, &ring);

  bench_sample_t s = {0, 0, 10, BENCH_SAMPLE_OK, 0};
  for (uint32_t i = 0; i < 5; i++) {  // 2 stockes, 3 perdus
    s.sequence_id = i;
    (void)bench_ring_push(&ring, &s);
  }
  EXPECT_EQ_INT(ring.producer_drop, 3, "3 pertes avant serialisation");

  const uint32_t emitted = bench_telemetry_drain(&tm, 16);
  EXPECT_EQ_INT(emitted, 3, "1 marqueur de lacune + 2 echantillons");

  // Le PREMIER message doit etre le marqueur : la lacune est situee.
  bench_frame_t f;
  size_t consumed = 0;
  EXPECT_EQ_INT(bench_frame_decode_prefix(cap.buf, cap.len, &f, &consumed),
                BENCH_FRAME_OK, "premiere trame valide");
  EXPECT_EQ_INT(f.payload[0], BENCH_TM_GAP, "premier message = marqueur de lacune");
  const uint32_t lost = ((uint32_t)f.payload[1] << 24) | ((uint32_t)f.payload[2] << 16) |
                        ((uint32_t)f.payload[3] << 8) | (uint32_t)f.payload[4];
  EXPECT_EQ_INT(lost, 3, "nombre de pertes dans le marqueur");
}

static void test_refus_du_puits(void) {
  cap_t cap;
  cap_init(&cap);
  cap.refuse_from = 1;  // accepte l'en-tete, refuse ensuite
  bench_sample_t slots[4];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 4);

  bench_telemetry_sink_t sink = {&cap, cap_write};
  bench_telemetry_t tm;
  bench_telemetry_init(&tm, sink, &ring);

  const bench_telemetry_header_t hdr = {.clock_id = 1, .tick_width_bits = 64,
                                        .wrap_policy = 0, .tick_hz = 1,
                                        .ring_capacity = 4};
  EXPECT_EQ_INT(bench_telemetry_emit_header(&tm, &hdr), 1, "en-tete accepte");

  bench_sample_t s = {1, 0, 10, BENCH_SAMPLE_OK, 0};
  (void)bench_ring_push(&ring, &s);
  (void)bench_telemetry_drain(&tm, 4);

  EXPECT(tm.frames_refused >= 1, "refus du puits compte");
  // La sequence AVANCE malgre le refus : la lacune de transport reste
  // detectable cote capture (transport_gap), distincte de producer_drop.
  EXPECT(tm.stream_seq >= 2, "sequence de flux avancee malgre le refus");
  EXPECT_EQ_INT(ring.producer_drop, 0, "un refus de puits n'est PAS un producer_drop");
}

// Reproduit EXACTEMENT le scenario du golden inter-langage et compare le flux
// produit, octet a octet, au fichier lu aussi par les tests Python. C'est
// l'ancrage qui empeche toute derive silencieuse entre les deux implementations
// du format.
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
      continue;  // separateurs / retours a la ligne
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

  // --- Rejoue le scenario de reference ---
  cap_t cap;
  cap_init(&cap);
  bench_sample_t slots[4];
  bench_ring_t ring;
  bench_ring_init(&ring, slots, 4);
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

  const uint32_t seqs[6] = {0, 1, 2, 3, 4, 5};
  const bench_ticks_t starts[6] = {100, 200, 300, 400, 500, 600};
  const bench_ticks_t ends[6] = {110, 220, 330, 440, 550, 700};
  const uint8_t sts[6] = {BENCH_SAMPLE_OK, BENCH_SAMPLE_OK, BENCH_SAMPLE_OK,
                          BENCH_SAMPLE_OK, BENCH_SAMPLE_OK, BENCH_SAMPLE_TIMEOUT};
  for (int i = 0; i < 6; i++) {
    bench_sample_t s = {seqs[i], starts[i], ends[i], sts[i], 0};
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

  const bench_telemetry_summary_t sum = {8, 4, 1, 1, 0, 0, 0, 2, 500};
  (void)bench_telemetry_emit_summary(&tm, &sum);
  (void)bench_telemetry_emit_histogram(&tm, &h);

  EXPECT_EQ_INT((long long)cap.len, (long long)exp_len, "taille du flux golden");
  EXPECT(cap.len == exp_len && memcmp(cap.buf, expected, exp_len) == 0,
         "flux identique au golden, octet a octet");
#endif
}

void run_telemetry_tests(void) {
  test_record_codec();
  test_ring_borne_et_pertes();
  test_histogramme();
  test_flux_trames();
  test_marqueur_de_lacune_dans_le_flux();
  test_refus_du_puits();
  test_golden_inter_langage();
}

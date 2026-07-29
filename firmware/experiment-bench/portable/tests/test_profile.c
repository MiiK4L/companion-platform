// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Profils : determinisme, vecteurs GOLDEN pins, motifs de payload DECLARATIFS,
// et planification des fautes.
#include <stdint.h>

#include "profiles/profile.h"
#include "tests/test_support.h"
#include "tests/tests.h"

void run_profile_tests(void) {
  // Determinisme du generateur.
  const bench_profile_t p = {.profile_id = "demo", .seed = 42};
  uint64_t s1 = 0;
  uint64_t s2 = 0;
  bench_profile_seed(&p, &s1);
  bench_profile_seed(&p, &s2);
  for (int i = 0; i < 5; i++) {
    EXPECT_EQ_INT(bench_profile_next(&s1), bench_profile_next(&s2),
                  "meme seed -> meme suite");
  }

  // Vecteurs GOLDEN seed = 42.
  const uint32_t golden42[4] = {0x0000000Au, 0xA00AAAFDu, 0x8B13399Cu, 0x283B88FEu};
  uint64_t g = 0;
  bench_profile_seed(&p, &g);
  for (int i = 0; i < 4; i++) {
    EXPECT_EQ_INT(bench_profile_next(&g), golden42[i], "vecteur golden seed=42");
  }

  // Vecteurs GOLDEN seed = 0xC0FFEE (seed du profil de demonstration).
  const bench_profile_t demo = {.profile_id = "demo", .seed = 0xC0FFEEull};
  const uint32_t goldenC0[4] = {0x00305E1Eu, 0xD0FB1E9Bu, 0xE15234F9u, 0xAC0D33B3u};
  uint64_t gc = 0;
  bench_profile_seed(&demo, &gc);
  for (int i = 0; i < 4; i++) {
    EXPECT_EQ_INT(bench_profile_next(&gc), goldenC0[i], "vecteur golden seed=0xC0FFEE");
  }

  // Motif de payload CONSTANT (declaratif).
  const bench_profile_t cst = {
      .payload_pattern = BENCH_PAYLOAD_CONSTANT, .payload_fill = 0xAB};
  uint8_t bc[4] = {0};
  uint64_t rc = 0;
  bench_profile_fill_payload(&cst, bc, 4, &rc);
  EXPECT(bc[0] == 0xAB && bc[3] == 0xAB, "payload constant");

  // Motif de payload INCREMENT (declaratif).
  const bench_profile_t inc = {
      .payload_pattern = BENCH_PAYLOAD_INCREMENT, .payload_fill = 0x10};
  uint8_t bi[4] = {0};
  uint64_t ri = 0;
  bench_profile_fill_payload(&inc, bi, 4, &ri);
  EXPECT(bi[0] == 0x10 && bi[1] == 0x11 && bi[2] == 0x12 && bi[3] == 0x13,
         "payload incremental");

  // Motif SEEDED : reproductible et derive du generateur.
  const bench_profile_t sd = {.payload_pattern = BENCH_PAYLOAD_SEEDED, .seed = 42};
  uint8_t bs[4] = {0};
  uint64_t rs = 0;
  bench_profile_seed(&sd, &rs);
  bench_profile_fill_payload(&sd, bs, 4, &rs);
  uint64_t chk = 0;
  bench_profile_seed(&sd, &chk);
  EXPECT(bs[0] == (uint8_t)(bench_profile_next(&chk) & 0xFF), "payload seede reproductible");

  // Planification des fautes.
  const bench_profile_t f = {.fault_crc_every = 3, .fault_timeout_every = 5};
  EXPECT(bench_profile_fault_crc(&f, 2), "faute CRC a l'index 2 (3e tx)");
  EXPECT(!bench_profile_fault_crc(&f, 1), "pas de faute CRC index 1");
  EXPECT(bench_profile_fault_timeout(&f, 4), "timeout a l'index 4 (5e tx)");
}

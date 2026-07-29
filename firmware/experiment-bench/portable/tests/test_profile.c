// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Profils : determinisme (meme seed -> meme suite), VECTEURS GOLDEN pins, et
// planification des fautes.
#include <stdint.h>

#include "profiles/profile.h"
#include "tests/test_support.h"
#include "tests/tests.h"

void run_profile_tests(void) {
  // Determinisme.
  const bench_profile_t p = {"demo", 1, 42, 4, 8, 2, 100, 0, 0};
  uint64_t s1 = 0;
  uint64_t s2 = 0;
  bench_profile_seed(&p, &s1);
  bench_profile_seed(&p, &s2);
  for (int i = 0; i < 5; i++) {
    EXPECT_EQ_INT(bench_profile_next(&s1), bench_profile_next(&s2),
                  "meme seed -> meme suite");
  }

  // Vecteurs GOLDEN pour seed = 42 (constantes calculees hors-ligne).
  const uint32_t golden42[4] = {0x0000000Au, 0xA00AAAFDu, 0x8B13399Cu, 0x283B88FEu};
  uint64_t g = 0;
  bench_profile_seed(&p, &g);
  for (int i = 0; i < 4; i++) {
    EXPECT_EQ_INT(bench_profile_next(&g), golden42[i], "vecteur golden seed=42");
  }

  // Vecteurs GOLDEN pour seed = 0xC0FFEE (seed du profil de demonstration).
  const bench_profile_t demo = {"demo", 1, 0xC0FFEEull, 4, 8, 2, 100, 0, 0};
  const uint32_t goldenC0[4] = {0x00305E1Eu, 0xD0FB1E9Bu, 0xE15234F9u, 0xAC0D33B3u};
  uint64_t gc = 0;
  bench_profile_seed(&demo, &gc);
  for (int i = 0; i < 4; i++) {
    EXPECT_EQ_INT(bench_profile_next(&gc), goldenC0[i], "vecteur golden seed=0xC0FFEE");
  }

  // Planification des fautes.
  const bench_profile_t f = {"f", 1, 1, 10, 8, 0, 100, 3, 5};
  EXPECT(bench_profile_fault_crc(&f, 2), "faute CRC a l'index 2 (3e tx)");
  EXPECT(!bench_profile_fault_crc(&f, 1), "pas de faute CRC index 1");
  EXPECT(bench_profile_fault_timeout(&f, 4), "timeout a l'index 4 (5e tx)");
}

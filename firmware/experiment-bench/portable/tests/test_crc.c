// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Vecteurs CRC-32/IEEE CONNUS (golden), en plus des proprietes stable/sensible.
#include <string.h>

#include "crc/crc.h"
#include "tests/test_support.h"
#include "tests/tests.h"

void run_crc_tests(void) {
  // Vecteurs de reference standard de CRC-32/IEEE 802.3.
  EXPECT_EQ_INT(bench_crc32((const uint8_t *)"", 0), 0x00000000u,
                "CRC-32 de la chaine vide");
  EXPECT_EQ_INT(bench_crc32((const uint8_t *)"123456789", 9), 0xCBF43926u,
                "CRC-32 golden de \"123456789\"");
  EXPECT_EQ_INT(
      bench_crc32((const uint8_t *)"The quick brown fox jumps over the lazy dog",
                  43),
      0x414FA339u, "CRC-32 golden de la phrase de reference");

  const uint8_t a[] = {1, 2, 3, 4};
  const uint8_t b[] = {1, 2, 3, 5};
  EXPECT_EQ_INT(bench_crc32(a, 4), bench_crc32(a, 4), "CRC stable");
  EXPECT(bench_crc32(a, 4) != bench_crc32(b, 4), "CRC sensible au contenu");
}

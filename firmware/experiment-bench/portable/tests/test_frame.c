// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Trame : encode->decode, tronquee, longueur incoherente, version inconnue,
// mauvais magic, CRC reellement corrompu, prefixe avec bourrage.
#include <string.h>

#include "frame/frame.h"
#include "tests/test_support.h"
#include "tests/tests.h"

void run_frame_tests(void) {
  const uint8_t payload[] = {0xDE, 0xAD, 0xBE, 0xEF};
  uint8_t buf[BENCH_FRAME_MAX_SIZE];

  // encode -> decode (aller-retour).
  const int n = bench_frame_encode(buf, sizeof(buf), 0x11223344u, payload, 4);
  EXPECT(n == (int)(BENCH_FRAME_OVERHEAD + 4), "longueur de trame encodee");
  bench_frame_t f;
  EXPECT_EQ_INT(bench_frame_decode(buf, (size_t)n, &f), BENCH_FRAME_OK,
                "decode d'une trame valide");
  EXPECT_EQ_INT(f.seq, 0x11223344u, "seq restitue");
  EXPECT_EQ_INT(f.payload_len, 4, "payload_len restitue");
  EXPECT(memcmp(f.payload, payload, 4) == 0, "payload restitue");

  // Tronquee (moins que l'overhead).
  EXPECT_EQ_INT(bench_frame_decode(buf, 5, &f), BENCH_FRAME_ERR_TRUNCATED,
                "trame tronquee rejetee");

  // Longueur incoherente (un octet en trop).
  EXPECT_EQ_INT(bench_frame_decode(buf, (size_t)n + 1, &f),
                BENCH_FRAME_ERR_BAD_LENGTH, "longueur incoherente rejetee");

  // Mauvais magic.
  uint8_t bad = buf[0];
  buf[0] ^= 0xFF;
  EXPECT_EQ_INT(bench_frame_decode(buf, (size_t)n, &f), BENCH_FRAME_ERR_BAD_MAGIC,
                "magic invalide rejete");
  buf[0] = bad;

  // Version inconnue.
  bad = buf[2];
  buf[2] = 0xEE;
  EXPECT_EQ_INT(bench_frame_decode(buf, (size_t)n, &f),
                BENCH_FRAME_ERR_BAD_VERSION, "version inconnue rejetee");
  buf[2] = bad;

  // CRC reellement corrompu.
  EXPECT_EQ_INT(bench_frame_corrupt_crc(buf, (size_t)n), 0, "corruption CRC appliquee");
  EXPECT_EQ_INT(bench_frame_decode(buf, (size_t)n, &f), BENCH_FRAME_ERR_BAD_CRC,
                "CRC corrompu rejete");

  // Payload reellement corrompu -> CRC ne correspond plus.
  const int m = bench_frame_encode(buf, sizeof(buf), 7, payload, 4);
  EXPECT(m > 0, "re-encode ok");
  EXPECT_EQ_INT(bench_frame_corrupt_payload(buf, (size_t)m, 1), 0,
                "corruption payload appliquee");
  EXPECT_EQ_INT(bench_frame_decode(buf, (size_t)m, &f), BENCH_FRAME_ERR_BAD_CRC,
                "payload corrompu detecte par le CRC");

  // Decode par prefixe : trame valide suivie de bourrage.
  const int p = bench_frame_encode(buf, sizeof(buf), 9, payload, 4);
  size_t consumed = 0;
  EXPECT_EQ_INT(bench_frame_decode_prefix(buf, (size_t)p + 10, &f, &consumed),
                BENCH_FRAME_OK, "prefixe : trame + bourrage acceptee");
  EXPECT_EQ_INT(consumed, p, "prefixe : taille consommee = taille de la trame");

  // payload_len > MAX rejete (fabrique un entete annoncant 0xFFFF).
  uint8_t big[BENCH_FRAME_OVERHEAD];
  memset(big, 0, sizeof(big));
  big[0] = BENCH_FRAME_MAGIC0;
  big[1] = BENCH_FRAME_MAGIC1;
  big[2] = (uint8_t)BENCH_FRAME_VERSION;
  big[7] = 0xFF;  // payload_len = 0xFFFF
  big[8] = 0xFF;
  EXPECT_EQ_INT(bench_frame_decode_prefix(big, sizeof(big), &f, &consumed),
                BENCH_FRAME_ERR_BAD_LENGTH, "payload_len > MAX rejete");
}

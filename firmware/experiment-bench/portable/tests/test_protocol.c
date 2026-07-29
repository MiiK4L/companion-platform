// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Protocole binaire : encode/decode, commande inconnue, argument trop long
// REJETE (jamais tronque), longueur incoherente, aller-retour des compteurs.
#include <string.h>

#include "protocol/protocol.h"
#include "tests/test_support.h"
#include "tests/tests.h"

void run_protocol_tests(void) {
  uint8_t buf[128];

  // START (sans argument) : aller-retour.
  bench_cmd_t start = {BENCH_CMD_START, 0, {0}};
  int n = bench_protocol_encode_cmd(buf, sizeof(buf), &start);
  EXPECT_EQ_INT(n, 2, "START encode sur 2 octets");
  bench_cmd_t out;
  EXPECT_EQ_INT(bench_protocol_decode_cmd(buf, (size_t)n, &out), BENCH_PROTO_OK,
                "START decode");
  EXPECT_EQ_INT(out.type, BENCH_CMD_START, "type START");

  // GET_CAPABILITIES / RESET_COUNTERS reconnus.
  bench_cmd_t caps = {BENCH_CMD_GET_CAPABILITIES, 0, {0}};
  n = bench_protocol_encode_cmd(buf, sizeof(buf), &caps);
  EXPECT_EQ_INT(bench_protocol_decode_cmd(buf, (size_t)n, &out), BENCH_PROTO_OK,
                "GET_CAPABILITIES decode");
  EXPECT_EQ_INT(out.type, BENCH_CMD_GET_CAPABILITIES, "type GET_CAPABILITIES");

  // SELECT_PROFILE avec argument.
  bench_cmd_t sel = {BENCH_CMD_SELECT_PROFILE, 4, {0}};
  memcpy(sel.profile_id, "demo", 4);
  n = bench_protocol_encode_cmd(buf, sizeof(buf), &sel);
  EXPECT_EQ_INT(n, 6, "SELECT_PROFILE encode 2+4");
  EXPECT_EQ_INT(bench_protocol_decode_cmd(buf, (size_t)n, &out), BENCH_PROTO_OK,
                "SELECT_PROFILE decode");
  EXPECT_EQ_INT(out.profile_id_len, 4, "longueur d'argument");
  EXPECT(memcmp(out.profile_id, "demo", 4) == 0, "argument restitue");

  // Commande inconnue.
  const uint8_t unknown[] = {0x7F, 0x00};
  EXPECT_EQ_INT(bench_protocol_decode_cmd(unknown, 2, &out),
                BENCH_PROTO_UNKNOWN_CMD, "commande inconnue signalee");
  EXPECT_EQ_INT(out.type, BENCH_CMD_UNKNOWN, "type inconnu");

  // Message tronque.
  const uint8_t trunc[] = {BENCH_CMD_START};
  EXPECT_EQ_INT(bench_protocol_decode_cmd(trunc, 1, &out), BENCH_PROTO_TRUNCATED,
                "message tronque rejete");

  // Longueur incoherente (arg_len ne colle pas avec len).
  const uint8_t badlen[] = {BENCH_CMD_START, 0x03, 0x01};
  EXPECT_EQ_INT(bench_protocol_decode_cmd(badlen, 3, &out), BENCH_PROTO_BAD_LENGTH,
                "longueur incoherente rejetee");

  // START avec argument interdit.
  const uint8_t startarg[] = {BENCH_CMD_START, 0x01, 0xAA};
  EXPECT_EQ_INT(bench_protocol_decode_cmd(startarg, 3, &out),
                BENCH_PROTO_BAD_LENGTH, "argument sur commande sans arg rejete");

  // Argument SELECT trop long : REJET a l'encodage (pas de troncature).
  bench_cmd_t toolong;
  memset(&toolong, 0, sizeof(toolong));
  toolong.type = BENCH_CMD_SELECT_PROFILE;
  toolong.profile_id_len = (uint8_t)(BENCH_PROFILE_ID_MAX + 1);
  EXPECT_EQ_INT(bench_protocol_encode_cmd(buf, sizeof(buf), &toolong), -1,
                "argument trop long rejete a l'encodage");

  // Argument SELECT trop long : REJET au decodage (fabrique a la main).
  uint8_t longmsg[2 + BENCH_PROFILE_ID_MAX + 1];
  memset(longmsg, 'x', sizeof(longmsg));
  longmsg[0] = BENCH_CMD_SELECT_PROFILE;
  longmsg[1] = (uint8_t)(BENCH_PROFILE_ID_MAX + 1);
  EXPECT_EQ_INT(bench_protocol_decode_cmd(longmsg, sizeof(longmsg), &out),
                BENCH_PROTO_BAD_ARG, "argument trop long rejete au decodage");

  // SELECT_PROFILE sans argument -> BAD_ARG.
  const uint8_t noarg[] = {BENCH_CMD_SELECT_PROFILE, 0x00};
  EXPECT_EQ_INT(bench_protocol_decode_cmd(noarg, 2, &out), BENCH_PROTO_BAD_ARG,
                "SELECT sans argument rejete");

  // Aller-retour des compteurs.
  bench_counters_t c;
  bench_counters_reset(&c);
  c.tx_ok = 7;
  c.crc_errors = 2;
  c.bytes_transferred = 0x1122334455667788ull;
  uint8_t cbuf[BENCH_COUNTERS_WIRE_SIZE];
  EXPECT_EQ_INT(bench_protocol_encode_counters(cbuf, sizeof(cbuf), &c),
                (int)BENCH_COUNTERS_WIRE_SIZE, "compteurs encodes (taille fixe)");
  bench_counters_t d;
  EXPECT_EQ_INT(bench_protocol_decode_counters(cbuf, sizeof(cbuf), &d),
                BENCH_PROTO_OK, "compteurs decodes");
  EXPECT_EQ_INT(d.tx_ok, 7, "compteur tx_ok restitue");
  EXPECT_EQ_INT(d.crc_errors, 2, "compteur crc_errors restitue");
  EXPECT(d.bytes_transferred == 0x1122334455667788ull, "compteur 64 bits restitue");
}

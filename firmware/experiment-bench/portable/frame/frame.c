// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "frame/frame.h"

#include <string.h>

#include "crc/crc.h"
#include "util/wire.h"

int bench_frame_encode(uint8_t *out, size_t cap, uint32_t seq,
                       const uint8_t *payload, size_t payload_len) {
  if (payload_len > BENCH_FRAME_MAX_PAYLOAD) {
    return -1;
  }
  const size_t total = BENCH_FRAME_OVERHEAD + payload_len;
  if (cap < total) {
    return -1;
  }
  out[0] = BENCH_FRAME_MAGIC0;
  out[1] = BENCH_FRAME_MAGIC1;
  out[2] = (uint8_t)BENCH_FRAME_VERSION;
  bench_wire_put_u32(out + 3, seq);
  bench_wire_put_u16(out + 7, (uint16_t)payload_len);
  if (payload_len > 0) {
    memcpy(out + BENCH_FRAME_HEADER_SIZE, payload, payload_len);
  }
  // CRC sur l'entete + le payload (tout ce qui precede le champ CRC).
  const uint32_t crc = bench_crc32(out, BENCH_FRAME_HEADER_SIZE + payload_len);
  bench_wire_put_u32(out + BENCH_FRAME_HEADER_SIZE + payload_len, crc);
  return (int)total;
}

bench_frame_result_t bench_frame_decode_prefix(const uint8_t *in, size_t avail,
                                               bench_frame_t *out,
                                               size_t *consumed) {
  if (avail < BENCH_FRAME_OVERHEAD) {
    return BENCH_FRAME_ERR_TRUNCATED;
  }
  if (in[0] != BENCH_FRAME_MAGIC0 || in[1] != BENCH_FRAME_MAGIC1) {
    return BENCH_FRAME_ERR_BAD_MAGIC;
  }
  if (in[2] != BENCH_FRAME_VERSION) {
    return BENCH_FRAME_ERR_BAD_VERSION;
  }
  const uint16_t payload_len = bench_wire_get_u16(in + 7);
  if (payload_len > BENCH_FRAME_MAX_PAYLOAD) {
    return BENCH_FRAME_ERR_BAD_LENGTH;
  }
  const size_t frame_len = (size_t)BENCH_FRAME_OVERHEAD + payload_len;
  if (avail < frame_len) {
    return BENCH_FRAME_ERR_TRUNCATED;  // trame annoncee non entierement presente
  }
  const uint32_t computed = bench_crc32(in, BENCH_FRAME_HEADER_SIZE + payload_len);
  const uint32_t received = bench_wire_get_u32(in + BENCH_FRAME_HEADER_SIZE + payload_len);
  if (computed != received) {
    return BENCH_FRAME_ERR_BAD_CRC;
  }
  out->version = in[2];
  out->seq = bench_wire_get_u32(in + 3);
  out->payload_len = payload_len;
  if (payload_len > 0) {
    memcpy(out->payload, in + BENCH_FRAME_HEADER_SIZE, payload_len);
  }
  if (consumed != NULL) {
    *consumed = frame_len;
  }
  return BENCH_FRAME_OK;
}

bench_frame_result_t bench_frame_decode(const uint8_t *in, size_t len,
                                        bench_frame_t *out) {
  size_t consumed = 0;
  const bench_frame_result_t r = bench_frame_decode_prefix(in, len, out, &consumed);
  if (r != BENCH_FRAME_OK) {
    return r;
  }
  // Exactitude : aucun octet en trop apres la trame.
  if (consumed != len) {
    return BENCH_FRAME_ERR_BAD_LENGTH;
  }
  return BENCH_FRAME_OK;
}

int bench_frame_corrupt_payload(uint8_t *frame, size_t len, size_t payload_index) {
  if (len < BENCH_FRAME_OVERHEAD) {
    return -1;
  }
  const uint16_t payload_len = bench_wire_get_u16(frame + 7);
  if (payload_index >= payload_len) {
    return -1;
  }
  frame[BENCH_FRAME_HEADER_SIZE + payload_index] ^= 0xFF;  // corruption reelle
  return 0;
}

int bench_frame_corrupt_crc(uint8_t *frame, size_t len) {
  if (len < BENCH_FRAME_OVERHEAD) {
    return -1;
  }
  frame[len - 1] ^= 0xFF;  // altere le dernier octet du CRC
  return 0;
}

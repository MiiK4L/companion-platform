// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "protocol/protocol.h"

#include <string.h>

#include "util/wire.h"

static int cmd_type_known(uint8_t t) {
  switch (t) {
    case BENCH_CMD_SELECT_PROFILE:
    case BENCH_CMD_START:
    case BENCH_CMD_STOP:
    case BENCH_CMD_READ_COUNTERS:
    case BENCH_CMD_RESET_COUNTERS:
    case BENCH_CMD_GET_CAPABILITIES:
      return 1;
    default:
      return 0;
  }
}

int bench_protocol_encode_cmd(uint8_t *out, size_t cap, const bench_cmd_t *cmd) {
  uint8_t arg_len = 0;
  if (cmd->type == BENCH_CMD_SELECT_PROFILE) {
    if (cmd->profile_id_len == 0 || cmd->profile_id_len > BENCH_PROFILE_ID_MAX) {
      return -1;  // rejet (argument absent ou trop long), jamais de troncature
    }
    arg_len = cmd->profile_id_len;
  }
  const size_t total = (size_t)2 + arg_len;
  if (cap < total) {
    return -1;
  }
  out[0] = (uint8_t)cmd->type;
  out[1] = arg_len;
  if (arg_len > 0) {
    memcpy(out + 2, cmd->profile_id, arg_len);
  }
  return (int)total;
}

bench_proto_status_t bench_protocol_decode_cmd(const uint8_t *in, size_t len,
                                               bench_cmd_t *out) {
  memset(out, 0, sizeof(*out));
  out->type = BENCH_CMD_UNKNOWN;
  if (len < 2) {
    return BENCH_PROTO_TRUNCATED;
  }
  const uint8_t type = in[0];
  const uint8_t arg_len = in[1];
  if ((size_t)2 + arg_len != len) {
    return BENCH_PROTO_BAD_LENGTH;
  }
  if (!cmd_type_known(type)) {
    return BENCH_PROTO_UNKNOWN_CMD;
  }
  if (type == BENCH_CMD_SELECT_PROFILE) {
    if (arg_len == 0) {
      return BENCH_PROTO_BAD_ARG;
    }
    if (arg_len > BENCH_PROFILE_ID_MAX) {
      return BENCH_PROTO_BAD_ARG;  // trop long => REJET
    }
    out->profile_id_len = arg_len;
    memcpy(out->profile_id, in + 2, arg_len);
  } else if (arg_len != 0) {
    return BENCH_PROTO_BAD_LENGTH;  // ces commandes n'ont pas d'argument
  }
  out->type = (bench_cmd_type_t)type;
  return BENCH_PROTO_OK;
}

int bench_protocol_encode_counters(uint8_t *out, size_t cap,
                                   const bench_counters_t *c) {
  if (cap < BENCH_COUNTERS_WIRE_SIZE) {
    return -1;
  }
  uint8_t *p = out;
  bench_wire_put_u32(p, c->tx_ok); p += 4;
  bench_wire_put_u32(p, c->tx_failed); p += 4;
  bench_wire_put_u32(p, c->crc_errors); p += 4;
  bench_wire_put_u32(p, c->timeouts); p += 4;
  bench_wire_put_u32(p, c->irq_count); p += 4;
  bench_wire_put_u32(p, c->resets); p += 4;
  bench_wire_put_u32(p, c->latency_samples); p += 4;
  bench_wire_put_u64(p, c->bytes_transferred); p += 8;
  bench_wire_put_u64(p, c->latency_sum_ticks); p += 8;
  bench_wire_put_u64(p, c->latency_max_ticks); p += 8;
  return (int)BENCH_COUNTERS_WIRE_SIZE;
}

bench_proto_status_t bench_protocol_decode_counters(const uint8_t *in, size_t len,
                                                    bench_counters_t *out) {
  if (len != BENCH_COUNTERS_WIRE_SIZE) {
    return BENCH_PROTO_BAD_LENGTH;
  }
  const uint8_t *p = in;
  out->tx_ok = bench_wire_get_u32(p); p += 4;
  out->tx_failed = bench_wire_get_u32(p); p += 4;
  out->crc_errors = bench_wire_get_u32(p); p += 4;
  out->timeouts = bench_wire_get_u32(p); p += 4;
  out->irq_count = bench_wire_get_u32(p); p += 4;
  out->resets = bench_wire_get_u32(p); p += 4;
  out->latency_samples = bench_wire_get_u32(p); p += 4;
  out->bytes_transferred = bench_wire_get_u64(p); p += 8;
  out->latency_sum_ticks = bench_wire_get_u64(p); p += 8;
  out->latency_max_ticks = bench_wire_get_u64(p); p += 8;
  return BENCH_PROTO_OK;
}

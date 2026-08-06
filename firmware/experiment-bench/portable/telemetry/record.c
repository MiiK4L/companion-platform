// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "telemetry/record.h"

#include "util/wire.h"

size_t bench_sample_encode(uint8_t *out, size_t cap, const bench_sample_t *s) {
  if (cap < BENCH_SAMPLE_WIRE_SIZE) {
    return 0;
  }
  size_t o = 0;
  out[o++] = s->producer_id;
  bench_wire_put_u32(out + o, s->producer_sequence_id); o += 4;
  bench_wire_put_u32(out + o, s->global_event_seq); o += 4;
  bench_wire_put_u64(out + o, s->t_request); o += 8;
  bench_wire_put_u64(out + o, s->t_grant); o += 8;
  bench_wire_put_u64(out + o, s->t_release); o += 8;
  bench_wire_put_u64(out + o, s->t_end); o += 8;
  out[o++] = s->status;
  out[o++] = s->timeout_cause;
  out[o++] = s->flags;
  return o;
}

size_t bench_sample_decode(const uint8_t *in, size_t len, bench_sample_t *out) {
  if (len < BENCH_SAMPLE_WIRE_SIZE) {
    return 0;
  }
  size_t o = 0;
  out->producer_id = in[o++];
  out->producer_sequence_id = bench_wire_get_u32(in + o); o += 4;
  out->global_event_seq = bench_wire_get_u32(in + o); o += 4;
  out->t_request = bench_wire_get_u64(in + o); o += 8;
  out->t_grant = bench_wire_get_u64(in + o); o += 8;
  out->t_release = bench_wire_get_u64(in + o); o += 8;
  out->t_end = bench_wire_get_u64(in + o); o += 8;
  out->status = in[o++];
  out->timeout_cause = in[o++];
  out->flags = in[o++];
  return o;
}

// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "telemetry/record.h"

#include "util/wire.h"

size_t bench_sample_encode(uint8_t *out, size_t cap, const bench_sample_t *s) {
  if (cap < BENCH_SAMPLE_WIRE_SIZE) {
    return 0;
  }
  bench_wire_put_u32(out, s->sequence_id);
  bench_wire_put_u64(out + 4, s->t_start);
  bench_wire_put_u64(out + 12, s->t_end);
  out[20] = s->status;
  out[21] = s->flags;
  return BENCH_SAMPLE_WIRE_SIZE;
}

size_t bench_sample_decode(const uint8_t *in, size_t len, bench_sample_t *out) {
  if (len < BENCH_SAMPLE_WIRE_SIZE) {
    return 0;
  }
  out->sequence_id = bench_wire_get_u32(in);
  out->t_start = bench_wire_get_u64(in + 4);
  out->t_end = bench_wire_get_u64(in + 12);
  out->status = in[20];
  out->flags = in[21];
  return BENCH_SAMPLE_WIRE_SIZE;
}

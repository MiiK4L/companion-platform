// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "protocol/protocol.h"

#include <stdio.h>
#include <string.h>

static int starts_with(const char *s, const char *prefix) {
  return strncmp(s, prefix, strlen(prefix)) == 0;
}

bench_cmd_t bench_protocol_parse(const char *line) {
  bench_cmd_t cmd = {BENCH_CMD_UNKNOWN, {0}};
  if (line == NULL) {
    return cmd;
  }
  if (starts_with(line, "SELECT ")) {
    cmd.type = BENCH_CMD_SELECT_PROFILE;
    const char *arg = line + strlen("SELECT ");
    // Copie bornee (jamais de debordement) ; troncature explicite.
    size_t i = 0;
    for (; arg[i] != '\0' && i < sizeof(cmd.arg) - 1; i++) {
      cmd.arg[i] = arg[i];
    }
    cmd.arg[i] = '\0';
  } else if (strcmp(line, "START") == 0) {
    cmd.type = BENCH_CMD_START;
  } else if (strcmp(line, "STOP") == 0) {
    cmd.type = BENCH_CMD_STOP;
  } else if (strcmp(line, "COUNTERS") == 0) {
    cmd.type = BENCH_CMD_READ_COUNTERS;
  }
  return cmd;
}

int bench_protocol_format_counters(char *out, size_t cap,
                                   const bench_counters_t *c) {
  const int written = snprintf(
      out, cap,
      "COUNTERS tx_ok=%u tx_failed=%u crc_errors=%u timeouts=%u irq=%u "
      "resets=%u bytes=%llu lat_sum=%llu lat_max=%llu lat_n=%u",
      c->tx_ok, c->tx_failed, c->crc_errors, c->timeouts, c->irq_count,
      c->resets, (unsigned long long)c->bytes_transferred,
      (unsigned long long)c->latency_sum_ticks,
      (unsigned long long)c->latency_max_ticks, c->latency_samples);
  if (written < 0 || (size_t)written >= cap) {
    return -1;
  }
  return written;
}

// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "adapters/host/mock_log.h"

static void ml_log(void *self, log_level_t level, const char *msg) {
  (void)msg;
  MockLog *ml = (MockLog *)self;
  if (level >= LOG_DEBUG && level <= LOG_ERROR) ml->count[level]++;
}

void mock_log_init(MockLog *ml) {
  for (int i = 0; i < 4; i++) ml->count[i] = 0;
}

ILog mock_log_port(MockLog *ml) {
  ILog p;
  p.self = ml;
  p.log = ml_log;
  return p;
}

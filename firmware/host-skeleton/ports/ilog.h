// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port ILog — SIGNATURE CANDIDATE (non figée). Journalisation abstraite ;
// aucune dépendance à un backend concret (UART, ESP-IDF, stdio…).
#ifndef COMPANION_PORTS_ILOG_H
#define COMPANION_PORTS_ILOG_H

typedef enum {
  LOG_DEBUG = 0,
  LOG_INFO = 1,
  LOG_WARN = 2,
  LOG_ERROR = 3,
} log_level_t;

typedef struct ILog {
  void *self;
  void (*log)(void *self, log_level_t level, const char *msg);
} ILog;

#endif  // COMPANION_PORTS_ILOG_H

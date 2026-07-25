// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// MockLog — adaptateur host de test : compte les messages par niveau (pour
// assertions). Aucun backend réel (UART/stdio/ESP-IDF).
#ifndef COMPANION_ADAPTERS_HOST_MOCK_LOG_H
#define COMPANION_ADAPTERS_HOST_MOCK_LOG_H

#include "ports/ilog.h"

typedef struct MockLog {
  int count[4];  // indexé par log_level_t
} MockLog;

void mock_log_init(MockLog *ml);

ILog mock_log_port(MockLog *ml);

#endif  // COMPANION_ADAPTERS_HOST_MOCK_LOG_H

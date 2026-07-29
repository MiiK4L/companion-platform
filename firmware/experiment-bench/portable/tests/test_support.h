// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Harnais de test minimal (sans dependance externe), style host-skeleton.
#ifndef BENCH_TEST_SUPPORT_H
#define BENCH_TEST_SUPPORT_H

#include <stdio.h>

extern int g_test_failures;

#define EXPECT(cond, msg)                                        \
  do {                                                           \
    if (!(cond)) {                                               \
      printf("FAIL: %s (%s:%d)\n", (msg), __FILE__, __LINE__);   \
      g_test_failures++;                                         \
    }                                                            \
  } while (0)

#define EXPECT_EQ_INT(a, b, msg)                                              \
  do {                                                                        \
    long long _a = (long long)(a);                                            \
    long long _b = (long long)(b);                                            \
    if (_a != _b) {                                                           \
      printf("FAIL: %s : %lld != %lld (%s:%d)\n", (msg), _a, _b, __FILE__,    \
             __LINE__);                                                       \
      g_test_failures++;                                                      \
    }                                                                         \
  } while (0)

#endif  // BENCH_TEST_SUPPORT_H

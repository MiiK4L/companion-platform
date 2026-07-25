// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Harnais de test MINIMAL et sans dépendance. Le choix d'un framework de test
// (Unity/CMock, GoogleTest…) est une DÉCISION NON FIGÉE (voir candidate
// components) ; ce harnais évite de la préempter tout en exécutant de vrais
// tests host en CI.
#ifndef COMPANION_TESTS_TEST_SUPPORT_H
#define COMPANION_TESTS_TEST_SUPPORT_H

#include <stdio.h>

extern int g_test_failures;

#define EXPECT(cond, msg)                                             \
  do {                                                               \
    if (!(cond)) {                                                   \
      g_test_failures++;                                             \
      fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, msg);  \
    }                                                                \
  } while (0)

#define EXPECT_EQ_INT(a, b, msg) EXPECT((long)(a) == (long)(b), msg)

#endif  // COMPANION_TESTS_TEST_SUPPORT_H

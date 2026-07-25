// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Runner de tests host minimal. Renvoie 0 si tous les tests passent, 1 sinon
// (exploité par la CI). Framework de test délibérément NON figé.
#include <stdio.h>

int g_test_failures = 0;

void test_launch_success(void);
void test_storage_not_found(void);
void test_runtime_load_fail(void);

int main(void) {
  test_launch_success();
  test_storage_not_found();
  test_runtime_load_fail();

  if (g_test_failures == 0) {
    printf("host-skeleton tests: OK\n");
    return 0;
  }
  fprintf(stderr, "host-skeleton tests: %d failure(s)\n", g_test_failures);
  return 1;
}

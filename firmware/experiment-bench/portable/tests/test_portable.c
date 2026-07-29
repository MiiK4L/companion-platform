// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Point d'entree des tests du coeur portable : agrege les suites par module
// (CRC, trame, protocole, ordonnanceur, transport, compteurs, profils, moteur).
#include <stdio.h>

#include "tests/tests.h"

int g_test_failures = 0;

int main(void) {
  run_crc_tests();
  run_frame_tests();
  run_protocol_tests();
  run_scheduler_tests();
  run_transport_tests();
  run_counters_tests();
  run_profile_tests();
  run_engine_tests();
  run_sim_tests();

  if (g_test_failures == 0) {
    printf("OK: coeur portable (toutes les suites)\n");
    return 0;
  }
  printf("ECHEC: %d test(s)\n", g_test_failures);
  return 1;
}

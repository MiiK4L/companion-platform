// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Prototypes des suites de tests du coeur portable (un fichier par module).
#ifndef BENCH_TESTS_H
#define BENCH_TESTS_H

void run_crc_tests(void);
void run_frame_tests(void);
void run_protocol_tests(void);
void run_scheduler_tests(void);
void run_transport_tests(void);
void run_counters_tests(void);
void run_profile_tests(void);
void run_engine_tests(void);
void run_sim_tests(void);
void run_telemetry_tests(void);

#endif  // BENCH_TESTS_H

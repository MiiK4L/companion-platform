# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0

# Coeur PORTABLE du firmware de banc : compilable et testable NATIVEMENT (aucune
# toolchain embarquee). Aucune dependance ESP-IDF / Pico SDK / FreeRTOS / registre.
cmake_minimum_required(VERSION 3.16)
project(bench_portable C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Werror)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(bench_portable STATIC
  crc/crc.c
  scheduler/scheduler.c
  profiles/profile.c
  counters/counters.c
  protocol/protocol.c
  transport/transport.c
  frame/frame.c
  scenario/scenario.c
  engine/engine.c
  sim/sim_link.c
  telemetry/record.c
  telemetry/ring.c
  telemetry/histogram.c
  telemetry/telemetry.c
  arbiter/arbiter.c
  concurrent/producer.c
  concurrent/concurrent.c
)

enable_testing()
add_executable(bench_portable_tests
  tests/test_portable.c
  tests/test_crc.c
  tests/test_frame.c
  tests/test_protocol.c
  tests/test_scheduler.c
  tests/test_transport.c
  tests/test_counters.c
  tests/test_profile.c
  tests/test_engine.c
  tests/test_sim.c
  tests/test_telemetry.c
  tests/test_concurrent.c
)
target_link_libraries(bench_portable_tests bench_portable)
# Golden INTER-LANGAGE : le meme fichier est lu par les tests C (reproduction
# octet a octet de l'encodeur) et par les tests Python (decodage et statistiques).
# Source unique de verite : aucune duplication du flux de reference.
target_compile_definitions(bench_portable_tests PRIVATE
  BENCH_GOLDEN_TELEMETRY_HEX="${CMAKE_CURRENT_SOURCE_DIR}/../../../tools/measurement/golden/telemetry/stream.hex")
add_test(NAME bench_portable_tests COMMAND bench_portable_tests)

# --- Verification de CABLAGE (compile-only) --------------------------------
# Les board applications + la HAL squelette compilent contre l'API PORTABLE.
# Ceci garantit l'absence de DERIVE d'API entre le coeur et les adaptateurs.
# Ce n'est PAS un build materiel : aucune toolchain embarquee (Pico SDK /
# ESP-IDF), aucun flash ; les primitives HAL sont des stubs. Compile mais NON
# execute (pas de add_test) : le comportement reel se valide au premier essai
# materiel local.
set(BENCH_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(bench_slave_wiring
  ${BENCH_ROOT}/boards/rp2040_reference/main.c
  ${BENCH_ROOT}/hal/rp2040/hal_rp2040.c
  ${BENCH_ROOT}/scenarios/demo/demo_profile.c
  ${BENCH_ROOT}/scenarios/demo/demo_scenario.c
)
target_include_directories(bench_slave_wiring PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR} ${BENCH_ROOT})
target_link_libraries(bench_slave_wiring bench_portable)

add_executable(bench_host_wiring
  ${BENCH_ROOT}/boards/xiao_esp32s3/main.c
  ${BENCH_ROOT}/hal/esp32/hal_esp32.c
  ${BENCH_ROOT}/scenarios/demo/demo_profile.c
  ${BENCH_ROOT}/scenarios/demo/demo_scenario.c
)
target_include_directories(bench_host_wiring PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR} ${BENCH_ROOT})
target_link_libraries(bench_host_wiring bench_portable)

# --- Definition de campagne EXP-L1-BRINGUP-001 (B3) -------------------------
# Cible SEPAREE du coeur : une campagne n'ajoute qu'un scenario declaratif et ne
# modifie pas le coeur. Les tests verifient que les profils sont rejouables et
# que la table des cas est coherente. AUCUNE execution, aucune mesure.
add_executable(bench_l1_campaign_tests
  ${BENCH_ROOT}/scenarios/l1_spi_bringup/tests/test_l1_campaign.c
  ${BENCH_ROOT}/scenarios/l1_spi_bringup/l1_profile.c
  ${BENCH_ROOT}/scenarios/l1_spi_bringup/l1_scenario.c
)
target_include_directories(bench_l1_campaign_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR} ${BENCH_ROOT})
target_link_libraries(bench_l1_campaign_tests bench_portable)
add_test(NAME bench_l1_campaign_tests COMMAND bench_l1_campaign_tests)

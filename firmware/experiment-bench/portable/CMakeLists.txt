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
)

enable_testing()
add_executable(bench_portable_tests tests/test_portable.c)
target_link_libraries(bench_portable_tests bench_portable)
add_test(NAME bench_portable_tests COMMAND bench_portable_tests)

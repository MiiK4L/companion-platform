# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0

# Build HOST du squelette (preuve d'architecture). Compile et exécute les tests
# host avec un compilateur C standard — AUCUNE dépendance ESP-IDF, aucun moteur
# réel, aucun framework de test tiers (choix non figé).
cmake_minimum_required(VERSION 3.16)
project(companion_host_skeleton C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT MSVC)
  add_compile_options(-Wall -Wextra -Werror)
endif()

# Racine d'inclusion : permet #include "ports/…", "services/…", etc.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(host_tests
  services/app_manager.c
  adapters/host/fake_runtime.c
  adapters/host/mock_storage.c
  adapters/host/mock_log.c
  composition/composition_root.c
  tests/test_app_manager.c
  tests/test_main.c)

enable_testing()
add_test(NAME host_tests COMMAND host_tests)

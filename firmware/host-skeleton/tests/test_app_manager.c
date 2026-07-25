// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Tests host de l'AppManager via la composition root, avec des adaptateurs
// mock/bouchon. Prouve que l'AppManager fonctionne SANS moteur réel (FakeRuntime)
// et ne dépend que des ports.
#include "composition/composition_root.h"
#include "tests/test_support.h"

static const unsigned char kUnit[] = {0x01, 0x02, 0x03};

// Lancement nominal : storage sert l'unité, runtime (bouchon) renvoie OK.
void test_launch_success(void) {
  HostComposition c;
  host_composition_build(&c, "hello", kUnit, sizeof(kUnit), RT_OK, RT_OK);

  am_status_t r = app_manager_launch(&c.app_manager, "hello");

  EXPECT_EQ_INT(r, AM_OK, "launch should succeed");
  EXPECT_EQ_INT(c.runtime.load_calls, 1, "runtime.load called once");
  EXPECT_EQ_INT(c.runtime.run_calls, 1, "runtime.run called once");
  EXPECT_EQ_INT(c.log.count[LOG_INFO], 1, "one INFO log on success");
}

// App absente du storage : erreur storage, runtime jamais appelé.
void test_storage_not_found(void) {
  HostComposition c;
  host_composition_build(&c, "hello", kUnit, sizeof(kUnit), RT_OK, RT_OK);

  am_status_t r = app_manager_launch(&c.app_manager, "absent");

  EXPECT_EQ_INT(r, AM_ERR_STORAGE, "missing app -> storage error");
  EXPECT_EQ_INT(c.runtime.load_calls, 0, "runtime not called on storage error");
  EXPECT_EQ_INT(c.log.count[LOG_WARN], 1, "one WARN log on storage error");
}

// Le runtime (bouchon) échoue au load : l'AppManager remonte l'erreur runtime.
void test_runtime_load_fail(void) {
  HostComposition c;
  host_composition_build(&c, "hello", kUnit, sizeof(kUnit), RT_ERR_LOAD, RT_OK);

  am_status_t r = app_manager_launch(&c.app_manager, "hello");

  EXPECT_EQ_INT(r, AM_ERR_RUNTIME, "load failure -> runtime error");
  EXPECT_EQ_INT(c.runtime.run_calls, 0, "run not called if load fails");
  EXPECT_EQ_INT(c.log.count[LOG_ERROR], 1, "one ERROR log on runtime failure");
}

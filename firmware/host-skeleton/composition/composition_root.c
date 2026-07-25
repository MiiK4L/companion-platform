// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "composition/composition_root.h"

void host_composition_build(HostComposition *c, const char *app_key,
                            const uint8_t *app_unit, size_t app_len,
                            rt_status_t load_result, rt_status_t run_result) {
  // 1) Initialiser les adaptateurs host (possédés par la composition).
  fake_runtime_init(&c->runtime, load_result, run_result);
  mock_storage_init(&c->storage, app_key, app_unit, app_len);
  mock_log_init(&c->log);

  // 2) Injecter leurs PORTS dans le service (le service ne connaît que les ports).
  app_manager_init(&c->app_manager, mock_storage_port(&c->storage),
                   fake_runtime_port(&c->runtime), mock_log_port(&c->log));
}

// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "composition/composition_root.h"

void host_composition_build(HostComposition *c, AppReference known_ref,
                            void *fake_handle, rt_status_t launch_result) {
  // 1) Initialiser les adaptateurs host (possédés par la composition).
  mock_app_source_init(&c->source, known_ref, fake_handle);
  fake_runtime_init(&c->runtime, launch_result);
  mock_log_init(&c->log);

  // 2) Injecter leurs PORTS dans le service (le service ne connaît que les ports).
  app_manager_init(&c->app_manager, mock_app_source_port(&c->source),
                   fake_runtime_port(&c->runtime), mock_log_port(&c->log));
}

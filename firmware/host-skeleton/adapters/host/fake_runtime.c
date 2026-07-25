// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "adapters/host/fake_runtime.h"

// Bouchon : ne fait qu'incrémenter un compteur et renvoyer le statut préréglé.
static rt_status_t fr_load(void *self, const uint8_t *unit, size_t len) {
  (void)unit;
  (void)len;  // le bouchon n'INTERPRÈTE rien.
  FakeRuntime *fr = (FakeRuntime *)self;
  fr->load_calls++;
  return fr->load_result;
}

static rt_status_t fr_run(void *self) {
  FakeRuntime *fr = (FakeRuntime *)self;
  fr->run_calls++;
  return fr->run_result;
}

void fake_runtime_init(FakeRuntime *fr, rt_status_t load_result,
                       rt_status_t run_result) {
  fr->load_result = load_result;
  fr->run_result = run_result;
  fr->load_calls = 0;
  fr->run_calls = 0;
}

IRuntime fake_runtime_port(FakeRuntime *fr) {
  IRuntime p;
  p.self = fr;
  p.load = fr_load;
  p.run = fr_run;
  return p;
}

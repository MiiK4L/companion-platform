// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "adapters/host/fake_runtime.h"

// Bouchon : compte l'appel, note l'id (opaque) reçu, renvoie le statut préréglé.
// Il n'INTERPRÈTE rien de l'artefact (ni handle, ni contenu).
static rt_status_t fr_launch(void *self, AppArtifactView artifact) {
  FakeRuntime *fr = (FakeRuntime *)self;
  fr->launch_calls++;
  fr->last_artifact_id = artifact.id;  // conservé pour assertion, non déréférencé
  return fr->launch_result;
}

void fake_runtime_init(FakeRuntime *fr, rt_status_t launch_result) {
  fr->launch_result = launch_result;
  fr->launch_calls = 0;
  fr->last_artifact_id = 0;
}

IRuntime fake_runtime_port(FakeRuntime *fr) {
  IRuntime p;
  p.self = fr;
  p.launch = fr_launch;
  return p;
}

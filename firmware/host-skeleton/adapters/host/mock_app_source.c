// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "adapters/host/mock_app_source.h"

static int str_eq(const char *a, const char *b) {
  while (*a && (*a == *b)) {
    a++;
    b++;
  }
  return *a == *b;
}

static as_status_t mas_resolve(void *self, const char *app_ref,
                               AppArtifactView *out) {
  MockAppSource *ms = (MockAppSource *)self;
  if (!str_eq(app_ref, ms->known_ref)) return AS_NOT_FOUND;
  out->id = app_ref;            // id opaque (diagnostic)
  out->handle = ms->fake_handle;  // poignée opaque
  return AS_OK;
}

void mock_app_source_init(MockAppSource *ms, const char *known_ref,
                          void *fake_handle) {
  ms->known_ref = known_ref;
  ms->fake_handle = fake_handle;
}

IAppSource mock_app_source_port(MockAppSource *ms) {
  IAppSource p;
  p.self = ms;
  p.resolve = mas_resolve;
  return p;
}

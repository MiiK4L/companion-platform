// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "adapters/host/mock_app_source.h"

#include <string.h>

// Égalité de références OPAQUES : comparaison stricte par octets (taille + contenu).
// Aucune sémantique de chaîne (pas de terminaison, pas de casse) n'est supposée.
static int ref_eq(AppReference a, AppReference b) {
  if (a.size != b.size) return 0;
  if (a.size == 0) return 1;
  return memcmp(a.data, b.data, a.size) == 0;
}

static as_status_t mas_resolve(void *self, AppReference reference,
                               AppArtifactView *out) {
  MockAppSource *ms = (MockAppSource *)self;
  if (!ref_eq(reference, ms->known_ref)) return AS_NOT_FOUND;
  // Libellé de diagnostic possédé par l'adaptateur (emprunté par la vue) — sans
  // rapport avec le contenu de la référence, qui reste opaque.
  out->id = "mock-app-source";
  out->handle = ms->fake_handle;  // poignée opaque
  return AS_OK;
}

void mock_app_source_init(MockAppSource *ms, AppReference known_ref,
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

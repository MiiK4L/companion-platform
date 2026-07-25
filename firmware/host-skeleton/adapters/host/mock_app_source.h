// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// MockAppSource — adaptateur host de test du port IAppSource. Résout une
// référence connue en une vue OPAQUE (poignée factice), sinon AS_NOT_FOUND.
// Aucun stockage réel ; ne présume ni format ni mode d'accès.
#ifndef COMPANION_ADAPTERS_HOST_MOCK_APP_SOURCE_H
#define COMPANION_ADAPTERS_HOST_MOCK_APP_SOURCE_H

#include "ports/iappsource.h"

typedef struct MockAppSource {
  const char *known_ref;  // référence reconnue
  void *fake_handle;      // poignée opaque servie (non interprétée)
} MockAppSource;

void mock_app_source_init(MockAppSource *ms, const char *known_ref,
                          void *fake_handle);

IAppSource mock_app_source_port(MockAppSource *ms);

#endif  // COMPANION_ADAPTERS_HOST_MOCK_APP_SOURCE_H

// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// MockStorage — adaptateur host de test : sert un unique blob pour une clé
// donnée, sinon ST_ERR_NOT_FOUND. Aucun accès flash/NVS réel.
#ifndef COMPANION_ADAPTERS_HOST_MOCK_STORAGE_H
#define COMPANION_ADAPTERS_HOST_MOCK_STORAGE_H

#include "ports/istorage.h"

typedef struct MockStorage {
  const char *key;       // clé reconnue
  const uint8_t *blob;   // contenu servi (non possédé)
  size_t blob_len;
} MockStorage;

void mock_storage_init(MockStorage *ms, const char *key, const uint8_t *blob,
                       size_t blob_len);

IStorage mock_storage_port(MockStorage *ms);

#endif  // COMPANION_ADAPTERS_HOST_MOCK_STORAGE_H

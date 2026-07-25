// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "adapters/host/mock_storage.h"

static int str_eq(const char *a, const char *b) {
  while (*a && (*a == *b)) {
    a++;
    b++;
  }
  return *a == *b;
}

static st_status_t ms_read(void *self, const char *key, uint8_t *out,
                           size_t cap, size_t *out_len) {
  MockStorage *ms = (MockStorage *)self;
  if (!str_eq(key, ms->key)) return ST_ERR_NOT_FOUND;
  if (ms->blob_len > cap) return ST_ERR_TOO_LARGE;  // borne respectée
  for (size_t i = 0; i < ms->blob_len; i++) out[i] = ms->blob[i];
  *out_len = ms->blob_len;
  return ST_OK;
}

void mock_storage_init(MockStorage *ms, const char *key, const uint8_t *blob,
                       size_t blob_len) {
  ms->key = key;
  ms->blob = blob;
  ms->blob_len = blob_len;
}

IStorage mock_storage_port(MockStorage *ms) {
  IStorage p;
  p.self = ms;
  p.read = ms_read;
  return p;
}

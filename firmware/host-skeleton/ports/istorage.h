// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Port IStorage — SIGNATURE CANDIDATE (non figée). Lecture d'un blob nommé
// (ex. un Manifest depuis un stockage). Aucune dépendance à NVS/LittleFS/ESP-IDF.
#ifndef COMPANION_PORTS_ISTORAGE_H
#define COMPANION_PORTS_ISTORAGE_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
  ST_OK = 0,
  ST_ERR_NOT_FOUND = 1,
  ST_ERR_TOO_LARGE = 2,
} st_status_t;

typedef struct IStorage {
  void *self;
  // Lit au plus `cap` octets de l'entrée `key` dans `out` ; `*out_len` = taille lue.
  st_status_t (*read)(void *self, const char *key, uint8_t *out, size_t cap,
                      size_t *out_len);
} IStorage;

#endif  // COMPANION_PORTS_ISTORAGE_H

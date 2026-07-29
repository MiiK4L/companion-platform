// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Codec du protocole de controle (texte, deterministe). Purement portable :
// analyse une commande, serialise une reponse. Aucune E/S : l'appelant fournit
// les octets (via la HAL serie). Aucune dependance plateforme.
#ifndef BENCH_PROTOCOL_H
#define BENCH_PROTOCOL_H

#include <stddef.h>

#include "counters/counters.h"

typedef enum {
  BENCH_CMD_UNKNOWN = 0,
  BENCH_CMD_SELECT_PROFILE = 1,  // "SELECT <profile_id>"
  BENCH_CMD_START = 2,           // "START"
  BENCH_CMD_STOP = 3,            // "STOP"
  BENCH_CMD_READ_COUNTERS = 4,   // "COUNTERS"
} bench_cmd_type_t;

typedef struct {
  bench_cmd_type_t type;
  char arg[64];  // argument (ex. profile_id), tronque si trop long
} bench_cmd_t;

// Analyse une ligne de commande (sans retour a la ligne). Deterministe.
bench_cmd_t bench_protocol_parse(const char *line);

// Serialise les compteurs BRUTS en une ligne stable ; retourne la longueur
// ecrite (hors NUL) ou -1 si la capacite est insuffisante.
int bench_protocol_format_counters(char *out, size_t cap,
                                   const bench_counters_t *counters);

#endif  // BENCH_PROTOCOL_H

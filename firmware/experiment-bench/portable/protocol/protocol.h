// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Protocole de controle BINAIRE, borne et deterministe. Il ne depend NI de
// printf NI de lignes de texte : les messages sont des octets, transportables
// sur UART, USB-CDC ou un transport simule. Le FRAMING, la VERSION et le CRC
// sont assures par la couche trame (frame.h) qui transporte ces messages ; ce
// module ne fait qu'encoder/decoder le PAYLOAD de controle (type + arguments).
//
// Une eventuelle console humaine (texte -> binaire) sera un ADAPTATEUR externe,
// jamais le contrat autoritaire.
//
// Message (payload de trame) : type[1] | arg_len[1] | arg[arg_len]
#ifndef BENCH_PROTOCOL_H
#define BENCH_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#include "counters/counters.h"

typedef enum {
  BENCH_CMD_UNKNOWN = 0,
  BENCH_CMD_SELECT_PROFILE = 1,   // arg = profile_id
  BENCH_CMD_START = 2,
  BENCH_CMD_STOP = 3,
  BENCH_CMD_READ_COUNTERS = 4,
  BENCH_CMD_RESET_COUNTERS = 5,
  BENCH_CMD_GET_CAPABILITIES = 6,
} bench_cmd_type_t;

// Codes de statut EXPLICITES retournes par le decodage/traitement.
typedef enum {
  BENCH_PROTO_OK = 0,
  BENCH_PROTO_UNKNOWN_CMD = 1,   // type de commande non reconnu
  BENCH_PROTO_BAD_ARG = 2,       // argument absent, trop long ou invalide
  BENCH_PROTO_BAD_LENGTH = 3,    // longueur incoherente avec le contenu
  BENCH_PROTO_TRUNCATED = 4,     // message trop court
} bench_proto_status_t;

#define BENCH_PROFILE_ID_MAX 32u  // longueur max d'un profile_id (borne stricte)

typedef struct {
  bench_cmd_type_t type;
  uint8_t profile_id_len;                    // longueur utile de profile_id
  char profile_id[BENCH_PROFILE_ID_MAX];     // pour SELECT_PROFILE (non NUL-borne)
} bench_cmd_t;

// Encode une commande dans "out". Retourne la longueur ecrite (>= 2) ou -1 si
// "cap" est insuffisant ou si l'argument depasse la borne (REJET, pas troncature).
int bench_protocol_encode_cmd(uint8_t *out, size_t cap, const bench_cmd_t *cmd);

// Decode un message de controle BORNE. Un argument trop long est REJETE
// (BENCH_PROTO_BAD_ARG), jamais tronque silencieusement.
bench_proto_status_t bench_protocol_decode_cmd(const uint8_t *in, size_t len,
                                               bench_cmd_t *out);

// Taille fixe de la reponse "compteurs" serialisee (7 x u32 + 3 x u64).
#define BENCH_COUNTERS_WIRE_SIZE (7u * 4u + 3u * 8u)  // 52

// Serialise / deserialise les compteurs BRUTS (format binaire fixe, gros-boutiste).
int bench_protocol_encode_counters(uint8_t *out, size_t cap,
                                   const bench_counters_t *c);
bench_proto_status_t bench_protocol_decode_counters(const uint8_t *in, size_t len,
                                                    bench_counters_t *out);

#endif  // BENCH_PROTOCOL_H

// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Format de TRAME portable, VERSIONNE et protege par CRC-32. C'est l'unite
// d'echange sur le fil (SPI aujourd'hui, autre transport demain). Le decodeur
// est strictement BORNE : il rejette toute trame tronquee, incoherente, de
// version inconnue ou dont le CRC ne correspond pas.
//
// Disposition (gros-boutiste pour tous les champs multi-octets) :
//
//   offset 0 : magic[2]          = {0xC5, 0xB5}
//   offset 2 : version[1]        = BENCH_FRAME_VERSION
//   offset 3 : seq[4]            numero de sequence
//   offset 7 : payload_len[2]    longueur du payload (<= BENCH_FRAME_MAX_PAYLOAD)
//   offset 9 : payload[payload_len]
//   offset 9+payload_len : crc32[4]   CRC-32/IEEE des octets [0 .. 9+payload_len)
//
// Le CRC couvre l'entete ET le payload (tout ce qui precede le champ CRC).
#ifndef BENCH_FRAME_H
#define BENCH_FRAME_H

#include <stddef.h>
#include <stdint.h>

#define BENCH_FRAME_MAGIC0 0xC5u
#define BENCH_FRAME_MAGIC1 0xB5u
#define BENCH_FRAME_VERSION 1u

#define BENCH_FRAME_HEADER_SIZE 9u   // magic(2)+version(1)+seq(4)+len(2)
#define BENCH_FRAME_CRC_SIZE 4u
#define BENCH_FRAME_OVERHEAD (BENCH_FRAME_HEADER_SIZE + BENCH_FRAME_CRC_SIZE)  // 13
#define BENCH_FRAME_MAX_PAYLOAD 256u
#define BENCH_FRAME_MAX_SIZE (BENCH_FRAME_OVERHEAD + BENCH_FRAME_MAX_PAYLOAD)

typedef enum {
  BENCH_FRAME_OK = 0,
  BENCH_FRAME_ERR_TRUNCATED = 1,    // moins d'octets que le minimum requis
  BENCH_FRAME_ERR_BAD_MAGIC = 2,
  BENCH_FRAME_ERR_BAD_VERSION = 3,
  BENCH_FRAME_ERR_BAD_LENGTH = 4,   // payload_len > MAX ou incoherent avec "len"
  BENCH_FRAME_ERR_BAD_CRC = 5,      // corruption detectee
  BENCH_FRAME_ERR_NOSPACE = 6,      // (encode) buffer de sortie trop petit
} bench_frame_result_t;

typedef struct {
  uint8_t version;
  uint32_t seq;
  uint16_t payload_len;
  uint8_t payload[BENCH_FRAME_MAX_PAYLOAD];
} bench_frame_t;

// Encode une trame. Retourne la longueur totale ecrite (> 0) ou -1 si le payload
// depasse BENCH_FRAME_MAX_PAYLOAD ou si "cap" est insuffisant.
int bench_frame_encode(uint8_t *out, size_t cap, uint32_t seq,
                       const uint8_t *payload, size_t payload_len);

// Decode la trame situee AU DEBUT d'un tampon de "avail" octets (le reste, par
// ex. du bourrage full-duplex, est ignore). Borne : rejette magic/version/CRC
// invalides, longueur > MAX et trame incompletement presente (TRUNCATED). En cas
// de succes, "*consumed" recoit la taille reelle de la trame.
bench_frame_result_t bench_frame_decode_prefix(const uint8_t *in, size_t avail,
                                               bench_frame_t *out, size_t *consumed);

// Decode une trame BORNEE dont "len" doit correspondre EXACTEMENT a la trame
// (aucun octet en trop). Remplit "out" en cas de succes.
bench_frame_result_t bench_frame_decode(const uint8_t *in, size_t len,
                                        bench_frame_t *out);

// --- Injection de faute REELLE (corrompt les octets, pas un compteur) ---
// Corrompt l'octet de payload d'index "payload_index" d'une trame encodee.
// Retourne 0 si applique, -1 si l'index est hors du payload.
int bench_frame_corrupt_payload(uint8_t *frame, size_t len, size_t payload_index);
// Corrompt le champ CRC d'une trame encodee. Retourne 0/-1.
int bench_frame_corrupt_crc(uint8_t *frame, size_t len);

#endif  // BENCH_FRAME_H

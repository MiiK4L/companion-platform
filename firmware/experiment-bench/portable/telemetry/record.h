// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Enregistrement de TELEMETRIE par transaction : la donnee BRUTE autoritaire de
// la latence. Le coeur n'agrege plus seulement des compteurs : il produit un
// echantillon par transaction, horodate en TICKS BRUTS, que l'outillage
// reconstruit en serie complete.
//
// La LATENCE n'est PAS transportee : elle est recalculee depuis t_start/t_end
// avec l'arithmetique WRAP-SAFE du coeur (scheduler.h). Transporter une valeur
// redondante autoriserait une contradiction entre elle et ses entrees.
//
// Un echantillon porte UN SEUL statut terminal, afin que l'identite de
// reconciliation ne double-compte jamais :
//
//   issued = ok + timeout + rejected + unpaired + duplicate + out_of_order
//            + producer_drop
//
// (les pertes de TRANSPORT sont reconciliees separement, par les numeros de
// sequence de trame ; elles ne sont pas des transactions executees.)
#ifndef BENCH_TELEMETRY_RECORD_H
#define BENCH_TELEMETRY_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "events/events.h"  // bench_ticks_t

// Statut TERMINAL d'un echantillon (un seul par transaction).
typedef enum {
  BENCH_SAMPLE_OK = 0,            // transaction terminee et valide
  BENCH_SAMPLE_TIMEOUT = 1,       // budget epuise (EXCLU de la distribution)
  BENCH_SAMPLE_REJECTED = 2,      // trame rejetee (CRC/format)
  BENCH_SAMPLE_UNPAIRED = 3,      // debut sans fin, ou appariement impossible
  BENCH_SAMPLE_DUPLICATE = 4,     // numero de sequence deja vu
  BENCH_SAMPLE_OUT_OF_ORDER = 5,  // sequence hors ordre (distinct de duplicate)
} bench_sample_status_t;

// Drapeaux (bitmask) : contexte non terminal, n'influe pas sur le statut.
#define BENCH_SAMPLE_FLAG_FAULT_CRC 0x01u      // faute CRC injectee par le profil
#define BENCH_SAMPLE_FLAG_FAULT_TIMEOUT 0x02u  // timeout injecte par le profil

typedef struct {
  uint32_t sequence_id;
  bench_ticks_t t_start;  // ticks BRUTS (aucune conversion embarquee)
  bench_ticks_t t_end;    // ticks BRUTS
  uint8_t status;         // bench_sample_status_t
  uint8_t flags;
} bench_sample_t;

// Taille encodee, FIXE : u32 + u64 + u64 + u8 + u8.
#define BENCH_SAMPLE_WIRE_SIZE 22u

// Encode/decode un echantillon (gros-boutiste). Retourne le nombre d'octets
// ecrits/lus, ou 0 si la capacite est insuffisante.
size_t bench_sample_encode(uint8_t *out, size_t cap, const bench_sample_t *s);
size_t bench_sample_decode(const uint8_t *in, size_t len, bench_sample_t *out);

#endif  // BENCH_TELEMETRY_RECORD_H

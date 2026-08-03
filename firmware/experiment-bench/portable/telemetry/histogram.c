// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "telemetry/histogram.h"

#include <stddef.h>

#include "util/sat.h"

int bench_histogram_init(bench_histogram_t *h, const bench_ticks_t *edges,
                         uint32_t bin_count, uint32_t *counts, uint32_t version) {
  h->edges = NULL;
  h->bin_count = 0;
  h->counts = NULL;
  h->underflow = 0;
  h->overflow = 0;
  h->sample_count = 0;
  h->version = version;

  if (edges == NULL || counts == NULL) {
    return 0;
  }
  if (bin_count == 0 || bin_count > BENCH_HISTOGRAM_MAX_BINS) {
    return 0;
  }
  // Bornes STRICTEMENT croissantes : sinon l'appartenance a une classe serait
  // ambigue et l'histogramme silencieusement faux.
  for (uint32_t i = 0; i < bin_count; i++) {
    if (!(edges[i] < edges[i + 1])) {
      return 0;
    }
  }
  for (uint32_t i = 0; i < bin_count; i++) {
    counts[i] = 0;
  }
  h->edges = edges;
  h->bin_count = bin_count;
  h->counts = counts;
  return 1;
}

void bench_histogram_add(bench_histogram_t *h, bench_ticks_t value) {
  if (h->edges == NULL || h->counts == NULL || h->bin_count == 0) {
    return;  // histogramme non initialise : ne fabrique aucun chiffre
  }
  bench_sat_inc_u32(&h->sample_count);

  if (value < h->edges[0]) {
    bench_sat_inc_u32(&h->underflow);
    return;
  }
  if (value >= h->edges[h->bin_count]) {
    bench_sat_inc_u32(&h->overflow);
    return;
  }
  // Classe [edges[i], edges[i+1]) : borne basse incluse, borne haute exclue.
  for (uint32_t i = 0; i < h->bin_count; i++) {
    if (value >= h->edges[i] && value < h->edges[i + 1]) {
      bench_sat_inc_u32(&h->counts[i]);
      return;
    }
  }
}

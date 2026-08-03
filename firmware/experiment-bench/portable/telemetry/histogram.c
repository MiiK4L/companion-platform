// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

#include "telemetry/histogram.h"

#include <stddef.h>

// Incremente en SATURANT et signale si la saturation a eu lieu : au-dela,
// l'histogramme n'est plus reconciliable.
static void inc_saturating(bench_histogram_t *h, uint32_t *counter) {
  if (*counter == UINT32_MAX) {
    h->saturated = 1;
    return;
  }
  (*counter)++;
}

int bench_histogram_init(bench_histogram_t *h, const bench_ticks_t *edges,
                         uint32_t bin_count, uint32_t *counts, uint32_t version) {
  h->edges = NULL;
  h->bin_count = 0;
  h->counts = NULL;
  h->underflow = 0;
  h->overflow = 0;
  h->sample_count = 0;
  h->version = version;
  h->saturated = 0;

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
  inc_saturating(h, &h->sample_count);

  if (value < h->edges[0]) {
    inc_saturating(h, &h->underflow);
    return;
  }
  if (value >= h->edges[h->bin_count]) {
    inc_saturating(h, &h->overflow);
    return;
  }
  // Classe [edges[i], edges[i+1]) : borne basse incluse, borne haute exclue.
  for (uint32_t i = 0; i < h->bin_count; i++) {
    if (value >= h->edges[i] && value < h->edges[i + 1]) {
      inc_saturating(h, &h->counts[i]);
      return;
    }
  }
}

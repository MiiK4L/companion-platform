// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Arbitrage et execution concurrente : atomicite, tie-break documente, FIFO
// stable, tourniquet avec producteur inactif, famine, saturation, invariants de
// topologie (bus_wait nul en separe, egalite stricte des modes isoles) et
// distinction des causes de timeout.
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "arbiter/arbiter.h"
#include "concurrent/concurrent.h"
#include "profiles/profile.h"
#include "telemetry/record.h"
#include "tests/test_support.h"
#include "tests/tests.h"

// --- Collecteur de resultats ------------------------------------------------
typedef struct {
  bench_conc_result_t items[512];
  uint32_t n;
} collect_t;

static void collect(void *ctx, const bench_conc_result_t *r) {
  collect_t *c = (collect_t *)ctx;
  if (c->n < 512) {
    c->items[c->n++] = *r;
  }
}

static bench_profile_t make_profile(const char *id, uint64_t seed, uint32_t count,
                                    uint32_t packet, uint32_t delay,
                                    uint32_t timeout) {
  bench_profile_t p;
  memset(&p, 0, sizeof(p));
  p.profile_id = id;
  p.profile_version = 1;
  p.seed = seed;
  p.transaction_count = count;
  p.packet_size = packet;
  p.inter_delay_ticks = delay;
  p.timeout_ticks = timeout;
  return p;
}

// --- Atomicite : aucune attribution tant que le bus est detenu --------------
static void test_atomicite(void) {
  bench_bus_request_t slots[8];
  bench_arbiter_t arb;
  bench_arbiter_init(&arb, BENCH_ARB_FIFO, slots, 8, 0);

  EXPECT_EQ_INT(bench_arbiter_request(&arb, 0, 10), 1, "demande A");
  EXPECT_EQ_INT(bench_arbiter_request(&arb, 1, 10), 1, "demande B");

  bench_ticks_t wait = 0;
  EXPECT_EQ_INT(bench_arbiter_grant(&arb, 10, 100, &wait), 0, "A servi");
  EXPECT(bench_arbiter_busy(&arb, 10), "bus detenu");
  // Pendant toute la transaction, AUCUNE autre attribution.
  EXPECT_EQ_INT(bench_arbiter_grant(&arb, 50, 100, &wait), BENCH_ARB_NO_OWNER,
                "aucune preemption au milieu d'une transaction");
  EXPECT_EQ_INT(bench_arbiter_grant(&arb, 109, 100, &wait), BENCH_ARB_NO_OWNER,
                "toujours detenu juste avant la fin");
  EXPECT(!bench_arbiter_busy(&arb, 110), "libere a la fin de la transaction");
  EXPECT_EQ_INT(bench_arbiter_grant(&arb, 110, 100, &wait), 1,
                "preemption possible ENTRE deux transactions");
  EXPECT_EQ_INT((long long)wait, 100, "attente de B = 100 ticks");
}

// --- Tie-break documente : demandes simultanees ----------------------------
static void test_tie_break_simultane(void) {
  bench_bus_request_t slots[8];
  bench_arbiter_t arb;
  bench_arbiter_init(&arb, BENCH_ARB_FIFO, slots, 8, 0);
  // Trois demandes au MEME tick : departagees par le rang d'arrivee.
  (void)bench_arbiter_request(&arb, 2, 5);
  (void)bench_arbiter_request(&arb, 0, 5);
  (void)bench_arbiter_request(&arb, 1, 5);
  bench_ticks_t w = 0;
  EXPECT_EQ_INT(bench_arbiter_grant(&arb, 5, 1, &w), 2, "1er arrive, 1er servi");
  EXPECT_EQ_INT(bench_arbiter_grant(&arb, 6, 1, &w), 0, "2e arrive");
  EXPECT_EQ_INT(bench_arbiter_grant(&arb, 7, 1, &w), 1, "3e arrive");
}

// --- FIFO stable : plusieurs requetes au meme tick --------------------------
static void test_fifo_stable(void) {
  bench_bus_request_t slots[8];
  bench_arbiter_t arb;
  bench_arbiter_init(&arb, BENCH_ARB_FIFO, slots, 8, 0);
  for (uint8_t i = 0; i < 4; i++) {
    (void)bench_arbiter_request(&arb, i % 2, 3);  // 0,1,0,1 au meme tick
  }
  bench_ticks_t w = 0;
  const uint8_t expected[4] = {0, 1, 0, 1};
  for (uint32_t i = 0; i < 4; i++) {
    EXPECT_EQ_INT(bench_arbiter_grant(&arb, 3 + i, 1, &w), expected[i],
                  "ordre d'arrivee strictement preserve");
  }
}

// --- Tourniquet : producteur temporairement inactif, puis reactivation ------
static void test_round_robin(void) {
  bench_bus_request_t slots[8];
  bench_arbiter_t arb;
  bench_arbiter_init(&arb, BENCH_ARB_ROUND_ROBIN, slots, 8, 0);
  bench_ticks_t w = 0;

  (void)bench_arbiter_request(&arb, 0, 0);
  (void)bench_arbiter_request(&arb, 1, 0);
  EXPECT_EQ_INT(bench_arbiter_grant(&arb, 0, 1, &w), 0, "alternance : A");
  EXPECT_EQ_INT(bench_arbiter_grant(&arb, 1, 1, &w), 1, "alternance : B");

  // B devient inactif : A ne doit pas etre bloque par le tourniquet.
  (void)bench_arbiter_request(&arb, 0, 2);
  (void)bench_arbiter_request(&arb, 0, 3);
  EXPECT_EQ_INT(bench_arbiter_grant(&arb, 2, 1, &w), 0, "A sert malgre B inactif");
  EXPECT_EQ_INT(bench_arbiter_grant(&arb, 3, 1, &w), 0, "A sert encore");

  // Reactivation de B : il reprend sa place a l'alternance suivante.
  (void)bench_arbiter_request(&arb, 0, 4);
  (void)bench_arbiter_request(&arb, 1, 4);
  EXPECT_EQ_INT(bench_arbiter_grant(&arb, 4, 1, &w), 1, "B reprend apres reactivation");
}

// --- Famine : absente sous FIFO/tourniquet, detectee sous priorite fixe -----
static void test_famine(void) {
  bench_ticks_t w = 0;
  // Priorite fixe (mode de STRESS) : le producteur faible est affame.
  bench_bus_request_t s1[16];
  bench_arbiter_t prio;
  bench_arbiter_init(&prio, BENCH_ARB_FIXED_PRIORITY, s1, 16, 5);
  bench_arbiter_set_priority(&prio, 0, 0);  // fort
  bench_arbiter_set_priority(&prio, 1, 9);  // faible
  (void)bench_arbiter_request(&prio, 1, 0);  // le faible arrive en premier
  for (uint32_t t = 0; t < 8; t++) {
    (void)bench_arbiter_request(&prio, 0, t);
    EXPECT_EQ_INT(bench_arbiter_grant(&prio, t, 1, &w), 0, "le fort passe devant");
  }
  EXPECT_EQ_INT(bench_arbiter_grant(&prio, 20, 1, &w), 1, "le faible finit par passer");
  EXPECT(prio.max_bus_wait_ticks[1] >= 20, "attente maximale du faible enregistree");
  EXPECT(prio.requests_over_starvation_threshold[1] >= 1, "famine DETECTEE");
  EXPECT_EQ_INT(prio.requests_over_starvation_threshold[0], 0,
                "le fort n'est jamais affame");

  // FIFO : aucune famine possible.
  bench_bus_request_t s2[16];
  bench_arbiter_t fifo;
  bench_arbiter_init(&fifo, BENCH_ARB_FIFO, s2, 16, 5);
  (void)bench_arbiter_request(&fifo, 1, 0);
  for (uint32_t t = 0; t < 6; t++) {
    (void)bench_arbiter_request(&fifo, 0, t);
  }
  EXPECT_EQ_INT(bench_arbiter_grant(&fifo, 0, 1, &w), 1, "FIFO sert le premier arrive");
  EXPECT_EQ_INT(fifo.requests_over_starvation_threshold[1], 0,
                "aucune famine sous FIFO");
}

// --- File bornee : debordement compte PAR PRODUCTEUR ------------------------
static void test_file_bornee(void) {
  bench_bus_request_t slots[2];
  bench_arbiter_t arb;
  bench_arbiter_init(&arb, BENCH_ARB_FIFO, slots, 2, 0);
  EXPECT_EQ_INT(bench_arbiter_request(&arb, 0, 0), 1, "1re demande");
  EXPECT_EQ_INT(bench_arbiter_request(&arb, 1, 0), 1, "2e demande");
  EXPECT_EQ_INT(bench_arbiter_request(&arb, 1, 0), 0, "file pleine");
  EXPECT_EQ_INT(arb.overflow_by_producer[1], 1, "debordement impute au bon producteur");
  EXPECT_EQ_INT(arb.overflow_by_producer[0], 0, "aucun debordement pour l'autre");
  EXPECT_EQ_INT(bench_arbiter_depth(&arb), 2, "profondeur totale");
  EXPECT_EQ_INT((long long)bench_arbiter_oldest_pending_age(&arb, 7), 7,
                "age de la plus ancienne requete");
}

// --- Invariant de topologie : bus_wait nul en SEPARE ------------------------
static void test_topologie_separee(void) {
  const bench_profile_t pa = make_profile("a", 11, 6, 8, 3, 1000);
  const bench_profile_t pb = make_profile("b", 22, 6, 8, 3, 1000);

  bench_bus_request_t sa[8];
  bench_bus_request_t sb[8];
  bench_arbiter_t bus_a;
  bench_arbiter_t bus_b;
  bench_arbiter_init(&bus_a, BENCH_ARB_FIFO, sa, 8, 0);
  bench_arbiter_init(&bus_b, BENCH_ARB_FIFO, sb, 8, 0);

  collect_t c = {{{0}}, 0};
  bench_concurrent_t e;
  bench_concurrent_init(&e, 5, collect, &c);
  bench_concurrent_add_producer(&e, &pa, 1, &bus_a);
  bench_concurrent_add_producer(&e, &pb, 1, &bus_b);
  EXPECT_EQ_INT(e.shared_topology, 0, "topologie SEPAREE detectee");
  (void)bench_concurrent_run(&e, 5000);

  EXPECT_EQ_INT(c.n, 12, "12 transactions (2 x 6)");
  for (uint32_t i = 0; i < c.n; i++) {
    EXPECT_EQ_INT((long long)c.items[i].bus_wait_ticks, 0,
                  "bus_wait_ticks == 0 en topologie separee");
  }
  // Les deux bus progressent bien dans le MEME temps virtuel.
  EXPECT(e.counters[0].tx_ok == 6 && e.counters[1].tx_ok == 6,
         "les deux producteurs ont progresse simultanement");
}

// --- Contention reelle en topologie PARTAGEE --------------------------------
static void test_topologie_partagee(void) {
  const bench_profile_t pa = make_profile("a", 11, 6, 8, 0, 100000);
  const bench_profile_t pb = make_profile("b", 22, 6, 8, 0, 100000);

  bench_bus_request_t s[16];
  bench_arbiter_t bus;
  bench_arbiter_init(&bus, BENCH_ARB_ROUND_ROBIN, s, 16, 0);

  collect_t c = {{{0}}, 0};
  bench_concurrent_t e;
  bench_concurrent_init(&e, 10, collect, &c);
  bench_concurrent_add_producer(&e, &pa, 1, &bus);
  bench_concurrent_add_producer(&e, &pb, 1, &bus);
  EXPECT_EQ_INT(e.shared_topology, 1, "topologie PARTAGEE detectee");
  (void)bench_concurrent_run(&e, 5000);

  EXPECT_EQ_INT(c.n, 12, "12 transactions");
  bench_ticks_t total_wait = 0;
  for (uint32_t i = 0; i < c.n; i++) {
    total_wait += c.items[i].bus_wait_ticks;
  }
  EXPECT(total_wait > 0, "la CONCURRENCE produit une attente de bus non nulle");

  // ORDRE GLOBAL strictement monotone, et sequences LOCALES continues.
  uint32_t local_next[2] = {0, 0};
  for (uint32_t i = 0; i < c.n; i++) {
    if (i > 0) {
      EXPECT(c.items[i].global_order > c.items[i - 1].global_order,
             "ordre global strictement monotone");
    }
    const uint8_t id = c.items[i].producer_id;
    EXPECT_EQ_INT(c.items[i].local_sequence, local_next[id],
                  "sequence locale continue malgre l'entrelacement");
    local_next[id]++;
  }
}

// --- Egalite STRICTE des modes isoles entre topologies ----------------------
static void run_isolated(int shared, collect_t *out) {
  const bench_profile_t pa = make_profile("a", 11, 8, 8, 2, 1000);
  const bench_profile_t pb = make_profile("b", 22, 8, 8, 2, 1000);
  static bench_bus_request_t s1[16];
  static bench_bus_request_t s2[16];
  static bench_arbiter_t bus1;
  static bench_arbiter_t bus2;
  bench_arbiter_init(&bus1, BENCH_ARB_FIFO, s1, 16, 0);
  bench_arbiter_init(&bus2, BENCH_ARB_FIFO, s2, 16, 0);

  bench_concurrent_t e;
  bench_concurrent_init(&e, 5, collect, out);
  // MEMES producteurs dans les deux cas ; le second est simplement DESACTIVE.
  bench_concurrent_add_producer(&e, &pa, 1, &bus1);
  bench_concurrent_add_producer(&e, &pb, 0, shared ? &bus1 : &bus2);
  (void)bench_concurrent_run(&e, 5000);
}

static void test_modes_isoles_identiques(void) {
  collect_t shared = {{{0}}, 0};
  collect_t separated = {{{0}}, 0};
  run_isolated(1, &shared);
  run_isolated(0, &separated);

  EXPECT_EQ_INT(shared.n, separated.n, "meme nombre de transactions");
  EXPECT_EQ_INT(shared.n, 8, "seul le producteur actif emet");
  int identical = (shared.n == separated.n);
  for (uint32_t i = 0; i < shared.n && identical; i++) {
    if (shared.items[i].producer_id != separated.items[i].producer_id ||
        shared.items[i].local_sequence != separated.items[i].local_sequence ||
        shared.items[i].global_order != separated.items[i].global_order ||
        shared.items[i].requested_at != separated.items[i].requested_at ||
        shared.items[i].released_at != separated.items[i].released_at ||
        shared.items[i].bus_wait_ticks != separated.items[i].bus_wait_ticks) {
      identical = 0;
    }
  }
  EXPECT(identical,
         "mode isole : resultats IDENTIQUES entre topologies (sans concurrence, "
         "la topologie ne doit rien changer)");
}

// --- Causes de timeout distinctes ------------------------------------------
static void test_causes_de_timeout(void) {
  // Budget minuscule + occupation longue => timeout d'ATTENTE DU BUS pour le
  // second producteur, pas un peripherique muet.
  const bench_profile_t pa = make_profile("a", 11, 3, 8, 0, 100000);
  const bench_profile_t pb = make_profile("b", 22, 3, 8, 0, 5);

  bench_bus_request_t s[16];
  bench_arbiter_t bus;
  bench_arbiter_init(&bus, BENCH_ARB_FIFO, s, 16, 0);

  collect_t c = {{{0}}, 0};
  bench_concurrent_t e;
  bench_concurrent_init(&e, 50, collect, &c);
  bench_concurrent_add_producer(&e, &pa, 1, &bus);
  bench_concurrent_add_producer(&e, &pb, 1, &bus);
  (void)bench_concurrent_run(&e, 5000);

  EXPECT(e.timeout_by_cause[BENCH_TIMEOUT_BUS_WAIT] > 0,
         "timeout impute a l'ATTENTE DU BUS");
  EXPECT_EQ_INT(e.timeout_by_cause[BENCH_TIMEOUT_PERIPHERAL_RESPONSE], 0,
                "aucun timeout impute a tort au peripherique");
}

// --- Saturation d'un seul producteur, puis des deux ------------------------
static void test_saturation(void) {
  // Un seul sature : il obtient le bus sans attente notable.
  {
    const bench_profile_t pa = make_profile("a", 11, 10, 8, 0, 100000);
    const bench_profile_t pb = make_profile("b", 22, 0, 8, 100, 100000);
    bench_bus_request_t s[16];
    bench_arbiter_t bus;
    bench_arbiter_init(&bus, BENCH_ARB_ROUND_ROBIN, s, 16, 0);
    collect_t c = {{{0}}, 0};
    bench_concurrent_t e;
    bench_concurrent_init(&e, 4, collect, &c);
    bench_concurrent_add_producer(&e, &pa, 1, &bus);
    bench_concurrent_add_producer(&e, &pb, 1, &bus);
    (void)bench_concurrent_run(&e, 5000);
    EXPECT_EQ_INT(c.n, 10, "seul le producteur sature emet");
    EXPECT_EQ_INT((long long)e.total_bus_wait[0], 0,
                  "aucune attente sans concurrent actif");
  }
  // Les deux saturent : les deux avancent, aucun n'est affame sous tourniquet.
  {
    const bench_profile_t pa = make_profile("a", 11, 10, 8, 0, 100000);
    const bench_profile_t pb = make_profile("b", 22, 10, 8, 0, 100000);
    bench_bus_request_t s[16];
    bench_arbiter_t bus;
    bench_arbiter_init(&bus, BENCH_ARB_ROUND_ROBIN, s, 16, 3);
    collect_t c = {{{0}}, 0};
    bench_concurrent_t e;
    bench_concurrent_init(&e, 4, collect, &c);
    bench_concurrent_add_producer(&e, &pa, 1, &bus);
    bench_concurrent_add_producer(&e, &pb, 1, &bus);
    (void)bench_concurrent_run(&e, 5000);
    EXPECT_EQ_INT(c.n, 20, "les deux producteurs progressent");
    EXPECT(e.counters[0].tx_ok > 0 && e.counters[1].tx_ok > 0,
           "aucun producteur bloque sous tourniquet");
  }
}

// --- Determinisme de l'entrelacement ---------------------------------------
static void test_entrelacement_deterministe(void) {
  collect_t a = {{{0}}, 0};
  collect_t b = {{{0}}, 0};
  for (int pass = 0; pass < 2; pass++) {
    const bench_profile_t pa = make_profile("a", 11, 8, 8, 1, 100000);
    const bench_profile_t pb = make_profile("b", 22, 8, 8, 2, 100000);
    static bench_bus_request_t s[16];
    static bench_arbiter_t bus;
    bench_arbiter_init(&bus, BENCH_ARB_ROUND_ROBIN, s, 16, 0);
    bench_concurrent_t e;
    bench_concurrent_init(&e, 3, collect, pass == 0 ? &a : &b);
    bench_concurrent_add_producer(&e, &pa, 1, &bus);
    bench_concurrent_add_producer(&e, &pb, 1, &bus);
    (void)bench_concurrent_run(&e, 5000);
  }
  EXPECT_EQ_INT(a.n, b.n, "meme nombre de transactions");
  int same = (a.n == b.n);
  for (uint32_t i = 0; i < a.n && same; i++) {
    if (a.items[i].producer_id != b.items[i].producer_id ||
        a.items[i].global_order != b.items[i].global_order ||
        a.items[i].granted_at != b.items[i].granted_at) {
      same = 0;
    }
  }
  EXPECT(same, "entrelacement EXACTEMENT reproductible (temps virtuel)");
}

void run_concurrent_tests(void) {
  test_atomicite();
  test_tie_break_simultane();
  test_fifo_stable();
  test_round_robin();
  test_famine();
  test_file_bornee();
  test_topologie_separee();
  test_topologie_partagee();
  test_modes_isoles_identiques();
  test_causes_de_timeout();
  test_saturation();
  test_entrelacement_deterministe();
}

# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Analyse de la telemetrie de latence (lot B4.1).

Couvre la methode de quantile FIGEE (cas limites compris), l'arithmetique
wrap-safe, la reconciliation sans double comptage, la distinction des deux
natures de perte, la derivation d'histogramme, et le GOLDEN INTER-LANGAGE : le
flux de reference est produit par l'encodeur C et doit etre decode a l'identique
ici."""

import json
import math
import unittest
from pathlib import Path

from measurement.analysis import histogram as H
from measurement.analysis import stats as S
from measurement.analysis import telemetry as T

_ROOT = Path(__file__).resolve().parents[1]
_GOLDEN = _ROOT / "golden" / "telemetry"


def _load_golden_stream() -> bytes:
    return bytes.fromhex("".join((_GOLDEN / "stream.hex").read_text().split()))


def _load_expected() -> dict:
    return json.loads((_GOLDEN / "expected.json").read_text(encoding="utf-8"))


class QuantileTest(unittest.TestCase):
    def test_methode_archivee(self):
        self.assertEqual(S.QUANTILE_METHOD, "nearest-rank-inclusive-v1")

    def test_vecteurs_golden(self):
        # P(p) = x[ceil(p*n)], indexe a partir de 1, sur valeurs triees.
        v = [10, 20, 30, 40]
        self.assertEqual(S.quantile(v, 50, 100), 20)  # ceil(2.0) = 2
        self.assertEqual(S.quantile(v, 95, 100), 40)  # ceil(3.8) = 4
        self.assertEqual(S.quantile(v, 99, 100), 40)  # ceil(3.96) = 4
        self.assertEqual(S.quantile(v, 25, 100), 10)  # ceil(1.0) = 1
        self.assertEqual(S.quantile(v, 100, 100), 40)

    def test_rang_exactement_sur_une_frontiere(self):
        """p*n entier : le rang ne doit PAS glisser d'un cran (arithmetique entiere)."""
        v = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        self.assertEqual(S.quantile(v, 50, 100), 5)  # ceil(5.0) = 5, pas 6
        self.assertEqual(S.quantile(v, 10, 100), 1)  # ceil(1.0) = 1
        self.assertEqual(S.quantile(v, 90, 100), 9)  # ceil(9.0) = 9
        v100 = list(range(1, 101))
        self.assertEqual(S.quantile(v100, 95, 100), 95)
        self.assertEqual(S.quantile(v100, 99, 100), 99)

    def test_cas_limites(self):
        self.assertIsNone(S.quantile([], 50, 100), "n=0 : quantile indisponible")
        self.assertEqual(S.quantile([7], 50, 100), 7)
        self.assertEqual(S.quantile([7], 99, 100), 7)
        self.assertEqual(S.quantile([3, 9], 50, 100), 3)  # ceil(1.0) = 1
        self.assertEqual(S.quantile([3, 9], 95, 100), 9)  # ceil(1.9) = 2
        self.assertEqual(S.quantile([5, 5, 5], 99, 100), 5, "valeurs toutes egales")
        v_odd = [1, 2, 3, 4, 5]
        self.assertEqual(S.quantile(v_odd, 50, 100), 3)  # ceil(2.5) = 3

    def test_p_hors_bornes(self):
        for num, den in ((0, 100), (101, 100), (-1, 100), (1, 0)):
            with self.subTest(p=(num, den)):
                with self.assertRaises(ValueError):
                    S.quantile([1, 2, 3], num, den)


class StatsTest(unittest.TestCase):
    def test_population_vide(self):
        st = S.latency_stats([])
        self.assertEqual(st["valid_sample_count"], 0)
        for key in ("min", "max", "mean", "p50", "p95", "p99"):
            self.assertIsNone(st[key], f"{key} ne doit pas etre fabrique")

    def test_stats_completes(self):
        st = S.latency_stats([40, 10, 30, 20])  # non triees en entree
        self.assertEqual(st["min"], 10)
        self.assertEqual(st["max"], 40)
        self.assertEqual(st["mean"], 25.0)
        self.assertEqual(st["p50"], 20)
        self.assertEqual(st["p99"], 40)
        # Ecart-type de POPULATION : sqrt(500/4) = sqrt(125).
        self.assertAlmostEqual(st["latency_stddev_population"], math.sqrt(125), places=12)
        self.assertEqual(st["dispersion_metric"], "latency_stddev_population")
        self.assertNotIn("jitter", st, "le terme generique 'jitter' est proscrit")

    def test_elapsed_wrap_safe(self):
        self.assertEqual(S.elapsed_wrap_safe(100, 150), 50)
        mask = (1 << 64) - 1
        # Rebouclage du compteur : une soustraction ordinaire donnerait un negatif.
        self.assertEqual(S.elapsed_wrap_safe(mask - 5, 4), 10)
        self.assertEqual(S.elapsed_wrap_safe(mask, 0), 1)

    def test_reconciliation(self):
        ok = S.reconcile(
            {
                "issued": 8,
                "ok": 4,
                "timeout": 1,
                "rejected": 1,
                "unpaired": 0,
                "duplicate": 0,
                "out_of_order": 0,
                "producer_drop": 2,
            }
        )
        self.assertTrue(ok["balanced"])
        self.assertEqual(ok["difference"], 0)

        bad = S.reconcile({"issued": 10, "ok": 4, "producer_drop": 2})
        self.assertFalse(bad["balanced"], "un ecart doit etre detecte")
        self.assertEqual(bad["difference"], 4)

    def test_transport_gap_hors_identite(self):
        """transport_gap n'est PAS une transaction executee : hors reconciliation."""
        counts = {
            "issued": 4,
            "ok": 4,
            "timeout": 0,
            "rejected": 0,
            "unpaired": 0,
            "duplicate": 0,
            "out_of_order": 0,
            "producer_drop": 0,
            "transport_gap": 99,
        }
        self.assertTrue(S.reconcile(counts)["balanced"])

    def test_completude(self):
        self.assertEqual(S.series_completeness(0, 0), "complete")
        self.assertEqual(S.series_completeness(1, 0), "incomplete")
        self.assertEqual(S.series_completeness(0, 1), "incomplete")


class HistogramTest(unittest.TestCase):
    def test_convention_de_classe(self):
        h = H.derive([9, 10, 19, 20, 39, 40, 100], [10, 20, 30, 40])
        self.assertEqual(h["underflow"], 1, "9 < 10")
        self.assertEqual(h["overflow"], 2, "40 et 100 >= borne haute")
        self.assertEqual(h["bin_counts"], [2, 1, 1])
        self.assertEqual(h["sample_count"], 7)
        self.assertEqual(
            sum(h["bin_counts"]) + h["underflow"] + h["overflow"], h["sample_count"]
        )

    def test_bornes_invalides(self):
        with self.assertRaises(ValueError):
            H.derive([1], [10])
        with self.assertRaises(ValueError):
            H.derive([1], [10, 10, 20])

    def test_comparaison(self):
        tooling = H.derive([10, 20], [0, 15, 25])
        self.assertFalse(H.compare(None, tooling)["compared"], "desactive != divergence")
        self.assertTrue(H.compare(dict(tooling), tooling)["match"])
        divergent = dict(tooling)
        divergent["bin_counts"] = [9, 9]
        result = H.compare(divergent, tooling)
        self.assertFalse(result["match"])
        self.assertIn("bin_counts", result["differences"])


class GoldenStreamTest(unittest.TestCase):
    """Le flux est produit par l'encodeur C : ce test verrouille les deux formats."""

    def setUp(self):
        self.parsed = T.parse_stream(_load_golden_stream())
        self.expected = _load_expected()

    def test_entete(self):
        self.assertEqual(self.parsed["header"], self.expected["header"])

    def test_trames_et_transport(self):
        self.assertEqual(self.parsed["frames_decoded"], self.expected["frames_decoded"])
        self.assertEqual(self.parsed["transport_gap"], self.expected["transport_gap"])
        self.assertEqual(self.parsed["frames_resync"], self.expected["frames_resync"])

    def test_marqueurs_de_lacune(self):
        self.assertEqual(self.parsed["gaps"], self.expected["gaps"])

    def test_echantillons(self):
        ids = [s["sequence_id"] for s in self.parsed["samples"]]
        statuses = [T.STATUS_NAMES[s["status"]] for s in self.parsed["samples"]]
        self.assertEqual(ids, self.expected["sample_sequence_ids"])
        self.assertEqual(statuses, self.expected["sample_statuses"])

    def test_bilan(self):
        self.assertEqual(self.parsed["summary"], self.expected["summary"])

    def test_latences_et_statistiques(self):
        lat = T.valid_latencies(self.parsed)
        self.assertEqual(lat, self.expected["valid_latencies"])
        st = S.latency_stats(lat)
        exp = self.expected["stats"]
        for key in ("valid_sample_count", "min", "max", "mean", "p50", "p95", "p99"):
            self.assertEqual(st[key], exp[key], key)
        self.assertAlmostEqual(
            st["latency_stddev_population"], exp["latency_stddev_population"], places=12
        )

    def test_bloc_analyse(self):
        block = T.build_analysis(self.parsed)
        self.assertEqual(block["series_completeness"], self.expected["series_completeness"])
        self.assertTrue(block["reconciliation"]["balanced"])
        self.assertEqual(block["losses"]["producer_drop"], 2)
        self.assertEqual(block["losses"]["transport_gap"], 0)
        self.assertTrue(block["histogram"]["comparison"]["match"])
        self.assertEqual(block["timeouts"]["timeout_count"], 1)

    def test_bloc_analyse_conforme_au_schema(self):
        """Le bloc produit doit etre CONTRACTUEL, pas seulement plausible."""
        from measurement.analysis import schema as home

        block = T.build_analysis(self.parsed)
        home.validate(block, home.load_schema("latency-analysis.schema.json"))
        home.validate(
            block["histogram"]["tooling"],
            home.load_schema("latency-histogram.schema.json"),
        )

    def test_serie_normalisee(self):
        rows = T.to_series_rows(self.parsed)
        self.assertEqual(len(rows), 6)
        self.assertEqual(len(T.CSV_COLUMNS), 7)
        # La latence est une colonne DERIVEE, recalculee wrap-safe.
        self.assertEqual(rows[0][3], 10)
        self.assertEqual(rows[0][5], "spi-shared")
        self.assertEqual(rows[0][6], "module-only")


class LossBiasTest(unittest.TestCase):
    """Les pertes ne sont pas aleatoires : elles frappent les latences hautes."""

    def test_p99_non_eligible_sur_serie_lacunaire(self):
        # Les 3 transactions les plus lentes ont ete perdues au tampon : les
        # survivants donnent un P99 flatteur.
        survivors = [10, 11, 12, 13, 14, 15, 16]
        parsed = {
            "header": {"variant": "v", "mode": "m"},
            "samples": [
                {"sequence_id": i, "t_start": 0, "t_end": v, "status": 0, "flags": 0}
                for i, v in enumerate(survivors)
            ],
            "gaps": [{"lost_count": 3, "after_sequence_id": 6}],
            "summary": {
                "issued": 10,
                "ok": 7,
                "timeout": 0,
                "rejected": 0,
                "unpaired": 0,
                "duplicate": 0,
                "out_of_order": 0,
                "producer_drop": 3,
                "timeout_budget_ticks": 1000,
            },
            "device_histogram": None,
            "transport_gap": 0,
        }
        block = T.build_analysis(parsed)
        self.assertEqual(block["latency"]["p99"], 16, "P99 des survivants, flatteur")
        # L'outillage doit REFUSER d'en faire un verdict, pas le publier tel quel.
        self.assertEqual(block["series_completeness"], "incomplete")
        self.assertFalse(
            block["quantiles_verdict_eligible"],
            "une serie lacunaire ne peut pas fonder un verdict sur P95/P99",
        )
        self.assertTrue(block["reconciliation"]["balanced"], "10 = 7 ok + 3 perdus")

    def test_serie_complete_eligible(self):
        parsed = {
            "header": {"variant": "v", "mode": "m"},
            "samples": [
                {"sequence_id": i, "t_start": 0, "t_end": 10 + i, "status": 0, "flags": 0}
                for i in range(5)
            ],
            "gaps": [],
            "summary": {
                "issued": 5,
                "ok": 5,
                "timeout": 0,
                "rejected": 0,
                "unpaired": 0,
                "duplicate": 0,
                "out_of_order": 0,
                "producer_drop": 0,
                "timeout_budget_ticks": 1000,
            },
            "device_histogram": None,
            "transport_gap": 0,
        }
        block = T.build_analysis(parsed)
        self.assertEqual(block["series_completeness"], "complete")
        self.assertTrue(block["quantiles_verdict_eligible"])


class StreamRobustnessTest(unittest.TestCase):
    def test_transport_gap_detecte_par_sequence(self):
        """Une trame absente laisse un trou dans la sequence : transport_gap."""
        data = _load_golden_stream()
        parsed_full = T.parse_stream(data)
        self.assertEqual(parsed_full["transport_gap"], 0)

        # Retire physiquement la 2e trame du flux : la sequence saute.
        first_len = 13 + int.from_bytes(data[7:9], "big")
        second_len = 13 + int.from_bytes(data[first_len + 7 : first_len + 9], "big")
        truncated = data[:first_len] + data[first_len + second_len :]
        parsed = T.parse_stream(truncated)
        self.assertEqual(parsed["transport_gap"], 1, "trame manquante detectee")
        self.assertLess(parsed["frames_decoded"], parsed_full["frames_decoded"])

    def test_flux_vide_rejete(self):
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(b"")

    def test_octets_parasites_ignores(self):
        """Des octets hors trame ne doivent pas produire de fausses trames."""
        data = _load_golden_stream()
        parsed = T.parse_stream(b"\x00\x01\x02" + data)
        self.assertEqual(parsed["frames_decoded"], 10)


if __name__ == "__main__":
    unittest.main()

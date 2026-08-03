# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Analyse de la telemetrie de latence (lot B4.1, format de flux v2).

Couvre la methode de quantile FIGEE (cas limites compris), l'arithmetique
wrap-safe a largeur DECLAREE, la reconciliation sans double comptage, la
distinction des deux natures de perte, la cloture autoritaire du flux, la
robustesse du parseur (tests NEGATIFS : trames tronquees, chaines invalides,
messages dupliques ou mal places) et le GOLDEN INTER-LANGAGE."""

import json
import math
import unittest
import zlib
from pathlib import Path

from measurement.analysis import histogram as H
from measurement.analysis import schema as home
from measurement.analysis import stats as S
from measurement.analysis import telemetry as T

_ROOT = Path(__file__).resolve().parents[1]
_GOLDEN = _ROOT / "golden" / "telemetry"


def _load_golden_stream() -> bytes:
    return bytes.fromhex("".join((_GOLDEN / "stream.hex").read_text().split()))


def _load_expected() -> dict:
    return json.loads((_GOLDEN / "expected.json").read_text(encoding="utf-8"))


def _frame(seq: int, payload: bytes) -> bytes:
    """Fabrique une trame B2 valide (CRC correct) autour d'un payload arbitraire."""
    head = bytes([0xC5, 0xB5, 1]) + seq.to_bytes(4, "big") + len(payload).to_bytes(2, "big")
    body = head + payload
    return body + (zlib.crc32(body) & 0xFFFFFFFF).to_bytes(4, "big")


def _header_payload(
    profile=b"p", variant=b"v", mode=b"m", width=64, version=T.STREAM_VERSION, tick_hz=1000
) -> bytes:
    p = bytes([T.TM_HEADER]) + version.to_bytes(2, "big")
    p += bytes([1, width, 0]) + tick_hz.to_bytes(8, "big") + (4).to_bytes(4, "big")
    p += bytes([0]) + (0).to_bytes(2, "big")
    for s in (profile, variant, mode):
        p += bytes([len(s)]) + s
    return p


def _footer_payload(last_seq: int, attempted: int) -> bytes:
    return (
        bytes([T.TM_FOOTER])
        + last_seq.to_bytes(4, "big")
        + attempted.to_bytes(4, "big")
        + (0).to_bytes(4, "big")
        + (0).to_bytes(4, "big")
        + (0).to_bytes(4, "big")
    )


class QuantileTest(unittest.TestCase):
    def test_methode_archivee(self):
        self.assertEqual(S.QUANTILE_METHOD, "nearest-rank-inclusive-v1")

    def test_vecteurs_golden(self):
        v = [10, 20, 30, 40]
        self.assertEqual(S.quantile(v, 50, 100), 20)  # ceil(2.0) = 2
        self.assertEqual(S.quantile(v, 95, 100), 40)  # ceil(3.8) = 4
        self.assertEqual(S.quantile(v, 99, 100), 40)
        self.assertEqual(S.quantile(v, 25, 100), 10)
        self.assertEqual(S.quantile(v, 100, 100), 40)

    def test_rang_exactement_sur_une_frontiere(self):
        v = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        self.assertEqual(S.quantile(v, 50, 100), 5, "ceil(5.0) = 5, pas 6")
        self.assertEqual(S.quantile(v, 90, 100), 9)
        v100 = list(range(1, 101))
        self.assertEqual(S.quantile(v100, 95, 100), 95)
        self.assertEqual(S.quantile(v100, 99, 100), 99)

    def test_cas_limites(self):
        self.assertIsNone(S.quantile([], 50, 100), "n=0 : indisponible")
        self.assertEqual(S.quantile([7], 99, 100), 7)
        self.assertEqual(S.quantile([3, 9], 50, 100), 3)
        self.assertEqual(S.quantile([3, 9], 95, 100), 9)
        self.assertEqual(S.quantile([5, 5, 5], 99, 100), 5)
        self.assertEqual(S.quantile([1, 2, 3, 4, 5], 50, 100), 3)

    def test_p_hors_bornes(self):
        for num, den in ((0, 100), (101, 100), (-1, 100), (1, 0)):
            with self.subTest(p=(num, den)), self.assertRaises(ValueError):
                S.quantile([1, 2, 3], num, den)


class StatsTest(unittest.TestCase):
    def test_population_vide(self):
        st = S.latency_stats([])
        self.assertEqual(st["valid_sample_count"], 0)
        for key in ("min", "max", "mean", "p50", "p95", "p99", "mean_numerator"):
            self.assertIsNone(st[key], f"{key} ne doit pas etre fabrique")

    def test_valeurs_exactes_rationnelles(self):
        """Moyenne et variance sont archivees en ENTIERS exacts, pas en flottants."""
        st = S.latency_stats([40, 10, 30, 20])
        self.assertEqual(st["mean_numerator"], 100)
        self.assertEqual(st["mean_denominator"], 4)
        self.assertEqual(st["variance_numerator"], 2000)
        self.assertEqual(st["variance_denominator"], 16)
        # Les vues flottantes derivent exactement des rationnels.
        self.assertEqual(st["mean"], st["mean_numerator"] / st["mean_denominator"])
        self.assertAlmostEqual(st["latency_stddev_population"], math.sqrt(125), places=12)
        # ... et sont explicitement declarees NON autoritaires.
        self.assertIn("mean", st["float_views_non_authoritative"])
        self.assertIn("latency_stddev_population", st["float_views_non_authoritative"])
        self.assertIn("p99", st["authoritative_fields"])
        self.assertNotIn("mean", st["authoritative_fields"])
        self.assertNotIn("jitter", st, "le terme generique 'jitter' est proscrit")

    def test_wrap_safe_par_largeur(self):
        self.assertEqual(S.elapsed_wrap_safe(100, 150), 50)
        self.assertEqual(S.elapsed_wrap_safe((1 << 64) - 6, 4, 64), 10)
        # Compteur 32 bits : un masque 64 bits donnerait une valeur absurde.
        self.assertEqual(S.elapsed_wrap_safe((1 << 32) - 6, 4, 32), 10)
        self.assertEqual(S.elapsed_wrap_safe(250, 4, 8), 10)
        self.assertEqual(S.elapsed_wrap_safe(65530, 4, 16), 10)

    def test_largeur_non_supportee_refusee(self):
        for width in (0, 7, 24, 48, 128):
            with self.subTest(width=width), self.assertRaises(ValueError):
                S.elapsed_wrap_safe(0, 1, width)

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
        bad = S.reconcile({"issued": 10, "ok": 4, "producer_drop": 2})
        self.assertFalse(bad["balanced"])
        self.assertEqual(bad["difference"], 4)

    def test_transport_gap_hors_identite(self):
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


class SequenceAnalysisTest(unittest.TestCase):
    def test_sequence_nominale(self):
        r = T.analyse_sequence([0, 1, 2, 3], 3)
        self.assertEqual(r["expected"], 4)
        self.assertEqual(r["missing_total"], 0)

    def test_lacune(self):
        r = T.analyse_sequence([0, 1, 4], 4)
        self.assertEqual(r["gap"], 1)
        self.assertEqual(r["missing_total"], 2, "sequences 2 et 3 manquantes")

    def test_doublon(self):
        r = T.analyse_sequence([0, 1, 1, 2], 2)
        self.assertEqual(r["duplicate"], 1)
        self.assertEqual(r["missing_total"], 0)

    def test_hors_ordre(self):
        r = T.analyse_sequence([0, 2, 1, 3], 3)
        self.assertEqual(r["out_of_order"], 1, "retour en arriere, pas une lacune geante")

    def test_wrap_uint32(self):
        m = (1 << 32) - 1
        r = T.analyse_sequence([m - 1, m, 0, 1], 1)
        self.assertEqual(r["expected"], 4, "comparaison MODULAIRE : aucune lacune")
        self.assertEqual(r["missing_total"], 0)
        self.assertEqual(r["wrap"], 1)

    def test_pertes_de_fin_detectees_par_la_cloture(self):
        r = T.analyse_sequence([0, 1, 2], 5)
        self.assertEqual(r["trailing_missing"], 3, "3 dernieres trames absentes")
        self.assertEqual(r["missing_total"], 3)


class HistogramTest(unittest.TestCase):
    def test_convention_de_classe(self):
        h = H.derive([9, 10, 19, 20, 39, 40, 100], [10, 20, 30, 40])
        self.assertEqual(h["underflow"], 1)
        self.assertEqual(h["overflow"], 2)
        self.assertEqual(h["bin_counts"], [2, 1, 1])
        self.assertEqual(
            sum(h["bin_counts"]) + h["underflow"] + h["overflow"], h["sample_count"]
        )

    def test_bornes_invalides(self):
        for edges in ([10], [10, 10, 20]):
            with self.subTest(edges=edges), self.assertRaises(ValueError):
                H.derive([1], edges)

    def test_comparaison(self):
        tooling = H.derive([10, 20], [0, 15, 25])
        self.assertFalse(H.compare(None, tooling)["compared"])
        self.assertTrue(H.compare(dict(tooling), tooling)["match"])
        divergent = dict(tooling)
        divergent["bin_counts"] = [9, 9]
        self.assertIn("bin_counts", H.compare(divergent, tooling)["differences"])

    def test_sature_non_reconciliable(self):
        tooling = H.derive([10, 20], [0, 15, 25])
        device = dict(tooling)
        device["saturated"] = True
        result = H.compare(device, tooling)
        self.assertFalse(result["compared"], "un histogramme sature n'est pas comparable")
        self.assertTrue(result["saturated"])


class GoldenStreamTest(unittest.TestCase):
    """Le flux est produit par l'encodeur C : ce test verrouille les deux formats."""

    def setUp(self):
        self.parsed = T.parse_stream(_load_golden_stream())
        self.expected = _load_expected()

    def test_entete(self):
        self.assertEqual(self.parsed["header"], self.expected["header"])

    def test_cloture(self):
        self.assertEqual(self.parsed["footer"], self.expected["footer"])
        self.assertTrue(self.parsed["footer_consistent"])
        self.assertEqual(
            self.parsed["stream_completeness"], self.expected["stream_completeness"]
        )

    def test_sequence_et_trames(self):
        self.assertEqual(self.parsed["frames_decoded"], self.expected["frames_decoded"])
        self.assertEqual(self.parsed["transport_gap"], self.expected["transport_gap"])
        self.assertEqual(self.parsed["unknown_messages"], 0)
        self.assertEqual(self.parsed["sequence"], self.expected["sequence"])

    def test_ordre_des_messages(self):
        """La lacune doit apparaitre APRES les echantillons qui la precedent."""
        self.assertEqual(self.parsed["gaps"], self.expected["gaps"])
        ids = [s["sequence_id"] for s in self.parsed["samples"]]
        self.assertEqual(ids, self.expected["sample_sequence_ids"])
        # Les quatre premiers echantillons precedent la lacune (seq 0..3),
        # les suivants la suivent (seq 6, 7).
        self.assertEqual(ids[:4], [0, 1, 2, 3])
        self.assertEqual(self.parsed["gaps"][0]["after_sequence_id"], 3)

    def test_bilan_et_statistiques(self):
        self.assertEqual(self.parsed["summary"], self.expected["summary"])
        lat = T.valid_latencies(self.parsed)
        self.assertEqual(lat, self.expected["valid_latencies"])
        st = S.latency_stats(lat)
        for key, value in self.expected["stats"].items():
            if key == "latency_stddev_population":
                self.assertAlmostEqual(st[key], value, places=12)
            else:
                self.assertEqual(st[key], value, key)

    def test_bloc_analyse(self):
        block = T.build_analysis(self.parsed)
        self.assertEqual(block["series_completeness"], "incomplete")
        self.assertEqual(block["stream_completeness"], "complete")
        self.assertFalse(
            block["quantiles_verdict_eligible"],
            "flux complet mais SERIE lacunaire : pas de verdict",
        )
        self.assertTrue(block["reconciliation"]["balanced"])
        self.assertEqual(block["losses"]["producer_drop"], 2)
        self.assertEqual(block["losses"]["transport_gap"], 0)
        self.assertTrue(block["clock"]["physical_time_available"])

    def test_bloc_analyse_conforme_au_schema(self):
        block = T.build_analysis(self.parsed)
        home.validate(block, home.load_schema("latency-analysis.schema.json"))
        home.validate(
            block["histogram"]["tooling"], home.load_schema("latency-histogram.schema.json")
        )


class ParserRobustnessTest(unittest.TestCase):
    """Tests NEGATIFS : une archive invalide doit produire une erreur CONTROLEE."""

    def test_flux_vide(self):
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(b"")

    def test_header_absent(self):
        stream = _frame(0, bytes([T.TM_SAMPLE]) + b"\x00" * 22)
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_header_non_premier(self):
        stream = _frame(0, bytes([T.TM_GAP]) + b"\x00" * 8) + _frame(1, _header_payload())
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_header_duplique(self):
        stream = _frame(0, _header_payload()) + _frame(1, _header_payload())
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_header_tronque(self):
        stream = _frame(0, bytes([T.TM_HEADER]) + b"\x00" * 5)
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_header_chaine_trop_longue(self):
        payload = _header_payload(profile=b"x" * (T.STR_MAX + 1))
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(_frame(0, payload))

    def test_header_chaine_non_imprimable(self):
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(_frame(0, _header_payload(profile=b"a\x01b")))

    def test_header_version_inattendue(self):
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(_frame(0, _header_payload(version=99)))

    def test_header_largeur_de_tick_invalide(self):
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(_frame(0, _header_payload(width=24)))

    def test_header_octets_excedentaires(self):
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(_frame(0, _header_payload() + b"\xff\xff"))

    def test_sample_trop_court_ou_trop_long(self):
        for extra in (b"\x00" * 21, b"\x00" * 23):
            stream = _frame(0, _header_payload()) + _frame(1, bytes([T.TM_SAMPLE]) + extra)
            with self.subTest(size=len(extra)), self.assertRaises(T.TelemetryError):
                T.parse_stream(stream)

    def test_summary_duplique(self):
        body = bytes([T.TM_SUMMARY]) + b"\x00" * 40
        stream = _frame(0, _header_payload()) + _frame(1, body) + _frame(2, body)
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_sample_apres_le_bilan(self):
        summary = bytes([T.TM_SUMMARY]) + b"\x00" * 40
        sample = bytes([T.TM_SAMPLE]) + b"\x00" * 22
        stream = _frame(0, _header_payload()) + _frame(1, summary) + _frame(2, sample)
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_histogramme_non_annonce(self):
        hist = bytes([T.TM_HISTOGRAM]) + (1).to_bytes(4, "big") * 5 + b"\x00"
        hist += (0).to_bytes(8, "big") + (10).to_bytes(8, "big") + (1).to_bytes(4, "big")
        stream = _frame(0, _header_payload()) + _frame(1, hist)
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_histogramme_bin_count_excessif(self):
        body = bytes([T.TM_HISTOGRAM]) + (1).to_bytes(4, "big")
        body += (999).to_bytes(4, "big") + b"\x00" * 13
        stream = _frame(0, _header_payload()) + _frame(1, body)
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_histogramme_tronque(self):
        body = bytes([T.TM_HISTOGRAM]) + (1).to_bytes(4, "big")
        body += (2).to_bytes(4, "big") + b"\x00" * 13  # annonce 2 classes, rien apres
        stream = _frame(0, _header_payload()) + _frame(1, body)
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_type_inconnu_compte(self):
        stream = _frame(0, _header_payload()) + _frame(1, bytes([0x7F]) + b"\x00" * 4)
        stream += _frame(2, _footer_payload(2, 3))
        parsed = T.parse_stream(stream)
        self.assertEqual(parsed["unknown_messages"], 1, "type inconnu compte, pas devine")

    def test_octets_parasites_ignores(self):
        parsed = T.parse_stream(b"\x00\x01\x02" + _load_golden_stream())
        self.assertEqual(parsed["frames_decoded"], 11)


class StreamClosureTest(unittest.TestCase):
    """Point 2 : sans cloture, une perte des DERNIERES trames est indetectable."""

    def test_cloture_absente_rend_incomplete(self):
        stream = _frame(0, _header_payload()) + _frame(1, bytes([T.TM_SUMMARY]) + b"\x00" * 40)
        parsed = T.parse_stream(stream)
        self.assertFalse(parsed["footer_present"])
        self.assertEqual(parsed["stream_completeness"], "incomplete")
        block = T.build_analysis(parsed)
        self.assertFalse(
            block["quantiles_verdict_eligible"],
            "sans cloture, aucun verdict sur les quantiles",
        )

    def test_derniere_trame_perdue_detectee(self):
        """La cloture est recue mais des trames anterieures manquent."""
        stream = _frame(0, _header_payload()) + _frame(5, _footer_payload(5, 6))
        parsed = T.parse_stream(stream)
        self.assertTrue(parsed["footer_present"])
        self.assertEqual(parsed["transport_gap"], 4, "seq 1..4 absentes")
        self.assertEqual(parsed["stream_completeness"], "incomplete")

    def test_cloture_incoherente(self):
        # frames_attempted ne correspond pas a last_stream_seq + 1.
        stream = _frame(0, _header_payload()) + _frame(1, _footer_payload(1, 99))
        parsed = T.parse_stream(stream)
        self.assertFalse(parsed["footer_consistent"])
        self.assertEqual(parsed["stream_completeness"], "incomplete")

    def test_cloture_perdue_capture_incomplete(self):
        """Le footer lui-meme peut etre perdu : la capture est alors incomplete."""
        full = _load_golden_stream()
        parsed_full = T.parse_stream(full)
        footer_len = 13 + T._FOOTER_BODY
        parsed = T.parse_stream(full[:-footer_len])
        self.assertTrue(parsed_full["footer_present"])
        self.assertFalse(parsed["footer_present"])
        self.assertEqual(parsed["stream_completeness"], "incomplete")


class ClockTest(unittest.TestCase):
    def test_tick_hz_nul_interdit_la_conversion(self):
        stream = _frame(0, _header_payload(tick_hz=0)) + _frame(1, _footer_payload(1, 2))
        parsed = T.parse_stream(stream)
        self.assertFalse(parsed["header"]["physical_time_available"])
        block = T.build_analysis(parsed)
        self.assertFalse(
            block["clock"]["physical_time_available"],
            "sans frequence, aucun verdict en secondes ou microsecondes",
        )

    def test_largeur_32_bits_appliquee(self):
        """Une latence a cheval sur le wrap d'un compteur 32 bits."""
        start = (1 << 32) - 5
        sample = (
            bytes([T.TM_SAMPLE])
            + (0).to_bytes(4, "big")
            + start.to_bytes(8, "big")
            + (5).to_bytes(8, "big")
            + bytes([0, 0])
        )
        stream = _frame(0, _header_payload(width=32)) + _frame(1, sample)
        stream += _frame(2, _footer_payload(2, 3))
        parsed = T.parse_stream(stream)
        self.assertEqual(T.valid_latencies(parsed), [10], "wrap 32 bits correctement traite")


class LossBiasTest(unittest.TestCase):
    """Les pertes ne sont pas aleatoires : elles frappent les latences hautes."""

    def _synthetic(self, producer_drop: int, ok: int, latencies: list[int]) -> dict:
        return {
            "header": {
                "variant": "v",
                "mode": "m",
                "tick_width_bits": 64,
                "tick_hz": 1000,
                "physical_time_available": True,
            },
            "samples": [
                {"sequence_id": i, "t_start": 0, "t_end": v, "status": 0, "flags": 0}
                for i, v in enumerate(latencies)
            ],
            "gaps": [{"lost_count": producer_drop, "after_sequence_id": ok - 1}]
            if producer_drop
            else [],
            "summary": {
                "issued": ok + producer_drop,
                "ok": ok,
                "timeout": 0,
                "rejected": 0,
                "unpaired": 0,
                "duplicate": 0,
                "out_of_order": 0,
                "producer_drop": producer_drop,
                "timeout_budget_ticks": 1000,
            },
            "device_histogram": None,
            "transport_gap": 0,
            "stream_completeness": "complete",
            "footer_present": True,
            "sequence": {},
        }

    def test_p99_non_eligible_sur_serie_lacunaire(self):
        survivors = [10, 11, 12, 13, 14, 15, 16]
        block = T.build_analysis(self._synthetic(3, 7, survivors))
        self.assertEqual(block["latency"]["p99"], 16, "P99 des survivants, flatteur")
        self.assertEqual(block["series_completeness"], "incomplete")
        self.assertFalse(block["quantiles_verdict_eligible"])
        self.assertTrue(block["reconciliation"]["balanced"], "10 = 7 ok + 3 perdus")

    def test_serie_complete_eligible(self):
        block = T.build_analysis(self._synthetic(0, 5, [10, 11, 12, 13, 14]))
        self.assertEqual(block["series_completeness"], "complete")
        self.assertTrue(block["quantiles_verdict_eligible"])

    def test_histogramme_sature_retire_l_eligibilite(self):
        parsed = self._synthetic(0, 4, [10, 20, 30, 40])
        parsed["device_histogram"] = {
            "version": 1,
            "bin_edges": [0, 50],
            "bin_counts": [4],
            "underflow": 0,
            "overflow": 0,
            "sample_count": 4,
            "saturated": True,
        }
        block = T.build_analysis(parsed)
        self.assertFalse(
            block["quantiles_verdict_eligible"],
            "histogramme sature : plus aucun verdict",
        )


if __name__ == "__main__":
    unittest.main()

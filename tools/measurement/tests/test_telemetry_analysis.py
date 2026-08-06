# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Analyse de la telemetrie (lots B4.1 et B4.2, format de flux v4).

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
    profile=b"p",
    variant=b"v",
    mode=b"m",
    width=64,
    version=T.STREAM_VERSION,
    tick_hz=1000,
    topology=0,
    producers=1,
) -> bytes:
    p = bytes([T.TM_HEADER]) + version.to_bytes(2, "big")
    p += bytes([1, width, 0]) + tick_hz.to_bytes(8, "big") + (4).to_bytes(4, "big")
    p += bytes([0]) + (0).to_bytes(2, "big")
    # v4 : topologie, politique d'arbitrage, nombre de producteurs, seuil.
    p += bytes([topology, 0, producers]) + (0).to_bytes(8, "big")
    for s in (profile, variant, mode):
        p += bytes([len(s)]) + s
    return p


def _footer_payload(
    last_seq: int,
    attempted: int,
    accepted: int | None = None,
    refused: int = 0,
    samples: int = 0,
) -> bytes:
    """Cloture. Convention : les compteurs sont captures AVANT l'emission du
    footer, donc accepted + refused + 1 == attempted quand tout est coherent."""
    if accepted is None:
        accepted = max(attempted - refused - 1, 0)
    return (
        bytes([T.TM_FOOTER])
        + last_seq.to_bytes(4, "big")
        + attempted.to_bytes(4, "big")
        + accepted.to_bytes(4, "big")
        + refused.to_bytes(4, "big")
        + samples.to_bytes(4, "big")
    )


def _summary_payload(
    issued: int = 0,
    ok: int = 0,
    producer_drop: int = 0,
    gap_records_merged: int = 0,
    gap_capacity: int = 4,
    producers: int = 1,
) -> bytes:
    """Bilan v4 : un bloc PAR PRODUCTEUR, puis les champs globaux."""
    p = bytes([T.TM_SUMMARY, producers])
    for pid in range(producers):
        vals = [issued, ok, 0, 0, 0, 0, 0, producer_drop] if pid == 0 else [0] * 8
        vals += [0, 0, 0, 0]  # timeouts par cause
        vals += [0, 0, 0, 0]  # file, famine, profondeur, reserve
        p += bytes([pid])
        for v in vals:
            p += v.to_bytes(4, "big")
        p += (0).to_bytes(8, "big") + (0).to_bytes(8, "big")
    p += gap_records_merged.to_bytes(4, "big") + gap_capacity.to_bytes(4, "big")
    return p + (0).to_bytes(8, "big")


def _sample_payload(
    producer_id=0,
    local=0,
    glob=0,
    t_req=0,
    t_grant=0,
    t_rel=10,
    t_end=10,
    status=0,
    cause=0,
    flags=0,
) -> bytes:
    """Echantillon v4 : deux numerotations et quatre instants de bus."""
    p = bytes([T.TM_SAMPLE, producer_id])
    p += local.to_bytes(4, "big") + glob.to_bytes(4, "big")
    for v in (t_req, t_grant, t_rel, t_end):
        p += v.to_bytes(8, "big")
    return p + bytes([status, cause, flags])


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
    """Flux v4 produit par l'encodeur C : ce test verrouille les deux formats."""

    def setUp(self):
        self.parsed = T.parse_stream(_load_golden_stream())
        self.expected = _load_expected()

    def test_entete(self):
        self.assertEqual(self.parsed["header"], self.expected["header"])
        self.assertEqual(self.parsed["header"]["stream_version"], 4)

    def test_cloture_et_sequence(self):
        self.assertEqual(self.parsed["footer"], self.expected["footer"])
        self.assertTrue(self.parsed["footer_consistent"])
        self.assertEqual(self.parsed["sequence"], self.expected["sequence"])
        self.assertEqual(self.parsed["transport_gap"], self.expected["transport_gap"])

    def test_echantillons(self):
        keys = (
            "producer_id",
            "producer_sequence_id",
            "global_event_seq",
            "t_request",
            "t_grant",
            "t_release",
            "t_end",
            "status",
            "timeout_cause",
        )
        got = [{k: s[k] for k in keys} for s in self.parsed["samples"]]
        self.assertEqual(got, self.expected["samples"])

    def test_deux_producteurs_entrelaces(self):
        ids = [s["producer_id"] for s in self.parsed["samples"]]
        self.assertGreaterEqual(len(set(ids)), 2, "au moins deux producteurs")
        self.assertNotEqual(ids, sorted(ids), "les producteurs sont ENTRELACES")

    def test_ordre_global_et_sequences_locales(self):
        """Les deux numerotations coexistent sans que l'une derive de l'autre."""
        globals_ = [s["global_event_seq"] for s in self.parsed["samples"]]
        self.assertEqual(globals_, sorted(globals_), "ordre global monotone")
        self.assertEqual(len(set(globals_)), len(globals_), "aucun doublon global")
        per: dict[int, list[int]] = {}
        for s in self.parsed["samples"]:
            per.setdefault(s["producer_id"], []).append(s["producer_sequence_id"])
        for pid, seqs in per.items():
            with self.subTest(producer=pid):
                self.assertEqual(seqs, sorted(seqs), "sequence locale croissante")

    def test_invariants_de_bus(self):
        """bus_wait = t_grant - t_request et bus_hold = t_release - t_grant."""
        saw_wait = False
        for s in self.parsed["samples"]:
            with self.subTest(seq=s["global_event_seq"]):
                self.assertEqual(T.bus_wait_ticks(s), s["t_grant"] - s["t_request"])
                self.assertEqual(T.bus_hold_ticks(s), s["t_release"] - s["t_grant"])
            saw_wait = saw_wait or T.bus_wait_ticks(s) > 0
        self.assertTrue(saw_wait, "le bus PARTAGE produit une attente non nulle")
        checks = T.check_bus_invariants(self.parsed)
        self.assertEqual(checks["ordering_violations"], [], "aucun instant incoherent")

    def test_cause_de_timeout_explicite(self):
        causes = {s["timeout_cause"] for s in self.parsed["samples"]}
        self.assertIn(1, causes, "au moins un timeout impute a l'ATTENTE DU BUS")
        self.assertNotIn(2, causes, "aucun timeout impute a tort au peripherique")

    def test_lacune_attribuee_a_un_producteur(self):
        self.assertEqual(self.parsed["gaps"], self.expected["gaps"])
        gap = self.parsed["gaps"][0]
        self.assertEqual(sum(gap["lost_by_producer"]), gap["lost_count"])
        self.assertTrue(
            any(gap["lost_by_producer"]), "l'identite du producteur ayant perdu est conservee"
        )

    def test_reconciliation_globale_et_par_producteur(self):
        s = self.parsed["summary"]
        self.assertEqual(s, self.expected["summary"])
        self.assertTrue(S.reconcile(s)["balanced"], "reconciliation GLOBALE")
        for entry in s["per_producer"]:
            with self.subTest(producer=entry["producer_id"]):
                self.assertTrue(
                    S.reconcile(entry)["balanced"], "reconciliation PAR PRODUCTEUR"
                )

    def test_analyse_par_producteur(self):
        per = T.per_producer_analysis(self.parsed)
        self.assertEqual(len(per), self.parsed["summary"]["producer_count"])
        for pid, block in per.items():
            with self.subTest(producer=pid):
                self.assertTrue(block["reconciliation"]["balanced"])
                self.assertTrue(block["local_sequence_contiguous"])

    def test_bloc_analyse_conforme_au_schema(self):
        block = T.build_analysis(self.parsed)
        home.validate(block, home.load_schema("latency-analysis.schema.json"))
        self.assertEqual(block["series_completeness"], self.expected["series_completeness"])
        self.assertEqual(
            block["eligibility"]["blocking"], self.expected["eligibility_blocking"]
        )

    def test_serie_normalisee(self):
        rows = T.to_series_rows(self.parsed)
        self.assertEqual(len(rows), len(self.parsed["samples"]))
        self.assertEqual(len(T.CSV_COLUMNS), len(rows[0]))
        self.assertIn("bus_wait_ticks", T.CSV_COLUMNS)
        self.assertIn("producer_id", T.CSV_COLUMNS)


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
        for extra in (b"\x00" * 43, b"\x00" * 45):
            stream = _frame(0, _header_payload()) + _frame(1, bytes([T.TM_SAMPLE]) + extra)
            with self.subTest(size=len(extra)), self.assertRaises(T.TelemetryError):
                T.parse_stream(stream)

    def test_summary_duplique(self):
        b = _summary_payload()
        stream = _frame(0, _header_payload()) + _frame(1, b) + _frame(2, b)
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

    def test_type_inconnu_rejete(self):
        """Version 3 : un type inconnu est REJETE, pas tolere."""
        stream = _frame(0, _header_payload()) + _frame(1, bytes([0x7F]) + b"\x00" * 4)
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_octets_parasites_ignores(self):
        parsed = T.parse_stream(b"\x00\x01\x02" + _load_golden_stream())
        self.assertEqual(parsed["frames_decoded"], _load_expected()["frames_decoded"])


class StreamClosureTest(unittest.TestCase):
    """Point 2 : sans cloture, une perte des DERNIERES trames est indetectable."""

    def test_cloture_absente_rend_incomplete(self):
        stream = _frame(0, _header_payload()) + _frame(1, _summary_payload())
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
        stream = _frame(0, _header_payload()) + _frame(1, _summary_payload())
        stream += _frame(5, _footer_payload(5, 6))
        parsed = T.parse_stream(stream)
        self.assertTrue(parsed["footer_present"])
        self.assertEqual(parsed["transport_gap"], 3, "seq 2..4 absentes")
        self.assertEqual(parsed["stream_completeness"], "incomplete")

    def test_cloture_incoherente(self):
        # frames_attempted ne correspond pas a last_stream_seq + 1.
        stream = _frame(0, _header_payload()) + _frame(1, _summary_payload())
        stream += _frame(2, _footer_payload(2, 99))
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
        stream = _frame(0, _header_payload(tick_hz=0)) + _frame(1, _summary_payload())
        stream += _frame(2, _footer_payload(2, 3))
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
        sample = _sample_payload(t_req=start, t_grant=start, t_rel=5, t_end=5)
        stream = _frame(0, _header_payload(width=32)) + _frame(1, sample)
        stream += _frame(2, _summary_payload(issued=1, ok=1))
        stream += _frame(3, _footer_payload(3, 4, samples=1))
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
                {
                    "producer_id": 0,
                    "producer_sequence_id": i,
                    "global_event_seq": i,
                    "t_request": 0,
                    "t_grant": 0,
                    "t_release": v,
                    "t_end": v,
                    "status": 0,
                    "timeout_cause": 0,
                    "flags": 0,
                }
                for i, v in enumerate(latencies)
            ],
            "gaps": [
                {
                    "lost_count": producer_drop,
                    "after_global_seq": ok - 1,
                    "lost_by_producer": [producer_drop, 0, 0, 0],
                }
            ]
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
                "gap_records_merged": 0,
                "gap_capacity": 8,
                "timeout_budget_ticks": 1000,
            },
            "device_histogram": None,
            "transport_gap": 0,
            "stream_completeness": "complete",
            "summary_present": True,
            "footer_present": True,
            "footer_consistent": True,
            "gap_localization_complete": True,
            "histogram_required": False,
            "histogram_present": False,
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


class StreamStateMachineTest(unittest.TestCase):
    """Point 3 : HEADER -> (SAMPLE|GAP)* -> SUMMARY -> HISTOGRAM? -> FOOTER."""

    _SAMPLE = _sample_payload()
    _GAP = bytes([T.TM_GAP]) + (0).to_bytes(4, "big") + (0).to_bytes(4, "big") + bytes([0])

    def test_ordre_nominal_accepte(self):
        stream = _frame(0, _header_payload()) + _frame(1, self._SAMPLE)
        stream += _frame(2, self._GAP) + _frame(3, _summary_payload(issued=1, ok=1))
        stream += _frame(4, _footer_payload(4, 5, samples=1))
        parsed = T.parse_stream(stream)
        self.assertEqual(parsed["stream_completeness"], "complete")

    def test_sample_apres_summary_interdit(self):
        stream = _frame(0, _header_payload()) + _frame(1, _summary_payload())
        stream += _frame(2, self._SAMPLE)
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_gap_apres_summary_interdit(self):
        stream = _frame(0, _header_payload()) + _frame(1, _summary_payload())
        stream += _frame(2, self._GAP)
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_footer_avant_summary_interdit(self):
        stream = _frame(0, _header_payload()) + _frame(1, _footer_payload(1, 2))
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_message_apres_footer_interdit(self):
        stream = _frame(0, _header_payload()) + _frame(1, _summary_payload())
        stream += _frame(2, _footer_payload(2, 3)) + _frame(3, self._SAMPLE)
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_footer_duplique_interdit(self):
        stream = _frame(0, _header_payload()) + _frame(1, _summary_payload())
        stream += _frame(2, _footer_payload(2, 3)) + _frame(3, _footer_payload(3, 4))
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

    def test_histogramme_avant_header_interdit(self):
        hist = bytes([T.TM_HISTOGRAM]) + b"\x00" * 21
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(_frame(0, hist))


class FooterReconciliationTest(unittest.TestCase):
    """Points 1, 2 et 5 : le bilan est obligatoire et la cloture est reconciliee."""

    def _stream(self, footer: bytes, summary: bytes | None = None) -> bytes:
        s = _frame(0, _header_payload())
        seq = 1
        if summary is not None:
            s += _frame(seq, summary)
            seq += 1
        return s + _frame(seq, footer)

    def test_summary_absent_rend_ineligible(self):
        """Sans bilan, on ignore si TOUTES les transactions sont representees."""
        stream = _frame(0, _header_payload()) + _frame(1, _footer_payload(1, 2))
        # Le footer avant summary est refuse par la machine d'etat : on verifie
        # donc le cas d'un flux qui s'arrete apres l'en-tete.
        with self.assertRaises(T.TelemetryError):
            T.parse_stream(stream)

        only_header = _frame(0, _header_payload())
        parsed = T.parse_stream(only_header)
        self.assertFalse(parsed["summary_present"])
        block = T.build_analysis(parsed)
        self.assertFalse(block["quantiles_verdict_eligible"])
        self.assertIn("summary_present", block["eligibility"]["blocking"])

    def test_summary_non_equilibre(self):
        # issued = 10 mais seulement 4 statuts terminaux comptes.
        stream = self._stream(
            _footer_payload(2, 3, samples=10), _summary_payload(issued=10, ok=4)
        )
        block = T.build_analysis(T.parse_stream(stream))
        self.assertFalse(block["reconciliation"]["balanced"])
        self.assertIn("reconciliation_balanced", block["eligibility"]["blocking"])

    def test_issued_different_de_samples_attempted(self):
        stream = self._stream(
            _footer_payload(2, 3, samples=99), _summary_payload(issued=4, ok=4)
        )
        parsed = T.parse_stream(stream)
        self.assertFalse(parsed["footer_checks"]["samples_attempted_matches_issued"])
        self.assertFalse(parsed["footer_consistent"])
        block = T.build_analysis(parsed)
        self.assertIn("summary_footer_consistent", block["eligibility"]["blocking"])

    def test_sequence_de_la_trame_footer_incorrecte(self):
        # Le footer annonce last_stream_seq = 9 mais sa trame porte seq 2.
        stream = self._stream(_footer_payload(9, 10), _summary_payload())
        parsed = T.parse_stream(stream)
        self.assertFalse(parsed["footer_checks"]["footer_frame_seq_matches"])
        self.assertFalse(parsed["footer_consistent"])

    def test_compteurs_footer_incoherents(self):
        # accepted + refused + 1 != attempted.
        stream = self._stream(_footer_payload(2, 3, accepted=0, refused=0), _summary_payload())
        parsed = T.parse_stream(stream)
        self.assertFalse(parsed["footer_checks"]["frames_accounted"])

    def test_premiere_sequence_non_nulle(self):
        stream = _frame(5, _header_payload()) + _frame(6, _summary_payload())
        stream += _frame(7, _footer_payload(7, 8))
        parsed = T.parse_stream(stream)
        self.assertFalse(parsed["footer_checks"]["first_sequence_is_zero"])
        self.assertFalse(parsed["footer_consistent"])


class GapLocalizationTest(unittest.TestCase):
    """Point 4 : des pertes non localisables retirent l'eligibilite."""

    def _stream(self, summary: bytes) -> bytes:
        s = _frame(0, _header_payload()) + _frame(1, summary)
        return s + _frame(2, _footer_payload(2, 3, samples=4))

    def test_plages_fusionnees(self):
        parsed = T.parse_stream(
            self._stream(
                _summary_payload(issued=4, ok=2, producer_drop=2, gap_records_merged=1)
            )
        )
        self.assertFalse(parsed["gap_localization_complete"])
        block = T.build_analysis(parsed)
        self.assertIn("gap_localization_complete", block["eligibility"]["blocking"])

    def test_pertes_sans_capacite_de_localisation(self):
        parsed = T.parse_stream(
            self._stream(_summary_payload(issued=4, ok=2, producer_drop=2, gap_capacity=0))
        )
        self.assertFalse(
            parsed["gap_localization_complete"],
            "des pertes sans place declaree ne sont pas localisables",
        )

    def test_aucune_perte_capacite_nulle_reste_localisable(self):
        parsed = T.parse_stream(self._stream(_summary_payload(issued=4, ok=4, gap_capacity=0)))
        self.assertTrue(parsed["gap_localization_complete"], "aucune perte a localiser")


class HistogramAnnouncedTest(unittest.TestCase):
    """Point 6 : un histogramme annonce ne peut pas disparaitre silencieusement."""

    def test_annonce_mais_absent(self):
        header = _header_payload()
        # Active le drapeau histogram_enabled (offset 18 du payload).
        header = header[:18] + bytes([1]) + header[19:]
        stream = _frame(0, header) + _frame(1, _summary_payload())
        stream += _frame(2, _footer_payload(2, 3))
        parsed = T.parse_stream(stream)
        self.assertTrue(parsed["histogram_required"])
        self.assertFalse(parsed["histogram_present"])
        block = T.build_analysis(parsed)
        self.assertIn("histogram_usable_if_required", block["eligibility"]["blocking"])

    def test_desactive_et_absent_reste_utilisable(self):
        stream = _frame(0, _header_payload()) + _frame(1, _summary_payload())
        stream += _frame(2, _footer_payload(2, 3))
        block = T.build_analysis(T.parse_stream(stream))
        self.assertTrue(
            block["eligibility"]["conditions"]["histogram_usable_if_required"],
            "absence normale quand l'histogramme est desactive",
        )

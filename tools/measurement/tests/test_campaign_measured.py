# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Chemin REEL : une acquisition mesuree sort en RAW et ne devient [M] que par
une promotion explicite et controlee. Utilise un driver mesure de TEST (non
livre). Couvre les negatifs (metadonnees incompletes, promotion non qualifiee)."""

import tempfile
import unittest

from measurement.acquisition.driver import InstrumentDriver, register_driver
from measurement.model import IncompleteMetadataError
from measurement.orchestration import (
    GuardrailError,
    mark_reviewed,
    promote_to_measured,
    record_analysis,
    record_verdict,
    run_campaign,
)


@register_driver("measured_stub")
class _MeasuredStub(InstrumentDriver):
    """Driver mesure de TEST (pas un pilote reel ; non enregistre en production)."""

    nature = "measured"

    def acquire(self, definition_id, config):
        return [
            {
                "name": "signal",
                "columns": [
                    {"name": "index", "unit": "sample"},
                    {"name": "value", "unit": "a.u."},
                ],
                "rows": [[0, 1.0], [1, 2.0], [2, 3.0]],
            }
        ]


def _definition():
    return {
        "experiment_id": "EXP-TEST-REAL-001",
        "experimental_question": "Question observable de test ?",
        "expected_observation": "Observation attendue de test.",
        "verdict_rule": "PASS si observation conforme.",
        "protocol_ref": "example://test-real",
        "dec": "DEC-TEST-000",
        "dut": {"description": "DUT de test"},
        "acquisition": {"driver": "measured_stub", "config": {}},
    }


def _complete_context():
    return {
        "setup": "banc-A",
        "fixture": "fixture-1",
        "instrument": {
            "brand": "ACME",
            "model": "X1",
            "firmware": "1.0",
            "probe": "p1",
            "calibre": "10x",
            "bandwidth": "100MHz",
            "parameters": "DC",
        },
        "dut": {"description": "carte de test", "hardware_revision": "revA"},
        "build_manifest": {
            "git_commit": "abc123",
            "git_dirty": False,
            "toolchain": "gcc",
            "esp_idf_version": "5.2",
            "target": "esp32s3",
            "build_configuration_hash": "cfg1",
            "artifact_sha256": "deadbeef",
            "tooling_version": "0.1.0",
            "measurement_tooling_commit": "def456",
        },
        "conditions": {"temperature": "25C", "supply": "3V3", "environment": "labo"},
        "calibration": {"reference": "cal-1", "verified_at": "2026-01-01"},
        "baseline": {"status": "approved", "record": "baseline-1"},
    }


class TestMeasuredPath(unittest.TestCase):
    def test_measured_requires_context(self):
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaises(IncompleteMetadataError):
                run_campaign(_definition(), tmp)  # pas de contexte

    def test_measured_rejects_incomplete_context(self):
        ctx = _complete_context()
        ctx["instrument"]["brand"] = ""  # champ vide interdit
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaises(IncompleteMetadataError):
                run_campaign(_definition(), tmp, context=ctx)

    def test_measured_starts_raw(self):
        with tempfile.TemporaryDirectory() as tmp:
            _run_dir, manifest = run_campaign(_definition(), tmp, context=_complete_context())
            self.assertEqual(manifest["acquisition_nature"], "measured")
            self.assertEqual(manifest["evidence_status"], "RAW")
            self.assertEqual(manifest["verdict"], "NOT_RUN")

    def test_promotion_requires_review_first(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp, context=_complete_context())
            with self.assertRaises(GuardrailError):
                promote_to_measured(run_dir)  # pas encore REVIEWED

    def test_promotion_requires_approved_baseline(self):
        ctx = _complete_context()
        ctx["baseline"]["status"] = "draft"
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp, context=ctx)
            mark_reviewed(run_dir)
            record_analysis(run_dir, {"result": "ok"})
            record_verdict(run_dir, "PASS", "conforme")
            with self.assertRaises(GuardrailError):
                promote_to_measured(run_dir)

    def test_promotion_requires_analysis(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp, context=_complete_context())
            mark_reviewed(run_dir)
            record_verdict(run_dir, "PASS", "conforme")
            with self.assertRaises(GuardrailError):
                promote_to_measured(run_dir)  # analyse absente

    def test_promotion_requires_decisive_verdict(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp, context=_complete_context())
            mark_reviewed(run_dir)
            record_analysis(run_dir, {"result": "ambigu"})
            record_verdict(run_dir, "INCONCLUSIVE", "donnees insuffisantes")
            with self.assertRaises(GuardrailError):
                promote_to_measured(run_dir)

    def test_full_controlled_promotion_to_measured(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp, context=_complete_context())
            mark_reviewed(run_dir)
            record_analysis(run_dir, {"result": "ok", "value": 2.0})
            record_verdict(run_dir, "PASS", "criteres satisfaits")
            manifest = promote_to_measured(run_dir)
            self.assertEqual(manifest["evidence_status"], "M")
            self.assertEqual(manifest["verdict"], "PASS")

    def test_promotion_detects_tampering(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp, context=_complete_context())
            mark_reviewed(run_dir)
            record_analysis(run_dir, {"result": "ok"})
            record_verdict(run_dir, "PASS", "conforme")
            series = run_dir / "series" / "signal.csv"
            series.write_text(series.read_text() + "3,9\n")
            with self.assertRaises(GuardrailError):
                promote_to_measured(run_dir)


if __name__ == "__main__":
    unittest.main()

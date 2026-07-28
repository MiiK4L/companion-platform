# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Chemin REEL : sortie RAW, revue tracée, verdict lié, promotion contrôlée vers
M, verrouillage à M, build git sale. Driver mesuré de TEST (non livré)."""

import tempfile
import unittest
from pathlib import Path

from measurement.acquisition.driver import InstrumentDriver, register_driver
from measurement.analysis.schema import SchemaError
from measurement.common.hashing import sha256_file
from measurement.model import IncompleteMetadataError
from measurement.orchestration import (
    GuardrailError,
    mark_reviewed,
    promote_to_measured,
    record_analysis,
    record_verdict,
    run_campaign,
)

_TS = "2026-01-01T00:00:00+00:00"


@register_driver("measured_stub")
class _MeasuredStub(InstrumentDriver):
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
        "expected_observation": "Observation attendue.",
        "verdict_rule": "PASS si conforme.",
        "protocol_ref": "example://test-real",
        "dec": "DEC-TEST-000",
        "dut": {"description": "DUT de test"},
        "acquisition": {"driver": "measured_stub", "config": {}},
    }


def _context(git_dirty=False, dirty_sha=None):
    na = "x"
    build = {
        "git_commit": "abc1234",
        "git_dirty": git_dirty,
        "toolchain": "gcc",
        "esp_idf_version": "5.2",
        "target": "esp32s3",
        "build_configuration_hash": "b" * 64,
        "artifact_sha256": "a" * 64,
        "tooling_version": "0.1.0",
        "measurement_tooling_commit": "def4567",
    }
    if dirty_sha is not None:
        build["dirty_diff_sha256"] = dirty_sha
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
        "build_manifest": build,
        "conditions": {"temperature": "25C", "supply": "3V3", "environment": na},
        "calibration": {"reference": "cal-1", "verified_at": "2026-01-01"},
        "baseline": {"status": "approved", "record": "baseline-1"},
    }


def _baseline():
    return {
        "baseline_id": "baseline-1",
        "baseline_version": 1,
        "protocol_ref": "example://test-real",
        "protocol_commit": "commit-xyz",
        "status": "approved",
        "approved_by": "reviewer-1",
        "approved_at": _TS,
        "bl_fields": [
            {
                "id": "BL-001",
                "name": "seuil_inrush",
                "value": "TBD",
                "confidence": "hypothesis",
                "justification": "hypothese de lancement",
            },
        ],
    }


def _link(run_dir):
    return {
        "verdict_rule_ref": "PASS si conforme.",
        "analysis_result_sha256": sha256_file(Path(run_dir) / "analysis-result.json"),
        "analysis_tool": "analyseur",
        "analysis_tool_version": "1.0",
        "decided_by": "ing-1",
        "decided_at": _TS,
    }


def _analysis():
    return {
        "experiment_id": "EXP-TEST-REAL-001",
        "analysis_tool": "analyseur",
        "analysis_tool_version": "1.0",
        "summary": "conforme",
        "values": {"value": 2.0},
    }


def _qualify(run_dir, verdict="PASS"):
    record_analysis(run_dir, _analysis())
    record_verdict(run_dir, verdict, "motif", link=_link(run_dir), timestamp=_TS)
    mark_reviewed(
        run_dir,
        reviewer="rev-1",
        reviewed_at=_TS,
        review_reason="revue ok",
        review_checklist=["a", "b"],
    )


class TestMeasuredPath(unittest.TestCase):
    def test_requires_context(self):
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaises(IncompleteMetadataError):
                run_campaign(_definition(), tmp)

    def test_rejects_incomplete_context(self):
        ctx = _context()
        ctx["instrument"]["brand"] = ""
        with tempfile.TemporaryDirectory() as tmp:
            # Champ vide : refuse par le schema (minLength) ou la completude.
            with self.assertRaises((SchemaError, IncompleteMetadataError)):
                run_campaign(_definition(), tmp, context=ctx)

    def test_starts_raw(self):
        with tempfile.TemporaryDirectory() as tmp:
            _run, view = run_campaign(
                _definition(), tmp, context=_context(), baseline_record=_baseline()
            )
            self.assertEqual(view["evidence_status"], "RAW")
            self.assertEqual(view["verdict"], "NOT_RUN")

    def test_promotion_requires_review(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _definition(), tmp, context=_context(), baseline_record=_baseline()
            )
            record_analysis(run_dir, _analysis())
            record_verdict(run_dir, "PASS", "m", link=_link(run_dir), timestamp=_TS)
            with self.assertRaises(GuardrailError):
                promote_to_measured(
                    run_dir, promoted_by="p", promoted_at=_TS, promotion_reason="r"
                )

    def test_promotion_requires_baseline_record(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp, context=_context())
            _qualify(run_dir)
            with self.assertRaises(GuardrailError):
                promote_to_measured(
                    run_dir, promoted_by="p", promoted_at=_TS, promotion_reason="r"
                )

    def test_promotion_requires_analysis(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _definition(), tmp, context=_context(), baseline_record=_baseline()
            )
            mark_reviewed(
                run_dir,
                reviewer="r",
                reviewed_at=_TS,
                review_reason="x",
                review_checklist=["a"],
            )
            with self.assertRaises(GuardrailError):
                promote_to_measured(
                    run_dir, promoted_by="p", promoted_at=_TS, promotion_reason="r"
                )

    def test_promotion_requires_decisive_verdict(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _definition(), tmp, context=_context(), baseline_record=_baseline()
            )
            _qualify(run_dir, verdict="INCONCLUSIVE")
            with self.assertRaises(GuardrailError):
                promote_to_measured(
                    run_dir, promoted_by="p", promoted_at=_TS, promotion_reason="r"
                )

    def test_full_promotion_and_lock(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _definition(), tmp, context=_context(), baseline_record=_baseline()
            )
            _qualify(run_dir)
            view = promote_to_measured(
                run_dir,
                promoted_by="ing-1",
                promoted_at=_TS,
                promotion_reason="criteres satisfaits",
            )
            self.assertEqual(view["evidence_status"], "M")
            self.assertTrue(view["locked"])
            # Verrouillage : toute mutation est refusee.
            with self.assertRaises(GuardrailError):
                record_verdict(run_dir, "FAIL", "x", link=_link(run_dir), timestamp=_TS)
            with self.assertRaises(GuardrailError):
                promote_to_measured(
                    run_dir, promoted_by="p", promoted_at=_TS, promotion_reason="r"
                )

    def test_review_identity_required(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _definition(), tmp, context=_context(), baseline_record=_baseline()
            )
            with self.assertRaises(GuardrailError):
                mark_reviewed(
                    run_dir,
                    reviewer="",
                    reviewed_at=_TS,
                    review_reason="x",
                    review_checklist=["a"],
                )

    def test_analysis_is_immutable(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _definition(), tmp, context=_context(), baseline_record=_baseline()
            )
            record_analysis(run_dir, _analysis())
            with self.assertRaises(GuardrailError):
                record_analysis(run_dir, _analysis())

    def test_dirty_build_blocks_then_allows_with_diff(self):
        patch = "--- a\n+++ b\n@@ dirty @@\n"
        from measurement.common.hashing import sha256_text

        # Sans diff archive : promotion refusee.
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _definition(),
                tmp,
                context=_context(git_dirty=True),
                baseline_record=_baseline(),
            )
            _qualify(run_dir)
            with self.assertRaises(GuardrailError):
                promote_to_measured(
                    run_dir, promoted_by="p", promoted_at=_TS, promotion_reason="r"
                )
        # Avec diff archive + hashe + justifie : promotion possible.
        with tempfile.TemporaryDirectory() as tmp:
            ctx = _context(git_dirty=True, dirty_sha=sha256_text(patch))
            run_dir, _ = run_campaign(
                _definition(),
                tmp,
                context=ctx,
                baseline_record=_baseline(),
                dirty_diff=patch,
            )
            _qualify(run_dir)
            view = promote_to_measured(
                run_dir,
                promoted_by="ing-1",
                promoted_at=_TS,
                promotion_reason="build sale justifie",
            )
            self.assertEqual(view["evidence_status"], "M")


if __name__ == "__main__":
    unittest.main()

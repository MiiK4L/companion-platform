# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Négatifs d'intégrité : état dérivé falsifié, transitions illégales, événements
mal formés, réutilisation de run_id, incohérences baseline/analyse, hash de build
invalide, et reconstruction des vues dérivées. Sans matériel."""

import json
import tempfile
import unittest
from pathlib import Path

from measurement.acquisition.driver import InstrumentDriver, register_driver
from measurement.analysis.schema import SchemaError
from measurement.common.canonical import canonical_json
from measurement.common.hashing import sha256_file
from measurement.orchestration import (
    GuardrailError,
    mark_reviewed,
    promote_to_measured,
    rebuild_derived,
    record_analysis,
    record_verdict,
    run_campaign,
    verify_run,
)

_TS = "2026-01-01T00:00:00+00:00"


@register_driver("meas_integrity")
class _Meas(InstrumentDriver):
    nature = "measured"

    def acquire(self, definition_id, config):
        return [
            {
                "name": "signal",
                "columns": [{"name": "i", "unit": "n"}, {"name": "v", "unit": "u"}],
                "rows": [[0, 1.0], [1, 2.0]],
            }
        ]


def _sim_definition():
    return {
        "experiment_id": "EXP-INT-000",
        "experimental_question": "q",
        "expected_observation": "o",
        "verdict_rule": "r",
        "protocol_ref": "example://int",
        "dec": "DEC-INT-000",
        "dut": {"description": "d"},
        "acquisition": {"driver": "simulation", "config": {"series": "signal", "samples": 3}},
    }


def _real_definition():
    return dict(_sim_definition(), acquisition={"driver": "meas_integrity", "config": {}})


def _context(protocol="example://int", record="bl-1"):
    return {
        "setup": "banc",
        "fixture": "fix",
        "instrument": {
            "brand": "b",
            "model": "m",
            "firmware": "f",
            "probe": "p",
            "calibre": "c",
            "bandwidth": "bw",
            "parameters": "pa",
        },
        "dut": {"description": "d", "hardware_revision": "rA"},
        "build_manifest": {
            "git_commit": "abc1234",
            "git_dirty": False,
            "toolchain": "gcc",
            "esp_idf_version": "5.2",
            "target": "esp32s3",
            "build_configuration_hash": "b" * 64,
            "artifact_sha256": "a" * 64,
            "tooling_version": "0.1.0",
            "measurement_tooling_commit": "def4567",
        },
        "conditions": {"temperature": "25C", "supply": "3V3", "environment": "lab"},
        "calibration": {"reference": "cal", "verified_at": _TS},
        "baseline": {"status": "approved", "record": record},
    }


def _baseline(protocol="example://int", bid="bl-1"):
    return {
        "baseline_id": bid,
        "protocol_ref": protocol,
        "protocol_commit": "commit",
        "status": "approved",
        "approved_by": "rev",
        "approved_at": _TS,
        "resolved_bl_fields": ["s"],
    }


def _analysis(exp="EXP-INT-000"):
    return {
        "experiment_id": exp,
        "analysis_tool": "t",
        "analysis_tool_version": "1",
        "summary": "ok",
        "values": {"v": 1},
    }


def _qualify(run_dir, exp="EXP-INT-000"):
    record_analysis(run_dir, _analysis(exp))
    link = {
        "verdict_rule_ref": "r",
        "analysis_result_sha256": sha256_file(Path(run_dir) / "analysis-result.json"),
        "analysis_tool": "t",
        "analysis_tool_version": "1",
        "decided_by": "i",
        "decided_at": _TS,
    }
    record_verdict(run_dir, "PASS", "m", link=link, timestamp=_TS)
    mark_reviewed(
        run_dir, reviewer="r", reviewed_at=_TS, review_reason="x", review_checklist=["a"]
    )


def _write_event(run_dir, name, event):
    (Path(run_dir) / "evidence-events" / name).write_text(
        canonical_json(event) + "\n", encoding="utf-8"
    )


def _last_event_sha(run_dir):
    files = sorted((Path(run_dir) / "evidence-events").glob("[0-9][0-9][0-9][0-9]-*.json"))
    return sha256_file(files[-1])


class TestIntegrity(unittest.TestCase):
    def test_falsified_derived_state(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_sim_definition(), tmp)
            state = run_dir / "evidence-state.json"
            data = json.loads(state.read_text())
            data["verdict"] = "PASS"
            state.write_text(canonical_json(data) + "\n", encoding="utf-8")
            with self.assertRaises(GuardrailError):
                verify_run(run_dir)

    def test_illegal_transition(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _real_definition(), tmp, context=_context(), baseline_record=_baseline()
            )
            event = {
                "event_id": "0002",
                "event_type": "promotion",
                "from_status": "REVIEWED",
                "to_status": "M",
                "actor": "x",
                "timestamp": _TS,
                "reason": "r",
                "inputs": [],
                "previous_event_sha256": _last_event_sha(run_dir),
                "details": {"promoted_by": "x", "promoted_at": _TS, "promotion_reason": "r"},
            }
            _write_event(run_dir, "0002-promotion.json", event)
            with self.assertRaises(GuardrailError):
                verify_run(run_dir)

    def test_review_event_without_reviewer(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _real_definition(), tmp, context=_context(), baseline_record=_baseline()
            )
            event = {
                "event_id": "0002",
                "event_type": "review",
                "from_status": "RAW",
                "to_status": "REVIEWED",
                "actor": "x",
                "timestamp": _TS,
                "reason": "r",
                "inputs": [],
                "previous_event_sha256": _last_event_sha(run_dir),
                "details": {
                    "reviewed_at": _TS,
                    "review_reason": "x",
                    "review_checklist": ["a"],
                },
            }
            _write_event(run_dir, "0002-review.json", event)
            with self.assertRaises(GuardrailError):
                verify_run(run_dir)

    def test_second_acquisition(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_sim_definition(), tmp)
            event = {
                "event_id": "0002",
                "event_type": "acquisition",
                "from_status": "NONE",
                "to_status": "S",
                "actor": "x",
                "timestamp": _TS,
                "reason": "r",
                "inputs": [],
                "previous_event_sha256": _last_event_sha(run_dir),
                "details": {"acquisition_nature": "simulated"},
            }
            _write_event(run_dir, "0002-acquisition.json", event)
            with self.assertRaises(GuardrailError):
                verify_run(run_dir)

    def test_event_after_m(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _real_definition(), tmp, context=_context(), baseline_record=_baseline()
            )
            _qualify(run_dir)
            promote_to_measured(
                run_dir, promoted_by="i", promoted_at=_TS, promotion_reason="ok"
            )
            event = {
                "event_id": "0005",
                "event_type": "verdict",
                "from_status": "M",
                "to_status": "M",
                "actor": "x",
                "timestamp": _TS,
                "reason": "r",
                "inputs": [],
                "previous_event_sha256": _last_event_sha(run_dir),
                "details": {"verdict": "FAIL", "verdict_reason": "x"},
            }
            _write_event(run_dir, "0005-verdict.json", event)
            with self.assertRaises(GuardrailError):
                verify_run(run_dir)

    def test_run_id_reuse_refused(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_campaign(_sim_definition(), tmp, run_id="DUP", generated_at=_TS)
            with self.assertRaises(GuardrailError):
                run_campaign(_sim_definition(), tmp, run_id="DUP", generated_at=_TS)

    def test_baseline_other_protocol(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _real_definition(),
                tmp,
                context=_context(),
                baseline_record=_baseline(protocol="other://x"),
            )
            _qualify(run_dir)
            with self.assertRaises(GuardrailError):
                promote_to_measured(
                    run_dir, promoted_by="i", promoted_at=_TS, promotion_reason="r"
                )

    def test_analysis_other_experiment(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _real_definition(), tmp, context=_context(), baseline_record=_baseline()
            )
            _qualify(run_dir, exp="EXP-OTHER-999")
            with self.assertRaises(GuardrailError):
                promote_to_measured(
                    run_dir, promoted_by="i", promoted_at=_TS, promotion_reason="r"
                )

    def test_build_manifest_invalid_hash(self):
        ctx = _context()
        ctx["build_manifest"]["artifact_sha256"] = "not-a-hash"
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaises(SchemaError):
                run_campaign(_real_definition(), tmp, context=ctx, baseline_record=_baseline())

    def test_delete_derived_then_rebuild(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_sim_definition(), tmp)
            for rel in ("evidence-state.json", "archive-index.json", "report.md"):
                (run_dir / rel).unlink()
            verify_run(run_dir)  # tolère l'absence des vues dérivées
            rebuild_derived(run_dir)
            for rel in ("evidence-state.json", "archive-index.json", "report.md"):
                self.assertTrue((run_dir / rel).is_file())
            verify_run(run_dir)


if __name__ == "__main__":
    unittest.main()

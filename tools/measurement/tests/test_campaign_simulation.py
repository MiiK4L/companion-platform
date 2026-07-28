# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Campagne simulee : archive append-only, reproductibilite, garde-fou (jamais M),
golden dataset, intégrité complète et détection d'altération. Sans matériel."""

import json
import tempfile
import unittest
from pathlib import Path

from measurement.orchestration import (
    GuardrailError,
    current_view,
    normalize_manifest,
    promote_to_measured,
    run_campaign,
    verify_run,
)

_ROOT = Path(__file__).resolve().parents[1]
_GOLDEN = _ROOT / "golden"

_ARCHIVE_FILES = (
    "acquisition-manifest.json",
    "evidence-state.json",
    "archive-index.json",
    "campaign-definition.json",
    "execution-context.json",
    "evidence-events/0001-acquisition.json",
    "series/signal.csv",
    "report.md",
)


def _definition():
    return json.loads((_GOLDEN / "campaign-definition.json").read_text(encoding="utf-8"))


class TestCampaignSimulation(unittest.TestCase):
    def test_archive_layout_and_status(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, view = run_campaign(_definition(), tmp)
            for rel in _ARCHIVE_FILES:
                self.assertTrue((run_dir / rel).is_file(), msg=f"manque: {rel}")
            self.assertEqual(view["acquisition_nature"], "simulated")
            self.assertEqual(view["evidence_status"], "S")
            self.assertEqual(view["verdict"], "NOT_RUN")
            self.assertFalse(view["locked"])

    def test_definition_copied_verbatim(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp)
            copied = json.loads(
                (run_dir / "campaign-definition.json").read_text(encoding="utf-8")
            )
            self.assertEqual(copied, _definition())

    def test_simulated_cannot_be_promoted(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp)
            with self.assertRaises(GuardrailError):
                promote_to_measured(
                    run_dir,
                    promoted_by="x",
                    promoted_at="2026-01-01T00:00:00+00:00",
                    promotion_reason="essai",
                )

    def test_identical_inputs_identical_artifacts(self):
        with tempfile.TemporaryDirectory() as tmp:
            kw = dict(run_id="RID", generated_at="1970-01-01T00:00:00+00:00")
            dir_a, _ = run_campaign(_definition(), Path(tmp) / "a", **kw)
            dir_b, _ = run_campaign(_definition(), Path(tmp) / "b", **kw)
            for rel in _ARCHIVE_FILES:
                self.assertEqual(
                    (dir_a / rel).read_bytes(),
                    (dir_b / rel).read_bytes(),
                    msg=f"non deterministe: {rel}",
                )

    def test_volatile_isolated(self):
        with tempfile.TemporaryDirectory() as tmp:
            dir_a, _ = run_campaign(
                _definition(),
                Path(tmp) / "a",
                run_id="R1",
                generated_at="1970-01-01T00:00:01+00:00",
            )
            dir_b, _ = run_campaign(
                _definition(),
                Path(tmp) / "b",
                run_id="R2",
                generated_at="1970-01-01T00:00:02+00:00",
            )
            man_a = json.loads((dir_a / "acquisition-manifest.json").read_text())
            man_b = json.loads((dir_b / "acquisition-manifest.json").read_text())
            self.assertNotEqual(man_a["run_id"], man_b["run_id"])
            self.assertEqual(normalize_manifest(man_a), normalize_manifest(man_b))
            self.assertEqual(
                (dir_a / "series" / "signal.csv").read_bytes(),
                (dir_b / "series" / "signal.csv").read_bytes(),
            )

    def test_matches_golden(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(
                _definition(),
                tmp,
                run_id="GOLDEN_RUN",
                generated_at="1970-01-01T00:00:00+00:00",
            )
            expected_csv = (_GOLDEN / "expected" / "series" / "signal.csv").read_text(
                encoding="utf-8"
            )
            self.assertEqual(
                (run_dir / "series" / "signal.csv").read_text(encoding="utf-8"),
                expected_csv,
            )
            manifest = json.loads(
                (run_dir / "acquisition-manifest.json").read_text(encoding="utf-8")
            )
            expected_norm = json.loads(
                (_GOLDEN / "expected" / "manifest.normalized.json").read_text(encoding="utf-8")
            )
            self.assertEqual(normalize_manifest(manifest), expected_norm)

    def test_verify_detects_series_tampering(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp)
            verify_run(run_dir)
            series = run_dir / "series" / "signal.csv"
            series.write_text(series.read_text() + "9,9\n", encoding="utf-8")
            with self.assertRaises(GuardrailError):
                verify_run(run_dir)

    def test_verify_detects_event_tampering(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp)
            event = run_dir / "evidence-events" / "0001-acquisition.json"
            data = json.loads(event.read_text())
            data["reason"] = "altere"
            event.write_text(json.dumps(data), encoding="utf-8")
            with self.assertRaises(GuardrailError):
                verify_run(run_dir)

    def test_verify_detects_context_tampering(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp)
            ctx = run_dir / "execution-context.json"
            ctx.write_text(ctx.read_text() + " ", encoding="utf-8")
            with self.assertRaises(GuardrailError):
                verify_run(run_dir)

    def test_current_view_matches_state(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, view = run_campaign(_definition(), tmp)
            self.assertEqual(view, current_view(run_dir))


if __name__ == "__main__":
    unittest.main()

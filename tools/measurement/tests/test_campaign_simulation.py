# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Campagne simulee de bout en bout : archive autosuffisante, round-trip,
reproductibilite, garde-fou (jamais [M]), golden dataset, intégrité. Sans matériel."""

import json
import tempfile
import unittest
from pathlib import Path

from measurement.orchestration import (
    GuardrailError,
    normalize_manifest,
    promote_to_measured,
    run_campaign,
    verify_run,
)

_ROOT = Path(__file__).resolve().parents[1]
_GOLDEN = _ROOT / "golden"

_ARCHIVE_FILES = (
    "campaign-definition.json",
    "execution-context.json",
    "manifest.json",
    "verdict.json",
    "report.md",
    "series/signal.csv",
)


def _definition():
    return json.loads((_GOLDEN / "campaign-definition.json").read_text(encoding="utf-8"))


class TestCampaignSimulation(unittest.TestCase):
    def test_autosufficient_archive_and_status(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, manifest = run_campaign(_definition(), tmp)
            for rel in _ARCHIVE_FILES:
                self.assertTrue((run_dir / rel).is_file(), msg=f"manque: {rel}")
            self.assertEqual(manifest["acquisition_nature"], "simulated")
            self.assertEqual(manifest["evidence_status"], "S")
            self.assertEqual(manifest["verdict"], "NOT_RUN")

    def test_definition_is_copied_verbatim(self):
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
                promote_to_measured(run_dir)

    def test_identical_inputs_produce_identical_artifacts(self):
        with tempfile.TemporaryDirectory() as tmp:
            kw = dict(run_id="RID", generated_at="1970-01-01T00:00:00+00:00")
            dir_a, _ = run_campaign(_definition(), Path(tmp) / "a", **kw)
            dir_b, _ = run_campaign(_definition(), Path(tmp) / "b", **kw)
            for rel in _ARCHIVE_FILES:
                self.assertEqual(
                    (dir_a / rel).read_bytes(),
                    (dir_b / rel).read_bytes(),
                    msg=f"artefact non deterministe: {rel}",
                )

    def test_volatile_fields_isolated_from_determinism(self):
        with tempfile.TemporaryDirectory() as tmp:
            dir_a, man_a = run_campaign(
                _definition(), Path(tmp) / "a", run_id="R1", generated_at="T1"
            )
            dir_b, man_b = run_campaign(
                _definition(), Path(tmp) / "b", run_id="R2", generated_at="T2"
            )
            self.assertNotEqual(man_a["run_id"], man_b["run_id"])
            self.assertEqual(normalize_manifest(man_a), normalize_manifest(man_b))
            self.assertEqual(
                (dir_a / "series" / "signal.csv").read_bytes(),
                (dir_b / "series" / "signal.csv").read_bytes(),
            )

    def test_matches_golden_dataset(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, manifest = run_campaign(
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
            expected_norm = json.loads(
                (_GOLDEN / "expected" / "manifest.normalized.json").read_text(encoding="utf-8")
            )
            self.assertEqual(normalize_manifest(manifest), expected_norm)

    def test_verify_detects_tampering(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, _ = run_campaign(_definition(), tmp)
            verify_run(run_dir)  # intègre
            series = run_dir / "series" / "signal.csv"
            series.write_text(series.read_text(encoding="utf-8") + "9,9\n", encoding="utf-8")
            with self.assertRaises(GuardrailError):
                verify_run(run_dir)


if __name__ == "__main__":
    unittest.main()

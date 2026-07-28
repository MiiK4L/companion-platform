# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Campagne simulee de bout en bout : round-trip, reproductibilite, garde-fou,
golden dataset, empreintes et structure d'archivage. Aucun materiel requis."""

import json
import tempfile
import unittest
from pathlib import Path

from measurement.common.hashing import sha256_file
from measurement.orchestration import (
    GuardrailError,
    mark_measured,
    normalize_manifest,
    run_campaign,
)

_ROOT = Path(__file__).resolve().parents[1]
_GOLDEN = _ROOT / "golden"


def _definition():
    return json.loads((_GOLDEN / "campaign-definition.json").read_text(encoding="utf-8"))


class TestCampaignSimulation(unittest.TestCase):
    def test_structure_and_nature(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, manifest = run_campaign(_definition(), tmp)
            self.assertTrue((run_dir / "manifest.json").is_file())
            self.assertTrue((run_dir / "report.md").is_file())
            self.assertTrue((run_dir / "series" / "signal.csv").is_file())
            self.assertEqual(manifest["nature"], "simulated")
            self.assertEqual(manifest["data_status"], "S")

    def test_guardrail_simulated_cannot_be_measured(self):
        with tempfile.TemporaryDirectory() as tmp:
            _run_dir, manifest = run_campaign(_definition(), tmp)
            with self.assertRaises(GuardrailError):
                mark_measured(manifest)
            # le statut n'a pas ete altere
            self.assertEqual(manifest["data_status"], "S")

    def test_identical_inputs_produce_identical_artifacts(self):
        with tempfile.TemporaryDirectory() as tmp:
            args = dict(run_id="RID", generated_at="1970-01-01T00:00:00+00:00")
            dir_a, _ = run_campaign(_definition(), Path(tmp) / "a", **args)
            dir_b, _ = run_campaign(_definition(), Path(tmp) / "b", **args)
            for rel in ("manifest.json", "report.md", "series/signal.csv"):
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
            # run_id/generated_at different, mais artefacts stables hors volatils
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

    def test_recorded_hashes_match_files(self):
        with tempfile.TemporaryDirectory() as tmp:
            run_dir, manifest = run_campaign(_definition(), tmp)
            for artifact in manifest["artifacts"]:
                self.assertEqual(
                    sha256_file(run_dir / artifact["path"]),
                    artifact["sha256"],
                )


if __name__ == "__main__":
    unittest.main()

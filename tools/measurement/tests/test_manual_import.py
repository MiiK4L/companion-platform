# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Driver manuel de reference : import (bruts texte + binaire), chemin d'import
ephemere (jamais archive), definition independante du chemin, negatifs. Host."""

import json
import tempfile
import unittest
from pathlib import Path

from measurement.analysis.schema import SchemaError
from measurement.common.ids import campaign_definition_id
from measurement.orchestration import run_campaign, verify_run

_TS = "2026-01-01T00:00:00+00:00"


def _definition():
    return {
        "experiment_id": "EXP-MANUAL-001",
        "experimental_question": "q",
        "expected_observation": "o",
        "verdict_rule": "r",
        "protocol_ref": "example://manual",
        "dec": "DEC-MAN-000",
        "dut": {"description": "d"},
        "acquisition": {"driver": "manual-import", "config": {}},
    }


def _context():
    na = "x"
    return {
        "setup": "banc",
        "fixture": "fx",
        "instrument": {
            k: na
            for k in [
                "brand",
                "model",
                "firmware",
                "probe",
                "calibre",
                "bandwidth",
                "parameters",
            ]
        },
        "dut": {"description": "d", "hardware_revision": "rA"},
        "build_manifest": {
            "git_commit": "c",
            "git_dirty": False,
            "toolchain": "gcc",
            "esp_idf_version": "5.2",
            "target": "esp32s3",
            "build_configuration_hash": "b" * 64,
            "artifact_sha256": "a" * 64,
            "tooling_version": "0.1.0",
            "measurement_tooling_commit": "m",
        },
        "conditions": {"temperature": "25C", "supply": "3V3", "environment": "lab"},
        "calibration": {"reference": "cal", "verified_at": _TS},
        "baseline": {"status": "not_applicable", "record": "N/A"},
    }


def _descriptor(**over):
    base = {
        "capture_id": "CAP-001",
        "capture_type": "logic-analyzer",
        "parameters": {"rate": "24MSs"},
        "raw_artifacts": [
            {"group": "logic-analyzer", "name": "cap.bin", "format": "sr"},
            {"group": "serial", "name": "dump.txt", "format": "txt"},
        ],
        "normalized": [{"series": "signal", "from_raw": "logic-analyzer/cap.bin"}],
        "series": [
            {
                "name": "signal",
                "csv": "signal.csv",
                "columns": [{"name": "index", "unit": "n"}, {"name": "value", "unit": "u"}],
            }
        ],
    }
    base.update(over)
    return base


def _make_import(root: Path, *, descriptor=None) -> Path:
    imp = root / "import-XYZ"  # chemin distinctif pour tester une eventuelle fuite
    (imp / "raw" / "logic-analyzer").mkdir(parents=True)
    (imp / "raw" / "serial").mkdir(parents=True)
    (imp / "raw" / "logic-analyzer" / "cap.bin").write_bytes(bytes(range(256)))
    (imp / "raw" / "serial" / "dump.txt").write_text("hello\n", encoding="utf-8")
    (imp / "signal.csv").write_text("index,value\n0,1\n1,2\n", encoding="utf-8")
    (imp / "import.json").write_text(
        json.dumps(descriptor if descriptor is not None else _descriptor()),
        encoding="utf-8",
    )
    return imp


class TestManualImport(unittest.TestCase):
    def test_import_text_and_binary(self):
        with tempfile.TemporaryDirectory() as tmp:
            imp = _make_import(Path(tmp))
            run_dir, view = run_campaign(
                _definition(),
                Path(tmp) / "out",
                context=_context(),
                acquisition_overrides={"import_dir": str(imp)},
                run_id="RUN1",
                generated_at=_TS,
            )
            verify_run(run_dir)
            self.assertEqual(view["evidence_status"], "RAW")
            self.assertEqual(
                (run_dir / "raw" / "logic-analyzer" / "cap.bin").read_bytes(),
                bytes(range(256)),
            )

    def test_import_path_never_archived(self):
        with tempfile.TemporaryDirectory() as tmp:
            imp = _make_import(Path(tmp))
            run_dir, _ = run_campaign(
                _definition(),
                Path(tmp) / "out",
                context=_context(),
                acquisition_overrides={"import_dir": str(imp)},
                run_id="RUN1",
                generated_at=_TS,
            )
            for path in run_dir.rglob("*"):
                if path.is_file():
                    self.assertNotIn(
                        "import-XYZ", path.read_text(errors="ignore"), msg=str(path)
                    )

    def test_definition_id_independent_of_import_dir(self):
        with tempfile.TemporaryDirectory() as tmp:
            imp_a = _make_import(Path(tmp) / "a")
            imp_b = _make_import(Path(tmp) / "b")
            dir_a, _ = run_campaign(
                _definition(),
                Path(tmp) / "oa",
                context=_context(),
                acquisition_overrides={"import_dir": str(imp_a)},
                run_id="RUN1",
                generated_at=_TS,
            )
            dir_b, _ = run_campaign(
                _definition(),
                Path(tmp) / "ob",
                context=_context(),
                acquisition_overrides={"import_dir": str(imp_b)},
                run_id="RUN1",
                generated_at=_TS,
            )
            self.assertEqual(dir_a.parent.name, dir_b.parent.name)  # meme definition_id
            archived = json.loads((dir_a / "campaign-definition.json").read_text())
            self.assertEqual(archived["acquisition"]["config"], {})
            self.assertEqual(dir_a.parent.name, campaign_definition_id(_definition()))

    def test_missing_import_dir_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaises(ValueError):
                run_campaign(
                    _definition(),
                    Path(tmp) / "out",
                    context=_context(),
                    run_id="RUN1",
                    generated_at=_TS,
                )

    def test_malformed_descriptor_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            bad = _descriptor()
            del bad["capture_id"]
            imp = _make_import(Path(tmp), descriptor=bad)
            with self.assertRaises(SchemaError):
                run_campaign(
                    _definition(),
                    Path(tmp) / "out",
                    context=_context(),
                    acquisition_overrides={"import_dir": str(imp)},
                    run_id="RUN1",
                    generated_at=_TS,
                )


if __name__ == "__main__":
    unittest.main()

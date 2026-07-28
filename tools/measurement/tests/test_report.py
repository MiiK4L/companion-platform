# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Rendu du rapport : sections completes, marquage simulation, déterminisme."""

import unittest

from measurement.reporting.report import render_report

_SECTIONS = [
    "Question experimentale",
    "Hypothese",
    "Baseline / protocole applique",
    "Resultats analyses",
    "Incertitude",
    "Ecarts au protocole",
    "Verdict",
    "Motif du verdict",
    "Limitations",
    "Suites proposees",
]


def _manifest(nature):
    return {
        "experiment_id": "EXP-X-001",
        "campaign_definition_id": "def",
        "definition_sha256": "dsha",
        "context_sha256": "csha",
        "run_id": "run",
        "generated_at": "T",
        "acquisition_nature": nature,
        "evidence_status": "S" if nature == "simulated" else "RAW",
        "verdict": "NOT_RUN",
        "verdict_reason": "analyse non executee",
        "protocol_ref": "p",
        "dec": "DEC-X-000",
        "tooling_version": "0.1.0",
        "artifacts": [{"path": "series/signal.csv", "sha256": "abc"}],
    }


def _context():
    return {
        "setup": "banc",
        "fixture": "fix",
        "instrument": {"brand": "ACME", "model": "X1", "firmware": "1.0"},
        "build_manifest": {"artifact_sha256": "deadbeef", "git_commit": "abc"},
    }


class TestReport(unittest.TestCase):
    def test_all_analytic_sections_present(self):
        report = render_report(_manifest("measured"), _context())
        for section in _SECTIONS:
            self.assertIn(f"## {section}", report)

    def test_points_to_exact_artifact_hash(self):
        report = render_report(_manifest("measured"), _context())
        self.assertIn("deadbeef", report)  # artefact SHA-256 du build

    def test_simulated_report_carries_warning(self):
        report = render_report(_manifest("simulated"), _context())
        self.assertIn("CAMPAGNE SIMULEE", report)
        self.assertIn("Aucune donnee `[M]`", report)

    def test_measured_report_has_no_simulation_warning(self):
        report = render_report(_manifest("measured"), _context())
        self.assertNotIn("CAMPAGNE SIMULEE", report)

    def test_render_is_deterministic(self):
        self.assertEqual(
            render_report(_manifest("simulated"), _context()),
            render_report(_manifest("simulated"), _context()),
        )


if __name__ == "__main__":
    unittest.main()

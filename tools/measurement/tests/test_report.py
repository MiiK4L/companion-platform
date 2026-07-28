# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Rendu du rapport : deterministe et marquage explicite d'une simulation."""

import unittest

from measurement.reporting.report import render_report


def _manifest(nature):
    return {
        "campaign_definition_id": "def",
        "run_id": "run",
        "generated_at": "T",
        "nature": nature,
        "data_status": "S" if nature == "simulated" else "M",
        "protocol_ref": "p",
        "dec": "DEC-X-000",
        "instrument": {"used": {}, "configuration": {}},
        "firmware_under_test": "",
        "dut_hardware_revision": "",
        "conditions": {},
        "artifacts": [{"path": "series/signal.csv", "sha256": "abc"}],
    }


class TestReport(unittest.TestCase):
    def test_simulated_report_carries_warning(self):
        report = render_report(_manifest("simulated"))
        self.assertIn("CAMPAGNE SIMULEE", report)
        self.assertIn("Aucune donnee `[M]`", report)

    def test_measured_report_has_no_simulation_warning(self):
        report = render_report(_manifest("measured"))
        self.assertNotIn("CAMPAGNE SIMULEE", report)

    def test_render_is_deterministic(self):
        self.assertEqual(
            render_report(_manifest("simulated")),
            render_report(_manifest("simulated")),
        )


if __name__ == "__main__":
    unittest.main()

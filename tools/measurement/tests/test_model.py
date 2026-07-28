# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Vocabulaire & regles : surete des noms, completude des metadonnees reelles."""

import unittest

from measurement import model


class TestSafeNames(unittest.TestCase):
    def test_series_name_valid(self):
        self.assertEqual(model.ensure_series_name("signal-1"), "signal-1")

    def test_series_name_rejects_uppercase(self):
        with self.assertRaises(model.UnsafeNameError):
            model.ensure_series_name("Signal")

    def test_series_name_rejects_path(self):
        for bad in ("../x", "a/b", "a.b", ""):
            with self.assertRaises(model.UnsafeNameError):
                model.ensure_series_name(bad)

    def test_safe_id_valid(self):
        for good in ("GOLDEN_RUN", "EXP-L2-INRUSH-001", "abc123"):
            self.assertEqual(model.ensure_safe_id(good, kind="run_id"), good)

    def test_safe_id_rejects_path_or_dots(self):
        for bad in ("a/b", "a..b", "a.b", "/abs", ""):
            with self.assertRaises(model.UnsafeNameError):
                model.ensure_safe_id(bad, kind="run_id")


class TestCompleteness(unittest.TestCase):
    def _complete(self):
        na = "x"
        return {
            "setup": na,
            "fixture": na,
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
            "dut": {"description": na, "hardware_revision": na},
            "build_manifest": {
                k: na
                for k in [
                    "git_commit",
                    "toolchain",
                    "esp_idf_version",
                    "target",
                    "build_configuration_hash",
                    "artifact_sha256",
                    "tooling_version",
                    "measurement_tooling_commit",
                ]
            }
            | {"git_dirty": False},
            "conditions": {"temperature": na, "supply": na, "environment": na},
            "calibration": {"reference": na, "verified_at": na},
            "baseline": {"status": "approved", "record": na},
        }

    def test_complete_context_has_no_missing(self):
        self.assertEqual(model.missing_measured_fields(self._complete()), [])
        model.require_complete_context(self._complete())  # ne leve pas

    def test_missing_top_level_field(self):
        ctx = self._complete()
        del ctx["setup"]
        self.assertIn("setup", model.missing_measured_fields(ctx))

    def test_empty_subfield_is_missing(self):
        ctx = self._complete()
        ctx["instrument"]["model"] = "   "
        self.assertIn("instrument.model", model.missing_measured_fields(ctx))

    def test_require_raises_on_incomplete(self):
        ctx = self._complete()
        del ctx["calibration"]
        with self.assertRaises(model.IncompleteMetadataError):
            model.require_complete_context(ctx)


if __name__ == "__main__":
    unittest.main()

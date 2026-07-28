# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Validation de schema (sous-ensemble maison) + garde de profil de mots-cles."""

import unittest

from measurement.analysis import schema as schema_mod

_HEX = "a" * 64


def _valid_definition():
    return {
        "experiment_id": "EXP-X-001",
        "experimental_question": "q",
        "expected_observation": "o",
        "verdict_rule": "r",
        "protocol_ref": "p",
        "dec": "DEC-X-000",
        "dut": {"description": "d"},
        "acquisition": {"driver": "simulation"},
    }


def _minimal_manifest():
    return {
        "experiment_id": "EXP-X-001",
        "campaign_definition_id": _HEX,
        "definition_sha256": _HEX,
        "context_sha256": _HEX,
        "baseline_sha256": "N/A",
        "run_id": "r",
        "generated_at": "1970-01-01T00:00:00+00:00",
        "acquisition_nature": "simulated",
        "protocol_ref": "p",
        "dec": "DEC-X-000",
        "tooling_version": "0.1.0",
        "artifacts": [],
    }


_ALL_SCHEMAS = (
    "campaign-definition.schema.json",
    "execution-context.schema.json",
    "build-manifest.schema.json",
    "acquisition-manifest.schema.json",
    "evidence-state.schema.json",
    "evidence-event.schema.json",
    "archive-index.schema.json",
    "baseline-record.schema.json",
    "analysis-result.schema.json",
    "measurement-series.schema.json",
    "event-details-acquisition.schema.json",
    "event-details-review.schema.json",
    "event-details-verdict.schema.json",
    "event-details-promotion.schema.json",
)

_MANIFEST_SCHEMA = "acquisition-manifest.schema.json"


class TestSchema(unittest.TestCase):
    def test_valid_definition_passes(self):
        schema_mod.validate(
            _valid_definition(),
            schema_mod.load_schema("campaign-definition.schema.json"),
        )

    def test_valid_manifest_passes(self):
        schema_mod.validate(_minimal_manifest(), schema_mod.load_schema(_MANIFEST_SCHEMA))

    def test_missing_required_field_fails(self):
        bad = _valid_definition()
        del bad["verdict_rule"]
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(bad, schema_mod.load_schema("campaign-definition.schema.json"))

    def test_enum_is_enforced(self):
        manifest = _minimal_manifest()
        manifest["acquisition_nature"] = "invalid"
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(manifest, schema_mod.load_schema(_MANIFEST_SCHEMA))

    def test_additional_property_rejected(self):
        manifest = _minimal_manifest()
        manifest["surprise"] = 1
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(manifest, schema_mod.load_schema(_MANIFEST_SCHEMA))

    def test_pattern_is_enforced(self):
        manifest = _minimal_manifest()
        manifest["experiment_id"] = "EXP/BAD"
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(manifest, schema_mod.load_schema(_MANIFEST_SCHEMA))

    def test_sha256_pattern_is_enforced(self):
        manifest = _minimal_manifest()
        manifest["definition_sha256"] = "not-a-hash"
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(manifest, schema_mod.load_schema(_MANIFEST_SCHEMA))

    def test_minlength_is_enforced(self):
        manifest = _minimal_manifest()
        manifest["protocol_ref"] = ""
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(manifest, schema_mod.load_schema(_MANIFEST_SCHEMA))

    def test_bool_is_not_integer(self):
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(True, {"type": "integer"})

    def test_shipped_schemas_use_only_supported_keywords(self):
        for name in _ALL_SCHEMAS:
            schema = schema_mod.load_schema(name)
            extra = schema_mod.unsupported_keywords(schema)
            self.assertEqual(extra, set(), msg=f"{name}: mots-cles hors profil {extra}")


if __name__ == "__main__":
    unittest.main()

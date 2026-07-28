# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Validation de schema (sous-ensemble maison) + garde de profil de mots-cles."""

import unittest

from measurement.analysis import schema as schema_mod


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
        "campaign_definition_id": "id",
        "definition_sha256": "d",
        "context_sha256": "c",
        "run_id": "r",
        "generated_at": "t",
        "acquisition_nature": "simulated",
        "evidence_status": "S",
        "verdict": "NOT_RUN",
        "verdict_reason": "x",
        "protocol_ref": "p",
        "dec": "DEC-X-000",
        "tooling_version": "0.1.0",
        "artifacts": [],
    }


_ALL_SCHEMAS = (
    "campaign-definition.schema.json",
    "execution-context.schema.json",
    "build-manifest.schema.json",
    "run-manifest.schema.json",
    "measurement-series.schema.json",
    "verdict.schema.json",
)


class TestSchema(unittest.TestCase):
    def test_valid_definition_passes(self):
        schema_mod.validate(
            _valid_definition(),
            schema_mod.load_schema("campaign-definition.schema.json"),
        )

    def test_missing_required_field_fails(self):
        bad = _valid_definition()
        del bad["verdict_rule"]
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(bad, schema_mod.load_schema("campaign-definition.schema.json"))

    def test_enum_is_enforced(self):
        manifest = _minimal_manifest()
        manifest["acquisition_nature"] = "invalid"
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(manifest, schema_mod.load_schema("run-manifest.schema.json"))

    def test_additional_property_rejected(self):
        manifest = _minimal_manifest()
        manifest["surprise"] = 1
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(manifest, schema_mod.load_schema("run-manifest.schema.json"))

    def test_pattern_is_enforced(self):
        manifest = _minimal_manifest()
        manifest["experiment_id"] = "EXP/BAD"  # slash interdit par le motif
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(manifest, schema_mod.load_schema("run-manifest.schema.json"))

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

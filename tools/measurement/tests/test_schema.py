# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Validation de schema (sous-ensemble JSON Schema)."""

import unittest

from measurement.analysis import schema as schema_mod


class TestSchema(unittest.TestCase):
    def test_valid_definition_passes(self):
        definition = {
            "protocol_ref": "x",
            "dec": "DEC-X-000",
            "dut": {"description": "d"},
            "acquisition": {"driver": "simulation"},
        }
        schema_mod.validate(
            definition, schema_mod.load_schema("campaign-definition.schema.json")
        )

    def test_missing_required_field_fails(self):
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(
                {"dec": "DEC-X-000"},
                schema_mod.load_schema("campaign-definition.schema.json"),
            )

    def test_enum_is_enforced(self):
        manifest = _minimal_manifest()
        manifest["nature"] = "invalid"
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(manifest, schema_mod.load_schema("run-manifest.schema.json"))

    def test_additional_property_rejected(self):
        manifest = _minimal_manifest()
        manifest["surprise"] = 1
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(manifest, schema_mod.load_schema("run-manifest.schema.json"))

    def test_bool_is_not_integer(self):
        with self.assertRaises(schema_mod.SchemaError):
            schema_mod.validate(True, {"type": "integer"})


def _minimal_manifest():
    return {
        "campaign_definition_id": "id",
        "run_id": "r",
        "generated_at": "t",
        "nature": "simulated",
        "data_status": "S",
        "protocol_ref": "p",
        "dec": "DEC-X-000",
        "instrument": {},
        "firmware_under_test": "",
        "dut_hardware_revision": "",
        "conditions": {},
        "artifacts": [],
    }


if __name__ == "__main__":
    unittest.main()

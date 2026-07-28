# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Parite avec un validateur JSON Schema STANDARD (jsonschema).

Verifie que (1) chaque schema livre est un JSON Schema draft 2020-12 valide,
(2) les golden datasets sont conformes, (3) sur des exemples cures, le validateur
maison et le validateur standard donnent le MEME verdict (accept/reject).

Ignore si ``jsonschema`` n'est pas installe (present en CI via requirements-dev).
"""

import json
import unittest
from pathlib import Path

from measurement.analysis import schema as home

try:
    from jsonschema import Draft202012Validator

    _HAS_JSONSCHEMA = True
except ImportError:
    _HAS_JSONSCHEMA = False

_ROOT = Path(__file__).resolve().parents[1]
_SCHEMAS = _ROOT / "schemas"
_GOLDEN = _ROOT / "golden"

_ALL_SCHEMAS = (
    "campaign-definition.schema.json",
    "execution-context.schema.json",
    "build-manifest.schema.json",
    "run-manifest.schema.json",
    "measurement-series.schema.json",
    "verdict.schema.json",
)


def _home_accepts(instance, schema) -> bool:
    try:
        home.validate(instance, schema)
        return True
    except home.SchemaError:
        return False


def _std_accepts(instance, schema) -> bool:
    return Draft202012Validator(schema).is_valid(instance)


@unittest.skipUnless(_HAS_JSONSCHEMA, "jsonschema non installe")
class TestSchemaParity(unittest.TestCase):
    def test_schemas_are_valid_json_schema(self):
        for name in _ALL_SCHEMAS:
            schema = home.load_schema(name)
            Draft202012Validator.check_schema(schema)  # leve si invalide

    def test_golden_definition_conforms_both(self):
        definition = json.loads(
            (_GOLDEN / "campaign-definition.json").read_text(encoding="utf-8")
        )
        schema = home.load_schema("campaign-definition.schema.json")
        self.assertTrue(_home_accepts(definition, schema))
        self.assertTrue(_std_accepts(definition, schema))

    def test_parity_on_curated_cases(self):
        defn = home.load_schema("campaign-definition.schema.json")
        man = home.load_schema("run-manifest.schema.json")
        good_def = json.loads(
            (_GOLDEN / "campaign-definition.json").read_text(encoding="utf-8")
        )
        bad_pattern = dict(good_def, experiment_id="EXP/BAD")
        missing = {k: v for k, v in good_def.items() if k != "verdict_rule"}
        good_manifest = json.loads(
            (_GOLDEN / "expected" / "manifest.normalized.json").read_text(encoding="utf-8")
        )
        good_manifest = dict(good_manifest, run_id="R1", generated_at="T1")
        bad_enum = dict(good_manifest, acquisition_nature="nope")

        cases = [
            (good_def, defn, True),
            (bad_pattern, defn, False),
            (missing, defn, False),
            (good_manifest, man, True),
            (bad_enum, man, False),
        ]
        for instance, schema, expected in cases:
            self.assertEqual(_home_accepts(instance, schema), expected)
            self.assertEqual(_std_accepts(instance, schema), expected)


if __name__ == "__main__":
    unittest.main()

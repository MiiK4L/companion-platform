# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Parite avec un validateur JSON Schema STANDARD (jsonschema).

(1) chaque schema livre est un JSON Schema draft 2020-12 valide ; (2) les golden
sont conformes ; (3) sur des exemples cures, validateur maison et standard
donnent le MEME verdict. Ignore si jsonschema absent (present en CI)."""

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
_GOLDEN = _ROOT / "golden"

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
    "capture.schema.json",
    "import-descriptor.schema.json",
    "event-details-acquisition.schema.json",
    "event-details-review.schema.json",
    "event-details-verdict.schema.json",
    "event-details-promotion.schema.json",
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
            Draft202012Validator.check_schema(home.load_schema(name))

    def test_golden_definition_conforms_both(self):
        definition = json.loads(
            (_GOLDEN / "campaign-definition.json").read_text(encoding="utf-8")
        )
        schema = home.load_schema("campaign-definition.schema.json")
        self.assertTrue(_home_accepts(definition, schema))
        self.assertTrue(_std_accepts(definition, schema))

    def test_parity_on_curated_cases(self):
        defn = home.load_schema("campaign-definition.schema.json")
        man = home.load_schema("acquisition-manifest.schema.json")
        good_def = json.loads(
            (_GOLDEN / "campaign-definition.json").read_text(encoding="utf-8")
        )
        bad_pattern = dict(good_def, experiment_id="EXP/BAD")
        missing = {k: v for k, v in good_def.items() if k != "verdict_rule"}
        good_manifest = json.loads(
            (_GOLDEN / "expected" / "manifest.normalized.json").read_text(encoding="utf-8")
        )
        good_manifest = dict(
            good_manifest, run_id="GOLDEN_RUN", generated_at="1970-01-01T00:00:00+00:00"
        )
        bad_enum = dict(good_manifest, acquisition_nature="nope")
        bad_sha = dict(good_manifest, definition_sha256="zzz")

        cases = [
            (good_def, defn, True),
            (bad_pattern, defn, False),
            (missing, defn, False),
            (good_manifest, man, True),
            (bad_enum, man, False),
            (bad_sha, man, False),
        ]
        for instance, schema, expected in cases:
            self.assertEqual(_home_accepts(instance, schema), expected)
            self.assertEqual(_std_accepts(instance, schema), expected)


if __name__ == "__main__":
    unittest.main()

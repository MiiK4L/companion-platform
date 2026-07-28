# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Primitives communes : canonique, hachage, identifiants."""

import unittest

from measurement.common.canonical import canonical_json
from measurement.common.hashing import sha256_text
from measurement.common.ids import campaign_definition_id, new_run_id


class TestCanonical(unittest.TestCase):
    def test_key_order_independent(self):
        self.assertEqual(
            canonical_json({"b": 1, "a": 2}),
            canonical_json({"a": 2, "b": 1}),
        )


class TestHashing(unittest.TestCase):
    def test_stable(self):
        self.assertEqual(sha256_text("abc"), sha256_text("abc"))
        self.assertNotEqual(sha256_text("abc"), sha256_text("abd"))


class TestIds(unittest.TestCase):
    def test_definition_id_is_deterministic(self):
        definition = {"a": 1, "nested": {"x": [1, 2, 3]}}
        self.assertEqual(
            campaign_definition_id(definition),
            campaign_definition_id(dict(definition)),
        )

    def test_definition_id_changes_with_content(self):
        self.assertNotEqual(
            campaign_definition_id({"a": 1}),
            campaign_definition_id({"a": 2}),
        )

    def test_run_id_is_unique(self):
        self.assertNotEqual(new_run_id(), new_run_id())


if __name__ == "__main__":
    unittest.main()

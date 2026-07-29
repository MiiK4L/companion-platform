# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Definitions de campagne EXP-L1-BRINGUP-001 (lot B3, DEFINITION SEULE).

Verifie que les six definitions sont conformes au schema, deterministes et
DISTINCTES, et surtout que la MATRICE D'APPLICABILITE est coherente : un mode
isole ne doit jamais etre juge sur les criteres de degradation concurrente, et
un critere non applicable ne doit pas etre exige implicitement.

Aucune execution, aucune acquisition : ces definitions ne produisent rien."""

import json
import unittest
from pathlib import Path

from measurement.analysis import schema as home
from measurement.common.ids import campaign_definition_id

_ROOT = Path(__file__).resolve().parents[1]
_DEFS = _ROOT / "campaigns" / "EXP-L1-BRINGUP-001"
_SCHEMA = _ROOT / "schemas" / "campaign-definition.schema.json"

# Criteres de degradation concurrente : sans concurrence, ils n'ont pas de sens.
_DEGRADATION_BL = {"BL-104", "BL-105"}
# Invariants de validite du banc : applicables a TOUS les cas.
_BENCH_INVARIANTS = {
    "BL-001", "BL-002", "BL-003", "BL-004", "BL-005", "BL-006",
    "BL-007", "BL-008", "BL-009", "BL-010", "BL-011", "BL-012",
}
_EXPECTED_CASES = {
    ("spi-shared", "screen-only"),
    ("spi-shared", "module-only"),
    ("spi-shared", "concurrent"),
    ("spi-separated", "screen-only"),
    ("spi-separated", "module-only"),
    ("spi-separated", "concurrent"),
}


def _load():
    return {f.name: json.loads(f.read_text(encoding="utf-8"))
            for f in sorted(_DEFS.glob("*.json"))}


class L1CampaignDefinitionsTest(unittest.TestCase):
    def setUp(self):
        self.docs = _load()

    def test_six_definitions_conformes_au_schema(self):
        schema = json.loads(_SCHEMA.read_text(encoding="utf-8"))
        self.assertEqual(len(self.docs), 6, "six configurations de run attendues")
        for name, doc in self.docs.items():
            with self.subTest(definition=name):
                # validate() leve SchemaError si non conforme ; ne retourne rien.
                home.validate(doc, schema)

    def test_couverture_des_cas(self):
        cases = {(d["variant_id"], d["parameters"]["load_mode"])
                 for d in self.docs.values()}
        self.assertEqual(cases, _EXPECTED_CASES, "2 topologies x 3 modes")

    def test_identifiants_deterministes_et_distincts(self):
        ids = {n: campaign_definition_id(d) for n, d in self.docs.items()}
        self.assertEqual(len(set(ids.values())), 6, "identifiants distincts")
        again = {n: campaign_definition_id(d) for n, d in _load().items()}
        self.assertEqual(ids, again, "identifiants deterministes")

    def test_matrice_applicabilite_coherente(self):
        for name, doc in self.docs.items():
            params = doc["parameters"]
            applicable = set(params["applicable_bl"])
            not_applicable = set(params["not_applicable_bl"])
            with self.subTest(definition=name):
                self.assertTrue(applicable, "au moins un critere applicable")
                self.assertFalse(applicable & not_applicable,
                                 "un critere ne peut etre a la fois applicable et N/A")
                self.assertTrue(_BENCH_INVARIANTS <= applicable,
                                "les invariants de banc s'appliquent a tous les cas")
                # Les metriques suivent la meme discipline.
                required = set(params["required_metrics"])
                na_metrics = set(params["not_applicable_metrics"])
                self.assertTrue(required, "au moins une metrique obligatoire")
                self.assertFalse(required & na_metrics,
                                 "une metrique ne peut etre a la fois requise et N/A")

    def test_modes_isoles_non_juges_sur_la_degradation(self):
        """Un mode isole n'a pas de concurrence : BL-104/BL-105 doivent etre N/A."""
        for name, doc in self.docs.items():
            params = doc["parameters"]
            mode = params["load_mode"]
            applicable = set(params["applicable_bl"])
            not_applicable = set(params["not_applicable_bl"])
            with self.subTest(definition=name, mode=mode):
                if mode == "concurrent":
                    self.assertTrue(_DEGRADATION_BL <= applicable,
                                    "concurrent : criteres de degradation applicables")
                else:
                    self.assertFalse(_DEGRADATION_BL & applicable,
                                     "mode isole : degradation NON applicable")
                    self.assertTrue(_DEGRADATION_BL <= not_applicable,
                                    "mode isole : degradation explicitement N/A")

    def test_metriques_absentes_du_mode(self):
        """screen-only n'a pas d'IRQ module ; module-only n'a pas de trafic ecran."""
        for name, doc in self.docs.items():
            params = doc["parameters"]
            mode = params["load_mode"]
            required = set(params["required_metrics"])
            with self.subTest(definition=name, mode=mode):
                if mode == "screen-only":
                    self.assertNotIn("M-IRQL", required, "pas d'IRQ module exploitable")
                    self.assertFalse({m for m in required if "(module)" in m},
                                     "aucune metrique module en screen-only")
                elif mode == "module-only":
                    self.assertFalse({m for m in required if "(screen)" in m},
                                     "aucune metrique ecran en module-only")
                    self.assertIn("M-IRQL", required, "IRQ module mesurable")

    def test_criteres_crc_distincts(self):
        """Rejets CRC et trames corrompues ACCEPTEES sont deux criteres separes."""
        for name, doc in self.docs.items():
            applicable = set(doc["parameters"]["applicable_bl"])
            with self.subTest(definition=name):
                self.assertIn("BL-101", applicable, "rejets CRC inattendus")
                self.assertIn("BL-107", applicable, "trames corrompues acceptees")

    def test_lien_vers_la_baseline_brouillon(self):
        for name, doc in self.docs.items():
            params = doc["parameters"]
            with self.subTest(definition=name):
                self.assertEqual(params["baseline_id"], "BL-EXP-L1-BRINGUP-001")
                self.assertEqual(params["baseline_draft_version"], 0,
                                 "brouillon : version 0 (l'approuvee sera la 1 en B4)")
                self.assertTrue(params["baseline_draft_ref"].endswith("baseline-draft.md"))
                self.assertIn("draft", params["baseline_status"])

    def test_base_de_temps_autoritaire_declaree(self):
        for name, doc in self.docs.items():
            with self.subTest(definition=name):
                self.assertEqual(doc["parameters"]["time_authority"], "logic-analyzer",
                                 "les latences inter-cartes ne sont pas datees par les MCU")

    def test_definition_seule_aucune_acquisition(self):
        for name, doc in self.docs.items():
            with self.subTest(definition=name):
                self.assertEqual(doc["acquisition"]["driver"], "manual-import")
                self.assertIn("aucun run", doc["acquisition"]["config"]["note"])


if __name__ == "__main__":
    unittest.main()

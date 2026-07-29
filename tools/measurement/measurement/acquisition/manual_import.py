# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Driver d'acquisition MANUEL de reference du socle.

Importe une acquisition realisee manuellement (analyseur, oscilloscope, dump
serie, image...) depuis un REPERTOIRE d'import ephemere (fourni au run, jamais
archive) : bruts par famille, descripteur de capture et series normalisees.

GENERIQUE : aucune logique metier (ni SPI/CX-Bus, ni verdict, ni [BL]). Il ne
fait que decouvrir, valider structurellement et transmettre les artefacts au
socle, qui les copie et les hashe. Il DEFINIT l'interface d'acquisition de
reference : un futur driver automatique produira les memes artefacts sans
modifier schemas, verify_run, rapports ni analyse.
"""

from __future__ import annotations

import csv
import json
from pathlib import Path
from typing import Any

from ..analysis import schema as schema_mod
from ..model import ensure_filename
from .driver import InstrumentDriver, register_driver


def _descriptor(config: dict[str, Any]) -> tuple[Path, dict[str, Any]]:
    if "import_dir" not in config:
        raise ValueError("manual-import : 'import_dir' requis (parametre de run)")
    import_dir = Path(config["import_dir"])
    descriptor = json.loads((import_dir / "import.json").read_text(encoding="utf-8"))
    schema_mod.validate(descriptor, schema_mod.load_schema("import-descriptor.schema.json"))
    return import_dir, descriptor


def _read_rows(path: Path) -> list[list[str]]:
    with open(path, encoding="utf-8", newline="") as handle:
        rows = list(csv.reader(handle))
    return [list(row) for row in rows[1:]]  # saute l'en-tete


@register_driver("manual-import")
class ManualImportDriver(InstrumentDriver):
    """Import manuel : bruts + capture + series depuis un repertoire ephemere."""

    nature = "measured"

    def acquire(self, definition_id: str, config: dict[str, Any]) -> list[dict[str, Any]]:
        import_dir, descriptor = _descriptor(config)
        series: list[dict[str, Any]] = []
        for entry in descriptor["series"]:
            csv_name = ensure_filename(entry["csv"])
            rows = _read_rows(import_dir / csv_name)
            series.append({"name": entry["name"], "columns": entry["columns"], "rows": rows})
        return series

    def capture(self, definition_id: str, config: dict[str, Any]) -> dict[str, Any]:
        import_dir, descriptor = _descriptor(config)
        raw = [
            {
                "group": ensure_filename(item["group"]),
                "name": ensure_filename(item["name"]),
                "format": item["format"],
                "path": str(
                    import_dir
                    / "raw"
                    / ensure_filename(item["group"])
                    / ensure_filename(item["name"])
                ),
            }
            for item in descriptor["raw_artifacts"]
        ]
        return {
            "capture_id": descriptor["capture_id"],
            "capture_type": descriptor["capture_type"],
            "parameters": descriptor["parameters"],
            "raw": raw,
            "normalized": descriptor["normalized"],
        }

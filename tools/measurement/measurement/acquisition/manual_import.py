# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Driver d'acquisition MANUEL de reference du socle.

Importe une acquisition realisee manuellement (analyseur, oscilloscope, dump
serie, image...) depuis un REPERTOIRE d'import ephemere (fourni au run, jamais
archive). L'entree est figee en UN SNAPSHOT immuable dans ``prepare()`` : lecture,
validation, inventaire, hachage et verification des relations une seule fois ;
``acquire()`` et ``capture()`` ne relisent jamais l'entree. Tout changement de
l'entree apres ``prepare()`` est donc sans effet sur l'archive (vue figee).

GENERIQUE : aucune logique metier (ni SPI/CX-Bus, ni verdict, ni [BL]). Definit
l'interface d'acquisition de reference ; un futur driver automatique produira les
memes artefacts sans modifier schemas, verify_run, rapports ni analyse.
"""

from __future__ import annotations

import csv
import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from ..analysis import schema as schema_mod
from ..model import ensure_filename
from .driver import InstrumentDriver, register_driver


class ManualImportError(ValueError):
    """Erreur d'import manuel (repertoire/descripteur/fichier invalide)."""


@dataclass(frozen=True)
class _Snapshot:
    series: list[dict[str, Any]]
    raw: list[dict[str, Any]]  # {group, name, format, content: bytes}
    capture_id: str
    capture_type: str
    parameters: dict[str, Any]
    normalized: list[dict[str, str]]


def _require_regular_within(base: Path, path: Path, label: str) -> None:
    """Refuse symlink / fichier special / repertoire / evasion hors de ``base``."""
    if path.is_symlink():
        raise ManualImportError(f"{label}: lien symbolique refuse")
    if not path.exists():
        raise ManualImportError(f"{label}: fichier absent")
    if not path.is_file():
        raise ManualImportError(
            f"{label}: fichier regulier requis (special/repertoire refuse)"
        )
    resolved = path.resolve()
    if resolved != base and base not in resolved.parents:
        raise ManualImportError(f"{label}: hors du repertoire d'import")


def _parse_csv(path: Path, columns: list[dict[str, str]]) -> list[list[str]]:
    """Parse un CSV normalise UTF-8 : en-tete == colonnes declarees, largeur
    constante, pas de ligne vide."""
    expected = [column["name"] for column in columns]
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError as error:
        raise ManualImportError(f"CSV {path.name}: encodage UTF-8 invalide") from error
    reader = list(csv.reader(text.splitlines()))
    if not reader:
        raise ManualImportError(f"CSV {path.name}: vide")
    header = reader[0]
    if header != expected:
        raise ManualImportError(f"CSV {path.name}: en-tete {header} != colonnes {expected}")
    rows: list[list[str]] = []
    for index, row in enumerate(reader[1:], start=2):
        if len(row) != len(expected):
            raise ManualImportError(
                f"CSV {path.name}: ligne {index} de largeur {len(row)} != {len(expected)}"
            )
        rows.append(list(row))
    return rows


def _build_snapshot(config: dict[str, Any]) -> _Snapshot:
    if "import_dir" not in config:
        raise ManualImportError("manual-import : 'import_dir' requis (parametre de run)")
    import_dir = Path(config["import_dir"])
    if not import_dir.is_dir():
        raise ManualImportError("import_dir inexistant ou non repertoire")
    base = import_dir.resolve()

    descriptor_path = import_dir / "import.json"
    _require_regular_within(base, descriptor_path, "import.json")
    descriptor = json.loads(descriptor_path.read_text(encoding="utf-8"))
    schema_mod.validate(descriptor, schema_mod.load_schema("import-descriptor.schema.json"))

    # Preflight : unicite (bruts, series, CSV) et references de tracabilite.
    raw_keys, series_names, csv_names = set(), set(), set()
    for item in descriptor["raw_artifacts"]:
        key = f"{ensure_filename(item['group'])}/{ensure_filename(item['name'])}"
        if key in raw_keys:
            raise ManualImportError(f"brut duplique: {key}")
        raw_keys.add(key)
    for entry in descriptor["series"]:
        if entry["name"] in series_names:
            raise ManualImportError(f"serie dupliquee: {entry['name']}")
        series_names.add(entry["name"])
        csv_name = ensure_filename(entry["csv"])
        if csv_name in csv_names:
            raise ManualImportError(f"fichier CSV partage non autorise: {csv_name}")
        csv_names.add(csv_name)
    provenance = {}
    for norm in descriptor["normalized"]:
        if norm["series"] not in series_names:
            raise ManualImportError(f"provenance vers serie inconnue: {norm['series']}")
        if norm["series"] in provenance:
            raise ManualImportError(f"provenance multiple pour la serie: {norm['series']}")
        if norm["from_raw"] not in raw_keys:
            raise ManualImportError(f"provenance vers brut inexistant: {norm['from_raw']}")
        provenance[norm["series"]] = norm["from_raw"]
    missing = series_names - set(provenance)
    if missing:
        raise ManualImportError(f"series sans provenance: {sorted(missing)}")

    # Mode strict : aucun brut present mais non declare (pas d'oubli silencieux).
    raw_root = import_dir / "raw"
    if raw_root.is_dir():
        for path in raw_root.rglob("*"):
            if path.is_symlink() or (path.is_file()):
                rel = path.relative_to(raw_root).as_posix()
                if path.is_symlink():
                    raise ManualImportError(f"brut non declare / lien symbolique: raw/{rel}")
                if rel not in raw_keys:
                    raise ManualImportError(
                        f"brut present mais non declare (mode strict): raw/{rel}"
                    )

    # Lecture figee : bruts (octets) et series (lignes).
    raw: list[dict[str, Any]] = []
    for item in descriptor["raw_artifacts"]:
        group, name = ensure_filename(item["group"]), ensure_filename(item["name"])
        source = import_dir / "raw" / group / name
        _require_regular_within(base, source, f"raw/{group}/{name}")
        raw.append(
            {
                "group": group,
                "name": name,
                "format": item["format"],
                "content": source.read_bytes(),
            }
        )
    series: list[dict[str, Any]] = []
    for entry in descriptor["series"]:
        csv_path = import_dir / ensure_filename(entry["csv"])
        _require_regular_within(base, csv_path, f"csv/{entry['csv']}")
        series.append(
            {
                "name": entry["name"],
                "columns": entry["columns"],
                "rows": _parse_csv(csv_path, entry["columns"]),
            }
        )
    return _Snapshot(
        series=series,
        raw=raw,
        capture_id=descriptor["capture_id"],
        capture_type=descriptor["capture_type"],
        parameters=descriptor["parameters"],
        normalized=descriptor["normalized"],
    )


@register_driver("manual-import")
class ManualImportDriver(InstrumentDriver):
    """Import manuel : bruts + capture + series depuis un repertoire ephemere."""

    nature = "measured"

    def __init__(self) -> None:
        self._snapshot: _Snapshot | None = None

    def prepare(self, definition_id: str, config: dict[str, Any]) -> None:
        self._snapshot = _build_snapshot(config)

    def _snap(self) -> _Snapshot:
        if self._snapshot is None:
            raise ManualImportError("manual-import : prepare() non appele")
        return self._snapshot

    def acquire(self, definition_id: str, config: dict[str, Any]) -> list[dict[str, Any]]:
        snap = self._snap()
        return [dict(s) for s in snap.series]

    def capture(self, definition_id: str, config: dict[str, Any]) -> dict[str, Any]:
        snap = self._snap()
        raw = [
            {
                "group": r["group"],
                "name": r["name"],
                "format": r["format"],
                "content": r["content"],
            }
            for r in snap.raw
        ]
        return {
            "capture_id": snap.capture_id,
            "capture_type": snap.capture_type,
            "parameters": snap.parameters,
            "raw": raw,
            "normalized": snap.normalized,
        }

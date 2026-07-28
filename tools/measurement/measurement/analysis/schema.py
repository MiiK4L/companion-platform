# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Validation de schema — sous-ensemble de JSON Schema, sans dependance externe.

Les CONTRATS de donnees sont les fichiers ``*.schema.json`` (standard ouvert,
validables par tout outil JSON Schema). Ce module en valide le sous-ensemble
utilise par le socle (type, required, properties, enum, additionalProperties,
items) afin de rester leger et hermetique en CI. Le coeur des formats ne depend
d'aucun framework de test.
"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

SCHEMAS_DIR = Path(__file__).resolve().parents[2] / "schemas"

_TYPE_MAP: dict[str, type | tuple[type, ...]] = {
    "object": dict,
    "array": list,
    "string": str,
    "number": (int, float),
    "integer": int,
    "boolean": bool,
    "null": type(None),
}


class SchemaError(Exception):
    """Instance non conforme au schema."""


def load_schema(name: str) -> dict[str, Any]:
    """Charge un schema par nom de fichier (relatif a ``schemas/``)."""
    return json.loads((SCHEMAS_DIR / name).read_text(encoding="utf-8"))


def validate(instance: Any, schema: dict[str, Any], path: str = "$") -> None:
    """Valide ``instance`` contre ``schema`` ; leve ``SchemaError`` sinon."""
    declared = schema.get("type")
    if declared is not None:
        expected = _TYPE_MAP[declared]
        # bool est un sous-type de int en Python : on l'exclut explicitement.
        if declared in ("integer", "number") and isinstance(instance, bool):
            raise SchemaError(f"{path}: booleen n'est pas un {declared}")
        if not isinstance(instance, expected):
            raise SchemaError(f"{path}: type attendu {declared!r}")

    if "enum" in schema and instance not in schema["enum"]:
        raise SchemaError(f"{path}: valeur hors enum {schema['enum']}")

    if isinstance(instance, dict) and (declared == "object" or "properties" in schema):
        props = schema.get("properties", {})
        for required in schema.get("required", []):
            if required not in instance:
                raise SchemaError(f"{path}: champ requis manquant {required!r}")
        if schema.get("additionalProperties") is False:
            for key in instance:
                if key not in props:
                    raise SchemaError(f"{path}: propriete non autorisee {key!r}")
        for key, sub in props.items():
            if key in instance:
                validate(instance[key], sub, f"{path}.{key}")

    if isinstance(instance, list) and "items" in schema:
        for index, item in enumerate(instance):
            validate(item, schema["items"], f"{path}[{index}]")

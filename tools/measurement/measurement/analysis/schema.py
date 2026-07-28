# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Validation de schema — sous-ensemble DOCUMENTE de JSON Schema, sans
dependance externe.

Les CONTRATS sont les fichiers ``*.schema.json`` (JSON Schema draft 2020-12,
validables par tout outil standard). Ce module valide le PROFIL de mots-cles
ci-dessous, afin de rester leger et hermetique en CI. Un validateur STANDARD
(jsonschema) est employe en CI pour verifier les schemas eux-memes, les golden
datasets, et la PARITE avec ce validateur (cf. tests/test_schema_parity.py).

PROFIL SUPPORTE (mots-cles de validation) : ``type``, ``enum``, ``required``,
``properties``, ``additionalProperties``, ``items``, ``pattern``. Mots-cles
d'annotation ignores (autorises) : ``$schema``, ``title``, ``description``.
Tout autre mot-cle est signale par :func:`unsupported_keywords`.
"""

from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any

SCHEMAS_DIR = Path(__file__).resolve().parents[2] / "schemas"

#: Mots-cles reellement pris en charge par la validation.
VALIDATION_KEYWORDS = frozenset(
    {
        "type",
        "enum",
        "required",
        "properties",
        "additionalProperties",
        "items",
        "pattern",
        "minLength",
        "minItems",
    }
)
#: Mots-cles d'annotation autorises mais non validants.
ANNOTATION_KEYWORDS = frozenset({"$schema", "title", "description"})
#: Profil complet accepte sans avertissement.
SUPPORTED_KEYWORDS = VALIDATION_KEYWORDS | ANNOTATION_KEYWORDS

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

    if "pattern" in schema and isinstance(instance, str):
        if not re.search(schema["pattern"], instance):
            raise SchemaError(f"{path}: chaine hors motif {schema['pattern']!r}")

    if "minLength" in schema and isinstance(instance, str):
        if len(instance) < schema["minLength"]:
            raise SchemaError(f"{path}: chaine trop courte (< {schema['minLength']})")

    if "minItems" in schema and isinstance(instance, list):
        if len(instance) < schema["minItems"]:
            raise SchemaError(f"{path}: liste trop courte (< {schema['minItems']})")

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


def unsupported_keywords(schema: Any) -> set[str]:
    """Retourne l'ensemble des mots-cles hors profil utilises dans ``schema``.

    Permet a la CI de detecter tout mot-cle qui serait IGNORE (donc non
    applique) par ce validateur leger.
    """
    found: set[str] = set()

    def walk(node: Any) -> None:
        if not isinstance(node, dict):
            return
        # Les cles d'un noeud de schema sont des mots-cles (hors sous-schemas).
        for key in node:
            if key not in SUPPORTED_KEYWORDS:
                found.add(key)
        # Recurse UNIQUEMENT dans les positions de sous-schemas (pas les noms de
        # proprietes, ni les valeurs d'enum, qui sont des donnees).
        props = node.get("properties")
        if isinstance(props, dict):
            for sub in props.values():
                walk(sub)
        items = node.get("items")
        if isinstance(items, dict):
            walk(items)
        additional = node.get("additionalProperties")
        if isinstance(additional, dict):
            walk(additional)

    walk(schema)
    return found

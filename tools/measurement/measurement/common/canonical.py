# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Serialisation canonique deterministe (cles triees, separateurs fixes).

Toute donnee destinee au hachage ou a la comparaison de determinisme passe
par ici, afin que des entrees identiques produisent des octets identiques.
"""

from __future__ import annotations

import json
from typing import Any


def canonical_json(obj: Any) -> str:
    """Retourne une representation JSON canonique et stable de ``obj``."""
    return json.dumps(
        obj,
        sort_keys=True,
        separators=(",", ":"),
        ensure_ascii=False,
    )


def canonical_bytes(obj: Any) -> bytes:
    """Version octets (UTF-8) de :func:`canonical_json`."""
    return canonical_json(obj).encode("utf-8")

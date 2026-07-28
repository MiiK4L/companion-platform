# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Identifiants de campagne.

Deux notions distinctes (voir docs/phase-1/experiment-platform/
measurement-data-model.md) :

- ``campaign_definition_id`` : DETERMINISTE, derive de la definition canonique
  (protocole, DUT, parametres, acquisition). Memes entrees -> meme identifiant.
- ``run_id`` : UNIQUE a chaque execution reelle (volatil, non reproductible).
"""

from __future__ import annotations

import uuid
from typing import Any

from .canonical import canonical_bytes
from .hashing import sha256_bytes

#: Champs exclus de toute comparaison de determinisme.
VOLATILE_FIELDS = ("run_id", "generated_at")


def campaign_definition_id(definition: dict[str, Any]) -> str:
    """Identifiant deterministe d'une definition de campagne.

    La definition ne contient aucun champ volatil ; l'identifiant est le
    hachage SHA-256 de sa forme canonique.
    """
    return sha256_bytes(canonical_bytes(definition))


def new_run_id() -> str:
    """Identifiant unique d'execution (non reproductible)."""
    return uuid.uuid4().hex

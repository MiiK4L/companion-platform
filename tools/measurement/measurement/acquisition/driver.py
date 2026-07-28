# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Interface de driver d'instrument + registre (architecture plugin).

Le coeur ne connait que l'interface ``InstrumentDriver``, jamais un instrument
concret. Ajouter un adaptateur = l'enregistrer via ``@register_driver`` sans
modifier orchestration/analyse/reporting.

Aucun pilote reel n'est fourni a ce stade : voir ``simulation.py`` (stub
deterministe). ``nature`` distingue une acquisition ``measured`` (reelle) d'une
acquisition ``simulated`` (outillage uniquement) — garde-fou applique en aval.
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from collections.abc import Callable
from typing import Any

_REGISTRY: dict[str, type[InstrumentDriver]] = {}


def register_driver(name: str) -> Callable[[type], type]:
    """Decorateur enregistrant un adaptateur de driver sous ``name``."""

    def decorator(cls: type) -> type:
        if name in _REGISTRY:
            raise ValueError(f"driver deja enregistre: {name}")
        _REGISTRY[name] = cls
        return cls

    return decorator


def get_driver(name: str) -> type[InstrumentDriver]:
    """Retourne la classe de driver enregistree sous ``name``."""
    if name not in _REGISTRY:
        raise KeyError(f"driver inconnu: {name!r} (connus: {available_drivers()})")
    return _REGISTRY[name]


def available_drivers() -> list[str]:
    """Liste triee des drivers enregistres."""
    return sorted(_REGISTRY)


class InstrumentDriver(ABC):
    """Contrat minimal d'un driver d'acquisition.

    ``nature`` vaut ``"measured"`` (instrument reel) ou ``"simulated"``.
    """

    nature: str = "measured"

    @abstractmethod
    def acquire(self, definition_id: str, config: dict[str, Any]) -> list[dict[str, Any]]:
        """Retourne une liste de series conformes au schema de serie.

        Chaque serie : ``{"name": str, "columns": [{"name","unit"}], "rows": [...]}``.
        """
        raise NotImplementedError

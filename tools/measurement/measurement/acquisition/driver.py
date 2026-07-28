# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Interface de driver d'instrument + registre STATIQUE.

Le coeur ne connait que l'interface ``InstrumentDriver``, jamais un instrument
concret. Registre STATIQUE et honnete (pas de decouverte dynamique) : ajouter un
adaptateur = l'importer dans le paquet ``acquisition`` (via ``@register_driver``)
pour qu'il s'enregistre ; l'orchestrateur et la CI, eux, restent inchanges.

Aucun pilote reel n'est fourni a ce stade : voir ``simulation.py``.

``nature`` doit etre declaree EXPLICITEMENT par chaque driver (``"measured"`` ou
``"simulated"``) : aucune valeur par defaut. Un driver ``measured`` produit des
donnees BRUTES (statut RAW), jamais directement une preuve [M].
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from collections.abc import Callable
from typing import Any

from ..model import ACQUISITION_NATURES

_REGISTRY: dict[str, type[InstrumentDriver]] = {}


def register_driver(name: str) -> Callable[[type], type]:
    """Decorateur enregistrant un adaptateur de driver sous ``name``.

    Refuse un driver dont ``nature`` n'est pas declaree explicitement.
    """

    def decorator(cls: type) -> type:
        if name in _REGISTRY:
            raise ValueError(f"driver deja enregistre: {name}")
        nature = getattr(cls, "nature", None)
        if nature not in ACQUISITION_NATURES:
            raise ValueError(
                f"le driver {cls.__name__!r} doit declarer nature parmi "
                f"{ACQUISITION_NATURES} (trouve: {nature!r})"
            )
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

    Chaque sous-classe DOIT declarer ``nature`` (``"measured"`` ou
    ``"simulated"``). Aucune valeur par defaut.
    """

    #: A declarer explicitement par le driver : "measured" ou "simulated".
    nature: str

    @abstractmethod
    def acquire(self, definition_id: str, config: dict[str, Any]) -> list[dict[str, Any]]:
        """Retourne une liste de series conformes au schema de serie.

        Chaque serie : ``{"name": str, "columns": [{"name","unit"}], "rows": [...]}``.
        """
        raise NotImplementedError

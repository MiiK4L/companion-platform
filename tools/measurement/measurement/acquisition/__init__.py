# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Couche acquisition : interface de driver, registre plugin, simulation.

L'import de ce paquet enregistre les drivers fournis (ici : la simulation).
Aucun pilote d'instrument reel n'est fourni a ce stade.
"""

from . import simulation  # noqa: F401  (effet de bord : enregistre le driver)
from .driver import (
    InstrumentDriver,
    available_drivers,
    get_driver,
    register_driver,
)

__all__ = [
    "InstrumentDriver",
    "available_drivers",
    "get_driver",
    "register_driver",
]

# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Driver de simulation — captures factices DETERMINISTES (sans materiel).

Sert uniquement a valider l'infrastructure (orchestration, formats, schemas,
rapports, reproductibilite logicielle). GARDE-FOU : ``nature = "simulated"`` ;
en aval, une donnee simulee ne peut jamais etre marquee ``measured`` ([M]).

Les valeurs sont derivees du hachage de (definition_id, index) : aucune source
d'alea non controlee, donc memes entrees -> memes octets.
"""

from __future__ import annotations

from typing import Any

from ..common.hashing import sha256_text
from .driver import InstrumentDriver, register_driver

_UINT32_MAX = 0xFFFFFFFF


@register_driver("simulation")
class SimulationDriver(InstrumentDriver):
    """Produit une serie deterministe a partir de l'identifiant de definition."""

    nature = "simulated"

    def acquire(self, definition_id: str, config: dict[str, Any]) -> list[dict[str, Any]]:
        samples = int(config.get("samples", 8))
        series_name = str(config.get("series", "signal"))
        rows: list[list[float]] = []
        for index in range(samples):
            seed = sha256_text(f"{definition_id}:{index}")
            value = int(seed[:8], 16) / _UINT32_MAX
            rows.append([index, round(value, 6)])
        return [
            {
                "name": series_name,
                "columns": [
                    {"name": "index", "unit": "sample"},
                    {"name": "value", "unit": "a.u."},
                ],
                "rows": rows,
            }
        ]

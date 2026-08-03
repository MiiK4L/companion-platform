# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Histogramme de latence : VUE DERIVEE, jamais autoritaire.

L'histogramme recalcule ICI, depuis la serie brute, fait foi. Celui eventuellement
produit par le MCU est SECONDAIRE : il est compare a ce recalcul, et toute
divergence est un defaut d'instrumentation ou d'implementation a investiguer,
pas un resultat a arbitrer.

Convention de classe, identique au firmware : ``[edges[i], edges[i+1])`` — borne
basse INCLUSE, borne haute EXCLUE."""

from typing import Any


def derive(values: list[int], edges: list[int]) -> dict[str, Any]:
    """Construit l'histogramme d'une population depuis la serie BRUTE."""
    if len(edges) < 2:
        raise ValueError("il faut au moins deux bornes")
    for i in range(len(edges) - 1):
        if not edges[i] < edges[i + 1]:
            raise ValueError("bornes non strictement croissantes")

    bin_count = len(edges) - 1
    counts = [0] * bin_count
    underflow = 0
    overflow = 0
    for v in values:
        if v < edges[0]:
            underflow += 1
        elif v >= edges[-1]:
            overflow += 1
        else:
            for i in range(bin_count):
                if edges[i] <= v < edges[i + 1]:
                    counts[i] += 1
                    break
    return {
        "bin_edges": list(edges),
        "bin_counts": counts,
        "underflow": underflow,
        "overflow": overflow,
        "sample_count": len(values),
    }


def compare(device: dict[str, Any] | None, tooling: dict[str, Any]) -> dict[str, Any]:
    """Compare l'histogramme embarque au recalcul autoritaire.

    ``device`` a None signifie que l'histogramme embarque etait desactive (cas
    par DEFAUT) : ce n'est PAS une divergence."""
    if device is None:
        return {"compared": False, "reason": "histogramme embarque desactive"}

    differences: list[str] = []
    if list(device.get("bin_edges", [])) != list(tooling["bin_edges"]):
        differences.append("bin_edges")
    for key in ("bin_counts", "underflow", "overflow", "sample_count"):
        if device.get(key) != tooling[key]:
            differences.append(key)
    return {
        "compared": True,
        "match": not differences,
        "differences": differences,
    }

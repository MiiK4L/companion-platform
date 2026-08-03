# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Statistiques de latence, en arithmetique ENTIERE et REPRODUCTIBLE.

Methode de quantile FIGEE et ARCHIVEE : ``nearest-rank-inclusive-v1``.

    P(p) = x[ceil(p x n)]   sur les valeurs TRIEES, indexees a partir de 1

Le rang est calcule en ENTIERS (``ceil(num*n/den)``), jamais en flottants : une
interpolation ou un arrondi flottant rendrait le resultat dependant de la
plateforme et non recalculable dans plusieurs annees.

Regles contractuelles :
  - ``n = 0`` : quantile INDISPONIBLE (``None``), jamais 0 ni une valeur inventee ;
  - ``p`` borne a ``0 < p <= 1`` ;
  - P50/P95/P99 sont calcules sur la MEME population filtree ;
  - aucune modification silencieuse : changer de methode impose une nouvelle
    version de nom (``...-v2``), afin que deux analyses restent comparables.

Les TIMEOUTS sont EXCLUS de la distribution principale (ils sont publies a part) :
inclure un budget de timeout ecraserait le P99 vers une constante artificielle."""

import math
from typing import Any

# Nom ARCHIVE de la methode : doit etre inscrit dans l'artefact d'analyse.
QUANTILE_METHOD = "nearest-rank-inclusive-v1"

# La dispersion implementee est l'ECART-TYPE DE POPULATION des latences. Le terme
# generique « gigue » n'est PAS employe : il recouvre au moins trois grandeurs
# distinctes (ecart-type des latences, variation inter-echantillons, ecart a une
# periode attendue). Seule la premiere est fournie, et elle est nommee comme telle.
DISPERSION_METRIC = "latency_stddev_population"

_MASK64 = (1 << 64) - 1


def elapsed_wrap_safe(t_start: int, t_end: int) -> int:
    """Duree ecoulee WRAP-SAFE, miroir exact de ``bench_elapsed`` du coeur.

    Soustraction modulaire sur 64 bits ; surtout PAS une soustraction Python
    ordinaire, qui donnerait un negatif au rebouclage du compteur."""
    return (t_end - t_start) & _MASK64


def quantile(sorted_values: list[int], num: int, den: int) -> int | None:
    """Quantile ``num/den`` par rang le plus proche, inclusif (entiers seulement)."""
    if den <= 0 or num <= 0 or num > den:
        raise ValueError("p doit verifier 0 < p <= 1")
    n = len(sorted_values)
    if n == 0:
        return None  # indisponible : on n'invente pas de valeur
    # ceil(num * n / den) en arithmetique entiere exacte.
    rank = -((-num * n) // den)
    if rank < 1:
        rank = 1
    if rank > n:
        rank = n
    return sorted_values[rank - 1]


def latency_stats(values: list[int]) -> dict[str, Any]:
    """Statistiques d'une population de latences (deja filtree)."""
    n = len(values)
    out: dict[str, Any] = {
        "quantile_method": QUANTILE_METHOD,
        "dispersion_metric": DISPERSION_METRIC,
        "valid_sample_count": n,
    }
    if n == 0:
        # Aucune statistique n'est fabriquee sur une population vide.
        out.update(
            {
                "min": None,
                "max": None,
                "mean": None,
                "p50": None,
                "p95": None,
                "p99": None,
                "latency_stddev_population": None,
            }
        )
        return out

    ordered = sorted(values)
    total = sum(ordered)
    total_sq = sum(v * v for v in ordered)
    # Variance de POPULATION exacte : (n*sum(x^2) - sum(x)^2) / n^2.
    variance_num = n * total_sq - total * total
    out.update(
        {
            "min": ordered[0],
            "max": ordered[-1],
            "mean": total / n,
            "p50": quantile(ordered, 50, 100),
            "p95": quantile(ordered, 95, 100),
            "p99": quantile(ordered, 99, 100),
            "latency_stddev_population": math.sqrt(variance_num / (n * n)),
        }
    )
    return out


def reconcile(counts: dict[str, int]) -> dict[str, Any]:
    """Verifie l'identite de reconciliation, sans double comptage.

    issued = ok + timeout + rejected + unpaired + duplicate + out_of_order
             + producer_drop

    Les ``transport_gap`` n'en font PAS partie : ce ne sont pas des transactions
    executees par le moteur, mais des artefacts perdus APRES emission. Ils sont
    reconcilies separement, par les numeros de sequence du flux."""
    terminal = (
        "ok",
        "timeout",
        "rejected",
        "unpaired",
        "duplicate",
        "out_of_order",
        "producer_drop",
    )
    accounted = sum(int(counts.get(k, 0)) for k in terminal)
    issued = int(counts.get("issued", 0))
    return {
        "issued": issued,
        "accounted": accounted,
        "balanced": accounted == issued,
        "difference": issued - accounted,
        "terminal_counts": {k: int(counts.get(k, 0)) for k in terminal},
    }


def series_completeness(producer_drop: int, transport_gap: int) -> str:
    """``complete`` seulement si AUCUNE perte, d'aucune des deux natures.

    Une serie ``incomplete`` ne doit pas fonder un verdict sur P95/P99 : les
    pertes ne sont pas aleatoires (un puits sature perd pendant les rafales),
    donc elles biaisent la queue de distribution dans le sens favorable."""
    return "complete" if (producer_drop == 0 and transport_gap == 0) else "incomplete"

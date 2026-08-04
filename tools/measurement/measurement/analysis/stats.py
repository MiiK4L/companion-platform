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

#: Largeurs de compteur acceptees. Une largeur non declaree ici est REFUSEE
#: plutot que traitee comme du 64 bits, ce qui fausserait les latences apres
#: rebouclage d'un compteur plus etroit.
SUPPORTED_TICK_WIDTHS = (8, 16, 32, 64)


def elapsed_wrap_safe(t_start: int, t_end: int, width_bits: int = 64) -> int:
    """Duree ecoulee WRAP-SAFE, miroir exact de ``bench_elapsed`` du coeur.

    Soustraction modulaire sur la largeur DECLAREE par le flux ; surtout PAS une
    soustraction Python ordinaire, qui donnerait un negatif au rebouclage, ni un
    masque 64 bits impose a un compteur 32 bits."""
    if width_bits not in SUPPORTED_TICK_WIDTHS:
        raise ValueError(f"largeur de tick non supportee : {width_bits}")
    mask = (1 << width_bits) - 1
    return (t_end - t_start) & mask


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
        # Les quantiles sont AUTORITAIRES (entiers exacts) ; la moyenne et
        # l'ecart-type ne le sont que sous leur forme RATIONNELLE exacte.
        "authoritative_fields": ["min", "max", "p50", "p95", "p99"],
        "exact_rational_fields": [
            "mean_numerator",
            "mean_denominator",
            "variance_numerator",
            "variance_denominator",
        ],
        "float_views_non_authoritative": ["mean", "latency_stddev_population"],
        "valid_sample_count": n,
    }
    if n == 0:
        # Aucune statistique n'est fabriquee sur une population vide.
        out.update(
            {
                "min": None,
                "max": None,
                "p50": None,
                "p95": None,
                "p99": None,
                "mean_numerator": None,
                "mean_denominator": None,
                "variance_numerator": None,
                "variance_denominator": None,
                "mean": None,
                "latency_stddev_population": None,
            }
        )
        return out

    ordered = sorted(values)
    total = sum(ordered)
    total_sq = sum(v * v for v in ordered)
    # Valeurs EXACTES archivees sous forme rationnelle : elles seules sont
    # recalculables a l'identique dans plusieurs annees et sur toute plateforme.
    # Variance de POPULATION : (n*sum(x^2) - sum(x)^2) / n^2.
    variance_num = n * total_sq - total * total
    out.update(
        {
            "min": ordered[0],
            "max": ordered[-1],
            "p50": quantile(ordered, 50, 100),
            "p95": quantile(ordered, 95, 100),
            "p99": quantile(ordered, 99, 100),
            "mean_numerator": total,
            "mean_denominator": n,
            "variance_numerator": variance_num,
            "variance_denominator": n * n,
            # Vues FLOTTANTES, explicitement NON AUTORITAIRES : elles peuvent
            # differer au dernier bit selon la plateforme. Les quantiles, eux,
            # restent autoritaires en ticks entiers.
            "mean": total / n,
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

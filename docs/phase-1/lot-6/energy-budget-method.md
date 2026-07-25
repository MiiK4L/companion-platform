<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 6 — Méthode du budget énergétique (*Proposé*, `DEC-L6-002`)

> **Statut : Ouvert / Proposé.** Pose la **méthode** d'un budget énergétique
> **mesuré** et d'une **cible d'autonomie provisoire** (révisable). **Aucune valeur
> d'autonomie décrétée** ; les cibles sont `[BL]`. Valeurs `[C]/[H]/[BL]` ;
> **aucune `[M]`**. Risque **R4**.

## Principe

Le budget énergie **v1 (mesuré)** remplacera le [budget v0 (paper) du L1](../lot-1/budgets-v0.md)
une fois les **mesures** disponibles. Ce lot **spécifie la méthode**, il ne mesure
pas.

## Entrées (postes de consommation)

| Poste | Source de la mesure |
|-------|---------------------|
| Deep-sleep système | [L5](../lot-5/deep-sleep-measurement.md) |
| Conso écran (par mode/niveau) | [L4](../lot-4/power-and-backlight.md) |
| MCU actif / radio (salves) | [protocole conso/mode](protocols/consumption-per-mode.md) |
| Régulateur (Iq à vide) | [régulation](regulation-comparison.md) |
| Fuite module coupé | [protocole fuite](protocols/module-leakage.md) |
| Capacité utile de la batterie | [stockage](energy-storage-comparison.md) + décharge réelle |

## Méthode (candidate)

1. Définir un **scénario d'usage de référence** (mesurable) — durées par mode.
2. Mesurer **la conso par mode** aux **coins de tension `VBAT`** (4,2 / 3,7 / 3,4 /
   3,0 V) → conso moyenne pondérée.
3. Croiser avec la **capacité utile mesurée** (décharge réelle vs estimation).
4. En déduire une **autonomie**, puis une **cible provisoire** justifiée (révisable).

## Hypothèses, marges & traçabilité (partie intégrante de la méthode)

- **Hypothèses documentées** : chaque poste/estimation porte son **statut**
  (`[DS]/[C]/[H]/[BL]/[M]`) et sa **source** ; aucune valeur « nue ».
- **Marges** : marge appliquée au budget **explicitée et justifiée** (pas une
  valeur cachée), en `[BL]`.
- **Provenance des mesures** : chaque poste renvoie à **son protocole** (colonne
  « Source de la mesure » ci-dessus) et à la **révision exacte** testée.
- **Traçabilité** : budget = **somme reproductible** de postes horodatés (commit
  firmware, config, DUT), rejouable ; les données brutes sont hashées (SHA-256).

## Seuils & cibles (tous `[BL]`)

> **Aucun seuil figé ici** : cible d'autonomie, conso par mode admissible, capacité
> minimale — **`[BL]`**, définis au **baselining** avec les mesures. Un budget
> mesuré très inférieur déclenche un **arbitrage écran/batterie/mode** (R4, lien
> [déclencheurs L5](../lot-5/arbitration-triggers.md)).

## Frontière avec les DEC (strictement méthodologique)

> Ce document décrit **uniquement comment construire et comparer** le budget
> énergétique (postes, modes, hypothèses documentées, marges, provenance des
> mesures, traçabilité) — **sans présumer du résultat**. Il **ne contient aucune
> préférence implicite ni cible de conception**.
>
> Les **choix d'architecture** restent **exclusivement** dans les décisions :
>
> | Choix | Décision (hors ce document) |
> |-------|-----------------------------|
> | Charge / power-path / **PMIC** / régulation | `DEC-L6-001` |
> | **Architecture de stockage / chimie** de batterie | `DEC-L6-002` |
> | **Stratégie d'estimation** d'état (ADC / jauge / hybride) | `DEC-L6-003` |
>
> La méthode **s'applique de la même manière** quelle que soit l'option retenue par
> ces DEC : elle **compare** des budgets, elle n'en **choisit** aucun.

## Alimente

- **`DEC-L6-002`** — **fournit la méthode** de budget + cible d'autonomie
  provisoire, établis **après** mesures. La méthode **ne décide** ni l'architecture
  ni la chimie ni l'estimation (→ `DEC-L6-001/002/003`). **Aucune cible décrétée
  ici.**

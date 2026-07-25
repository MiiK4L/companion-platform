<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 6 — Architectures de stockage d'énergie (*Proposé*, `DEC-L6-002`)

> **Statut : Ouvert / Proposé.** Comparaison **par architecture de stockage** ; la
> **chimie** (LiPo, Li-Ion, LiFePO4…) est une **propriété** des solutions comparées,
> **pas** une famille de premier niveau. Valeurs `[DS]/[H]/[BL]` ; **aucune `[M]`**.

## Architectures de stockage comparées

| Architecture | Principe | Remarque |
|--------------|----------|----------|
| **(A1) Mono-cellule + protection intégrée (PCM)** | 1 cellule + circuit de protection | usuel format poche |
| **(A2) Mono-cellule nue + protection externe** | cellule + protection sur la carte | contrôle, mais responsabilité sécurité |
| **(A3) Multi-cellule** | plusieurs cellules (série/parallèle) | capacité/tension ↑ ; complexité (BMS) |
| **(A4) Cellule intégrée à un module** | pack prêt (connecteur + protection) | intégration rapide |

## La chimie comme **propriété** (critère de comparaison, non une famille)

| Propriété (chimie) | LiPo (3,7 V) | Li-Ion (3,6–3,7 V) | LiFePO4 (3,2 V) | Autres pertinentes |
|--------------------|--------------|--------------------|-----------------|--------------------|
| Tension nominale / plateau | 3,7 V **[DS]** | 3,6–3,7 V **[DS]** | **3,2 V, très plat** **[DS]** | selon |
| Sécurité / stabilité thermique | moyenne | moyenne | **meilleure** **[DS]** | selon |
| Densité d'énergie | **élevée** | élevée | plus faible | selon |
| Courbe de décharge (impact estimation) | pentue | pentue | **plate** (SOC par tension difficile) | selon |
| Format poche | **oui** (souple) | cylindrique/prismatique | variable | selon |

> La **courbe plate** de LiFePO4 **complique l'estimation par tension** (lien
> [estimation d'état](battery-state-estimation.md)) : propriété à croiser avec
> `DEC-L6-003`.

## Grille comparative (architectures × critères communs)

| Critère | (A1) Mono+PCM | (A2) Nue+ext. | (A3) Multi | (A4) Pack module |
|---------|---------------|---------------|------------|------------------|
| Sécurité (protection) | **intégrée** | à concevoir | BMS requis | intégrée |
| Capacité / tension | 1 cellule | 1 cellule | **modulable** | selon pack |
| Format poche / intégration | bon | bon | volumineux | rapide |
| Complexité | faible | moyenne | **élevée** | faible |
| Coût | moyen | faible | élevé | moyen-élevé |
| Disponibilité | multi-source **[H]** | multi-source | selon | selon |

## Alimente

- **`DEC-L6-002`** — architecture de stockage **+** [budget & autonomie](energy-budget-method.md),
  arbitrées **après** mesures (décharge réelle, sécurité). **Aucune architecture ni
  chimie retenue.**

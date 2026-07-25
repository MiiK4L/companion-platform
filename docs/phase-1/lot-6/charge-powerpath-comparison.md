<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 6 — Architectures de charge / power-path (*Proposé*, `DEC-L6-001`)

> **Statut : Ouvert / Proposé.** Sous-fonction de l'[architecture
> d'alimentation](power-chain-architecture.md). Comparaison **par architecture** ;
> les composants **illustrent**. Valeurs `[DS]/[H]/[BL]` ; **aucune `[M]`**.

## Architectures comparées

| Architecture | Principe | Illustré par (non retenu) |
|--------------|----------|---------------------------|
| **(C1) Chargeur simple** | charge la batterie ; le système est alimenté **depuis la batterie** | MCP73831, TP4056 |
| **(C2) Chargeur avec power-path** | alimente le **système** ET charge la batterie **séparément** (système opérable batterie vide) | BQ24074, BQ25185 |
| **(C3) PMIC intégré** | charge + power-path + régulation + protections dans un même circuit | (PMIC à sélectionner) |
| **(C4) Charge via le module cœur** | recharge par le chargeur embarqué du cœur (ex. XIAO) | dépend du cœur (`DEC-L1-002`) |

## Grille comparative (critères communs)

| Critère | (C1) Simple | (C2) Power-path | (C3) PMIC | (C4) via cœur |
|---------|-------------|-----------------|-----------|---------------|
| Système opérable batterie vide/absente | **non** | **oui** **[DS]** | oui | selon cœur |
| Courant de charge | selon réf. | jusqu'à ~1–1,5 A **[DS]** | selon PMIC | limité (cœur) **[H]** |
| Complexité / surface | faible **[H]** | moyenne **[H]** | **faible** (intégré) **[H]** | **minimale** **[H]** |
| Flexibilité (rails, réglages) | faible | moyenne | élevée | faible |
| Coût | faible **[H]** | moyen **[H]** | élevé **[H]** | inclus **[H]** |
| Disponibilité / secondes sources | multi-source **[H]** | multi-source **[H]** | à évaluer | dépend cœur |
| Dépendance au choix de cœur | non | non | non | **forte** (`DEC-L1-002`) |
| Thermique en charge | `banc` | `banc` | `banc` | `banc` |

*(Aucune conclusion : cellules `banc` à mesurer.)*

## Synthèse

- **(C1)** simple/économique mais **système lié à la batterie** ; **(C2)** power-path
  = système opérable sans batterie (atout robustesse) ; **(C3)** PMIC = intégration
  vs coût/dispo ; **(C4)** via cœur = minimal mais **couplé** à `DEC-L1-002`.

## Alimente

- **`DEC-L6-001`** — l'architecture de charge/power-path, arbitrée **après**
  mesures (courant, thermique). **Aucune retenue.**

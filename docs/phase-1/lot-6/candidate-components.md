<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 6 — Composants candidats (séries représentatives documentaires, *Proposé*)

> **Statut : Ouvert / Proposé — PRÉ-SHORTLIST DOCUMENTAIRE.** Les composants
> **illustrent une solution** au sein d'une fonction, **ne définissent pas la
> fonction**. **≥ 2 séries sourcées par fonction** ; définition en
> [Lot 2B](../lot-2b/README.md). **Aucune retenue, aucun MPN final.** `[DS]` =
> fiche ; `à relever` sinon ; **aucune `[M]`**. Statut par ligne ; cycle de vie à
> confirmer au sourcing (`R8`).

## 1. Charge / power-path

| Réf. | Fab. | Architecture illustrée | Points [DS] | Source | Caractérisation |
|------|------|------------------------|-------------|--------|-----------------|
| **BQ24074** | TI | chargeur **avec power-path** | 1-cell linéaire ~1,5 A, VBAT 4,2 V, OVP | TI BQ24074 | partielle |
| **BQ25185** | TI | chargeur **power-path**, faible Iq | charge ~1 A, système ~3,125 A | TI BQ25185 | partielle |
| **MCP73831** | Microchip | chargeur **simple** | linéaire 1-cell | Microchip MCP73831 | partielle |
| **TP4056** | (générique) | chargeur **simple** bas coût | linéaire 1-cell | fiche TP4056 | à compléter |

## 2. Régulation

| Réf. | Fab. | Architecture illustrée | Points [DS] | Source | Caractérisation |
|------|------|------------------------|-------------|--------|-----------------|
| **TLV757P** | TI | **LDO** faible Iq | Iq ~25 µA, 1 A **[DS]** | TI TLV757P | partielle |
| **TPS62840** | TI | **buck** ultra-basse conso | Iq ~60 nA, 750 mA, 1,8–6,5 V **[DS]** | TI TPS62840 | partielle |

## 3. Estimation d'état de batterie

| Réf. | Fab. | Approche illustrée | Points [DS] | Source | Caractérisation |
|------|------|--------------------|-------------|--------|-----------------|
| **MAX17048** | Analog Devices | jauge à **modèle** (voltage) | 3 µA, ModelGauge, sans shunt **[DS]** | Analog MAX17048 | partielle |
| **LC709203F** | Onsemi | jauge à **modèle** (voltage) | 1-cell, tension + SOC **[DS]** | Onsemi LC709203F | partielle |
| **BQ27441** | TI | jauge **coulomb** (shunt) | comptage de charge | TI BQ27441 | à compléter |
| **ADC (cœur)** | — | **ADC seul** | mesure VBAT (firmware) | N/A (intégré) | N/A |

## 4. Stockage d'énergie (chimie = propriété, sourcé au niveau cellule/pack)

| Solution | Chimie (propriété) | Remarque | Caractérisation |
|----------|--------------------|----------|-----------------|
| Cellule pouch + PCM | **LiPo 3,7 V** | format poche souple | à sourcer (cellule/pack) |
| Cellule cylindrique | **Li-Ion (ex. 18650)** | densité ; volume | à sourcer |
| Cellule LiFePO4 | **LiFePO4 3,2 V** | sûreté, courbe plate | à sourcer |

> Les batteries se sourcent au **niveau cellule/pack** (multi-fournisseur) ; la
> **chimie** est une **propriété** (cf. [stockage](energy-storage-comparison.md)),
> pas une famille. **Aucune cellule retenue.**

## 5. Sources datasheet (traçabilité)

> Consultées le **2026-07-23** ; `[DS]` issues de ces documents. Références « à
> compléter » **sans `[DS]`** tant qu'une série précise n'est pas documentée.

| Réf. | Fabricant | Document | Date | Référence stable |
|------|-----------|----------|------|------------------|
| BQ24074 | TI | 1,5 A Li-Ion charger + power-path | 2026-07-23 | `ti.com` (BQ24074) |
| BQ25185 | TI | Li-Ion charger power-path | 2026-07-23 | `ti.com` (BQ25185) |
| MCP73831 | Microchip | 1-cell Li-Ion charge management | 2026-07-23 | `microchip.com` (MCP73831) |
| TLV757P | TI | 1 A low-IQ LDO | 2026-07-23 | `ti.com` (TLV757P) |
| TPS62840 | TI | 750 mA, 60 nA IQ buck | 2026-07-23 | `ti.com` (TPS62840) |
| MAX17048 | Analog Devices | 3 µA ModelGauge fuel gauge | 2026-07-23 | `analog.com` (MAX17048) |
| LC709203F | Onsemi | 1-cell fuel gauge | 2026-07-23 | `onsemi.com` (LC709203F) |
| BQ27441 | TI | coulomb-counting fuel gauge | 2026-07-23 | `ti.com` (BQ27441) |

## Alimente

- **`DEC-L6-001` / `DEC-L6-002` / `DEC-L6-003`** — ces séries **illustrent** les
  solutions ; **aucune retenue**, aucun MPN final.

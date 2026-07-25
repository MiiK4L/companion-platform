<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 4 — Partage du bus SPI écran ↔ CX-Bus (*Proposé*, `DEC-L4-001`)

> **Statut : Ouvert / Proposé.** Traite le **risque R2** (contention SPI
> écran ↔ CX-Bus) au niveau **affichage**, en **réutilisant les entrées du
> [Lot 2A](../lot-2/isolation-and-switching.md)** (isolation/CS) et de
> [2B](../lot-2b/pinout.md) (brochage). Valeurs `[H]/[BL]` ; **aucune `[M]`**.

## 1. Question

L'écran (fort trafic, rafraîchissement) et le module CX-Bus **partagent-ils** le
SPI de façon viable, **sans glitch écran ni corruption bus** ?

## 2. Facteurs

| Facteur | Contenu | Étiquette |
|---------|---------|-----------|
| Interface écran | **SPI** ou **QSPI** (débit ↑) — cf. [familles](display-families-comparison.md) | **[DS]** |
| Chip-selects distincts | écran vs module ; arbitrage | **[H]** |
| Débit & occupation | rafraîchissement écran vs accès module | `banc` |
| Hot-plug pendant trafic SPI | cas critique (cf. [2A hot-plug](../lot-2/hot-plug.md)) | `banc` |
| Option : SPI module **optionnel/séparé** | découpler écran et module | **[H]** |

## 3. Critères (proposés, à mesurer)

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Glitch écran lors d'un accès simultané | 0 hors spec | **[P]** |
| Corruption bus module | 0 | **[P]** |
| FPS écran sous trafic module | ≥ cible (≈ 30 fps) | **[P]/[BL]** |

Mesure : [protocole contention SPI](protocols/spi-contention.md) (couplé aux
protocoles FPS et R2 du Lot 2A).

## Alimente

- **`DEC-L4-001`** — l'**interface** (SPI/QSPI) et la **viabilité du partage**
  (ou un SPI module séparé) sont des **critères d'arbitrage** ; à mesurer.
  Contrainte **remontée** vers le brochage (`DEC-L2-002`) si nécessaire.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 4 — Budget mémoire & FPS (*Proposé*)

> **Statut : Ouvert / Proposé.** Sépare **contraintes imposées par le matériel**
> (`DEC-L4-001`) et **stratégie mémoire du moteur** (`DEC-L4-002`) : le matériel
> **fournit des contraintes**, le moteur **décide comment les exploiter**. Valeurs
> `[DS]/[C]/[H]/[BL]` ; **aucune `[M]`**. À confronter au
> [budget L1](../lot-1/budgets-v0.md) (RAM/PSRAM ESP32-S3).

## 1. Contraintes mémoire **imposées par le matériel** (`DEC-L4-001`)

| Contrainte | Origine | Impact |
|-----------|---------|--------|
| **Taille du framebuffer** | résolution × profondeur de couleur | `largeur × hauteur × octets/pixel` **[C]** |
| **Profondeur de couleur** | contrôleur/dalle (16 bit RGB565, 18 bit…) | octets/pixel **[DS]** |
| **GDDRAM embarquée** | certains contrôleurs (ex. SSD1351) stockent l'image | réduit la RAM Host **[DS]** |
| **Alignement / DMA** | transferts SPI/QSPI par DMA | tampons alignés, contraintes de taille **[H]** |
| **Interface / débit** | SPI vs QSPI vs RGB | plafonne le FPS atteignable **[H]** |

Exemple de calcul **[C]** (indicatif, non figé) : un plein écran **240×240** en
**RGB565** (2 o/px) = **≈ 112,5 Kio** de framebuffer plein ; un buffer **partiel
1/10** ≈ **11 Kio**. Ces chiffres sont **calculés**, à **confronter** aux budgets.

## 2. Stratégie mémoire **du moteur** (`DEC-L4-002`)

Le moteur **exploite** les contraintes ci-dessus via sa
[stratégie](graphics-engine-comparison.md) : **buffer partiel** (+ dirty
rectangles), **rendu direct** (framebuffer plein), **double/triple buffer** (DMA),
**tuiles**. Le **compromis RAM ↔ FPS** est **mesuré** (protocoles), pas décrété.

## 3. Cible de fluidité (proposée)

| Grandeur | Cible | Étiquette |
|----------|-------|-----------|
| FPS UI (animations) | **≈ 30 fps** | **[P]** |
| Budget RAM du moteur retenu | **dans le budget L1** (`[BL]` Kio) | **[P]/[BL]** |

### Champs à finaliser au baselining (`[BL]`)

- **Budget RAM cible** exact (Kio) dérivé du budget L1 et du cœur retenu.
- **Résolution/profondeur** de référence pour le calcul du framebuffer.

## Alimente

- **`DEC-L4-001`** — fournit les **contraintes** (framebuffer, profondeur, DMA).
- **`DEC-L4-002`** — **décide** de la stratégie qui tient le budget, **prouvée** par
  les [protocoles RAM/FPS](protocols/README.md). **Rien de figé ici.**

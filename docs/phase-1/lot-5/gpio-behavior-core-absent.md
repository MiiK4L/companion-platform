<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 5 — Comportement des GPIO cœur absent / éteint / remplacé (*Proposé*)

> **Statut : Ouvert / Proposé.** Analyse **documentaire** des conséquences sur
> l'**intégrité des bus** et le **back-powering** quand le cœur n'est pas actif.
> **Réutilise les entrées du [Lot 2A](../lot-2/back-powering.md)**. Valeurs
> `[H]/[BL]` ; **aucune `[M]`**.

## 1. Trois états du cœur

| État | Situation | Enjeux |
|------|-----------|--------|
| **Absent** | support vide (cœur retiré) | lignes flottantes ; back-powering du reste de la carte |
| **Éteint** | cœur monté mais non alimenté | GPIO en haute-Z ? diodes ESD internes alimentées par les bus ? |
| **Remplacé** | en cours d'échange (transitoire) | états intermédiaires ; séquencement |

## 2. Conséquences à cadrer

| Effet | Exigence candidate | Étiquette |
|-------|--------------------|-----------|
| **Lignes de bus flottantes** | pull-up/down définis ; pas d'état indéterminé nuisible | **[BL]** |
| **Back-powering** (via GPIO/bus vers le cœur éteint) | pas de ré-alimentation parasite (entrée [2A](../lot-2/back-powering.md)) | **[H]** |
| **Intégrité des bus partagés** (I²C/SPI) | les autres périphériques restent opérables ou en repli défini | **[BL]** |
| **Courant de fuite** cœur éteint | ≤ `[BL]` ; n'empêche pas le deep-sleep | **[BL]** |
| **Séquencement au remplacement** | pas de latch-up / état bloqué (cf. [2A](../lot-2/electrical-risk-analysis.md)) | `banc` |

## 3. Lien avec la sûreté électrique (L2A)

Le comportement « cœur éteint/absent » est le **pendant, côté cœur**, du
back-powering étudié côté **module CX-Bus** en L2A : mêmes principes (haute-Z,
non-réalimentation, séquencement). Les **protections/topologies** candidates y
sont réutilisées.

## Alimente

- **`DEC-L5-001`** — via les exigences « cœur absent/éteint sûr » ; mesuré par le
  [protocole GPIO cœur absent](protocols/gpio-core-absent.md). Peut **remonter**
  une contrainte de topologie (pull-ups, isolation) vers la conception carte.

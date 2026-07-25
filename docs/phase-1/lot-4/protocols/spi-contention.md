<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L4-SPI — Contention SPI écran ↔ CX-Bus (R2)

> **Statut : Brouillon (en attente de baselining).** Essai **matériel + code**.
> Traite **R2** au niveau affichage ; **réutilise** les entrées
> [Lot 2A](../../lot-2/electrical-risk-analysis.md). [Conventions](README.md) ·
> [définitions](event-definitions.md). Seuils `[P]/[BL]` ; **aucune `[M]`**.
> Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 4 · alimente **`DEC-L4-001`** (viabilité du SPI partagé / interface).

## Objectif (une question mesurable)

Sous **trafic écran** (rafraîchissement) **et** accès **module CX-Bus**
simultanés, observe-t-on **0 glitch écran hors spec** et **0 corruption bus** tout
en tenant la **cible FPS** ?

## Conditions

Écran actif (SPI/QSPI) + accès module CX-Bus injectés ; chip-selects distincts ;
option **SPI module séparé** évaluée en variante.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Glitch écran (accès simultané) | 0 hors spec (CRC/caméra) | **[P]** |
| Corruption bus module | 0 | **[P]** |
| FPS sous trafic module | ≥ cible (≈ 30 fps) | **[P]/[BL]** |

### Champs à finaliser au baselining (`[BL]`)

- Profil de trafic module ; débit écran ; définition « hors spec » (cf.
  [définitions](event-definitions.md) — glitch = observation de sortie).

## Plan d'essai (type **matériel + code**)

- **`n_dut`** ≥ 2 (panneau + fixture module) · variantes (SPI partagé vs séparé) ·
  **`n_campaigns`** ≥ 2 · commit firmware exact.

## Données brutes attendues

Traces bus (analyseur) + FPS + indicateurs de glitch écran, horodatés ; SHA-256 au
rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

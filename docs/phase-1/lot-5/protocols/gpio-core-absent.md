<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L5-GPIO — Comportement GPIO cœur absent / éteint / remplacé

> **Statut : Brouillon (en attente de baselining).** Essai **matériel + code**.
> Réutilise les entrées [Lot 2A](../../lot-2/back-powering.md). [Conventions](README.md) ·
> [définitions](event-definitions.md). Seuils `[P]/[BL]` ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L5-001`** (sûreté « cœur absent/éteint ») ; ↔ back-powering L2A.

## Objectif (une question mesurable)

Cœur **absent**, **éteint** ou **en cours de remplacement**, les **lignes de bus**
restent-elles dans un **état défini** (pas d'indétermination nuisible), **sans
back-powering** ni blocage des autres périphériques ?

## États testés

Support **vide** ; cœur **monté non alimenté** ; **transitoire** de remplacement.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Ligne de bus indéterminée nuisible | 0 (pull-up/down définis) | **[P]** |
| Back-powering (rail cœur via bus) | ≤ `[BL]` (pas de réveil parasite) | **[P]/[BL]** |
| Opérabilité des autres périphériques | conforme ou repli défini | **[P]** |
| Latch-up / blocage au remplacement | 0 (cf. [2A](../../lot-2/electrical-risk-analysis.md)) | **[P]** |

### Champs à finaliser au baselining (`[BL]`)

- Seuils de fuite/back-powering ; pull-ups/down attendus ; définition du « repli ».

## Plan d'essai (matériel + code)

- **`n_dut`** ≥ 2 supports · états (absent/éteint/transitoire) figés ·
  **`n_campaigns`** ≥ 2 · analyseur de bus + sonde de courant + commit firmware exact.

## Critères d'arrêt immédiat

Latch-up ou back-powering soutenu → consigner.

## Données brutes attendues

Traces bus + courant rail cœur par état, horodatées ; SHA-256 au rapport.

## Sécurité opérateur

Limitation de courant ; ESD standard.

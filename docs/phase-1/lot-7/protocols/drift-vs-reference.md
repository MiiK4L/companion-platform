<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L7-DRIFT — Dérive vs référence

> **Statut : Brouillon (en attente de baselining).** Essai **matériel + mesure
> fine**. [Conventions](README.md) · [définitions](event-definitions.md). **Seuil
> `[BL]`** ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L7-001`** (précision de la source de temps).

## Objectif (une question mesurable)

Quelle est la **dérive** (ppm) de chaque source de temps (interne / externe /
TCXO) mesurée **contre une référence** sur une durée représentative ?

## Conditions

Sources en parallèle vs **référence de temps** (GPS/atomique/labo) ; durée `[BL]`
(le plan Phase 1 évoque **≥ 72 h**, seuil/durée **arrêtés au baselining**) ;
température(s) documentée(s) (impact TCXO vs non compensé).

## Seuils / grandeurs

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Dérive vs référence | ≤ **`[BL]`** ppm | **[BL]** |
| Sensibilité à la température | documentée | `banc` |

> **Aucune valeur ppm figée ici** : seuil et durée arrêtés au **baselining**.

## Plan d'essai (matériel + mesure fine)

- **`n_dut`** ≥ 2 par source · durée `[BL]` · **`n_campaigns`** ≥ 2 · référence
  étalonnée ; température(s) consignées.

## Données brutes attendues

Écarts horodatés vs référence, température ; SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

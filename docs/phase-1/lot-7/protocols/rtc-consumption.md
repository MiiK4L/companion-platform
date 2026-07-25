<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L7-CONSO — Consommation de la base de temps

> **Statut : Brouillon (en attente de baselining).** Essai **matériel + mesure
> fine**. [Conventions](README.md) · [définitions](event-definitions.md). **Seuil
> `[BL]`** ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L7-001`** ; **entrée** du [budget énergie L6](../../lot-6/energy-budget-method.md).

## Objectif (une question mesurable)

Quelle est la **consommation** de chaque source de temps (RTC + sauvegarde), en
fonctionnement et en veille système ?

## Conditions

Mesure du courant du RTC (et de la sauvegarde/switchover) ; système en deep-sleep
(couplé [L5](../../lot-5/deep-sleep-measurement.md)).

## Seuils / grandeurs

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Conso RTC (veille système) | ≤ **`[BL]`** | **[BL]** |
| Conso sauvegarde (pile/supercap) | mesurée | `banc` |

> **Aucune valeur « < 1 µA » figée ici** : seuil arrêté au **baselining** (dérivé
> du budget d'autonomie).

## Plan d'essai (matériel + mesure fine)

- **`n_dut`** ≥ 2 par source · **`n_campaigns`** ≥ 2 · µA/nA-mètre.

## Données brutes attendues

Traces courant RTC/sauvegarde, horodatées ; SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L6-SOC — Précision d'estimation de l'état de batterie

> **Statut : Brouillon (en attente de baselining).** Essai **matériel +
> référence**. [Conventions](README.md) · [définitions](event-definitions.md).
> **Seuil de précision = `[BL]`** (défini au baselining, **pas** figé ici) ;
> **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L6-003`** (stratégie d'estimation d'état).

## Objectif (une question mesurable)

Quelle est l'**erreur d'estimation du SOC** de chaque approche (ADC / jauge
modèle / coulomb / hybride) **par rapport à une référence coulombmétrique**, sur
une **décharge complète** ?

## Conditions

Approche testée en parallèle d'un **coulombmètre de référence** (vérité terrain) ;
décharge complète représentative ; chimie(s) documentée(s) (courbe plate LiFePO4
= cas difficile pour l'ADC).

## Seuils / grandeurs

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Erreur d'estimation SOC vs référence | ≤ **`[BL]`** | **[BL]** |
| Robustesse selon chimie (courbe plate) | erreur ≤ **`[BL]`** | **[BL]** |

> **Aucune cible « ±X % » n'est figée dans ce document** : le seuil est arrêté au
> **baselining**, en fonction du besoin produit et de la chimie retenue.

### Champs à finaliser au baselining (`[BL]`)

- Seuil d'erreur admissible ; chimie(s) de test ; profil de décharge ; référence.

## Plan d'essai (matériel + référence)

- **`n_dut`** ≥ 2 cellules · approches comparées en parallèle · **`n_campaigns`**
  ≥ 2 · coulombmètre étalonné ; commit firmware exact.

## Données brutes attendues

Courbes SOC estimé vs référence par approche/chimie ; SHA-256 au rapport.

## Sécurité opérateur

Manipulation batterie : protection surintensité/thermique ; ESD standard.

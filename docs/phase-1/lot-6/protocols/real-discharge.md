<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L6-DISCH — Décharge réelle & capacité utile

> **Statut : Brouillon (en attente de baselining).** Essai **matériel +
> référence**. [Conventions](README.md) · [définitions](event-definitions.md).
> **Seuils `[BL]`** ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L6-002`** (capacité utile → budget & autonomie) et
**`DEC-L6-003`** (référence pour l'estimation).

## Objectif (une question mesurable)

Quelle est la **capacité utile réelle** (mAh) restituée en décharge, sous le
**profil d'usage de référence**, jusqu'à la coupure sous-tension (UVP) ?

## Conditions

Décharge sous **profil représentatif** (modes pondérés) ; **coulombmètre de
référence** ; coupure à `VBAT` bas (UVP) ; chimie(s) documentée(s).

## Seuils / grandeurs

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Capacité utile (mAh) | mesurée (entrée budget) | valeur `banc` |
| Écart capacité annoncée − utile | ≤ **`[BL]`** | **[BL]** |
| Seuil de coupure UVP | **`[BL]`** V | **[BL]** |

### Champs à finaliser au baselining (`[BL]`)

- Profil de décharge ; seuil UVP ; écart admissible ; chimie(s).

## Plan d'essai (matériel + référence)

- **`n_dut`** ≥ 2 cellules · **`n_campaigns`** ≥ 2 · coulombmètre étalonné ;
  couplé à la [précision d'estimation](state-estimation-accuracy.md).

## Données brutes attendues

Courbes décharge (VBAT, courant, capacité cumulée), horodatées ; SHA-256 au rapport.

## Sécurité opérateur

Manipulation batterie : protection surintensité/thermique ; pas de décharge
profonde non surveillée.

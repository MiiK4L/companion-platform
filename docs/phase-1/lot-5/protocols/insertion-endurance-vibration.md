<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L5-ENDUR — Endurance d'insertion + vibration + épaisseur

> **Statut : Brouillon (en attente de baselining).** Essai **matériel**.
> [Conventions](README.md) · [définitions](event-definitions.md). Seuils
> `[P]/[BL]` ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L5-001`** (tenue mécanique du support).

## Objectif (une question mesurable)

Le support tient-il **≥ N cycles** d'insertion **sans dégradation**, **0
intermittence** sous **vibration**, et quelle **épaisseur** ajoute-t-il ?

## Conditions

Insertion/retrait motorisés (vitesse `[BL]`) ; profil de **vibration** défini
(`[BL]`) sous **charge** ; **relevé d'épaisseur** avant/après.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Cycles d'insertion sans dégradation | ≥ `[BL]` | **[P]/[BL]** |
| Intermittence sous vibration | 0 > `[BL]` µs | **[P]/[BL]** |
| Force de rétention (fin d'essai) | ≥ `[BL]` N | **[P]/[BL]** |
| Épaisseur ajoutée | mesurée (mm) | `[BL]` (contrainte boîtier) |

### Champs à finaliser au baselining (`[BL]`)

- Nombre de cycles cible ; **profil de vibration** représentatif du nomade ;
  vitesse ; seuils de rétention/intermittence.

## Plan d'essai (matériel)

- **`n_dut`** ≥ 2 supports (par famille) · **`n_campaigns`** ≥ 2 · détecteur
  d'intermittence + 4 fils + capteur de force + comparateur d'épaisseur.

## Critères d'arrêt immédiat

Perte de rétention, casse, intermittences répétées au-delà du seuil.

## Données brutes attendues

R_contact/cycles, journal d'intermittences, force de rétention, épaisseur ; SHA-256
au rapport.

## Sécurité opérateur

Banc motorisé / pot vibrant : protections, arrêt d'urgence, fixation.

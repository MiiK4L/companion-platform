<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L6-LEAK — Fuite du module coupé

> **Statut : Brouillon (en attente de baselining).** Essai **matériel + mesure
> fine**. Réutilise les entrées [Lot 2A](../../lot-2/back-powering.md). [Conventions](README.md)
> · [définitions](event-definitions.md). **Seuil `[BL]`** ; **aucune `[M]`**.
> Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L6-001`** (impact autonomie) ; **frontière** : le power-gating du
slot relève de **`DEC-L2-003`** (non décidé ici).

## Objectif (une question mesurable)

Quel est le **courant de fuite** vers le slot lorsque **`VMOD` est coupé**
(module power-gated) — impact sur le budget deep-sleep ?

## Conditions

`VMOD` coupé ; bus au repos et actifs (cf. back-powering L2A) ; module (ou charge
équivalente) présent puis absent.

## Seuils / grandeurs

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Fuite vers le slot, `VMOD` coupé | ≤ **`[BL]`** µA | **[BL]** |
| Contribution au budget deep-sleep | ≤ **`[BL]`** | **[BL]** |

> **Aucune valeur « < 10 µA » figée ici** : le seuil est arrêté au **baselining**
> (dérivé de la cible d'autonomie).

### Champs à finaliser au baselining (`[BL]`)

- Seuil de fuite admissible (dérivé du budget) ; conditions (bus actifs/repos).

## Plan d'essai (matériel + mesure fine)

- **`n_dut`** ≥ 2 · états (module présent/absent, bus repos/actif) · **`n_campaigns`**
  ≥ 2 · µA-mètre sur `VMOD`.

## Données brutes attendues

Traces courant de fuite par état, horodatées ; SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

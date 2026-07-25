<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L7-DELTAT — Δt sur cycles deep-sleep

> **Statut : Brouillon (en attente de baselining).** Essai **matériel + code**.
> [Conventions](README.md) · [définitions](event-definitions.md). **Seuil `[BL]`** ;
> **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L7-001`** (exactitude du temps au réveil).

## Objectif (une question mesurable)

Après **N cycles** power-off/deep-sleep → réveil, le **Δt** (temps écoulé) est-il
**exact** (erreur nulle) vs une référence ?

## Conditions

Cycles deep-sleep → réveil (alarme RTC / timer / mouvement selon architecture) ;
**référence de temps** ; Δt calculé par le firmware vs Δt réel.

## Seuils / grandeurs

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Erreur de Δt | 0 sur **`[BL]`** cycles | **[BL]** |
| Perte de validité au réveil | 0 (état correct exposé) | **[P]** |

> Le plan évoque **100 cycles / erreur 0** ; le **nombre exact** est `[BL]`
> (baselining).

## Plan d'essai (matériel + code)

- **`n_dut`** ≥ 2 · **`n_cycles`** ≥ `[BL]` (deep-sleep→réveil) · **`n_campaigns`**
  ≥ 2 · référence de temps ; commit firmware exact.

## Données brutes attendues

Δt réel vs calculé par cycle, source de réveil, état de validité ; SHA-256 au
rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

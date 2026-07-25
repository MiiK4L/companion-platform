<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L7-RECOV — Reprise d'état sur coupures brutales

> **Statut : Brouillon (en attente de baselining).** Essai **matériel + code**.
> [Conventions](README.md) · [définitions](event-definitions.md). **Seuil `[BL]`** ;
> **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L7-002`** (support **et** modèle de cohérence de la persistance).

## Objectif (une question mesurable)

Après de **nombreuses coupures brutales** (power-off non contrôlé, y compris
**pendant une écriture**), le système **reprend-il un état cohérent**, **sans
corruption non détectée** et **sans heure fausse marquée valide** ?

## Conditions

Coupures brutales injectées à des **phases variées** (dont **pendant écriture**) ;
support (NVS/LittleFS…) × modèle de cohérence (journal/A-B/checkpoint…) testés
**séparément** (cf. [persistance](../persistence-comparison.md)).

## Seuils / grandeurs

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Corruption non détectée | **0** | **[P]** |
| Reprise à un état cohérent | 100 % sur **`[BL]`** coupures | **[P]/[BL]** |
| MAJ interrompue distinguée de corruption | 100 % | **[P]** |
| Heure fausse exposée comme « heure valide » (au lieu d'« heure inconnue ») | **0** | **[P]** |

> Le plan évoque **100 coupures / 0 corruption** ; le **nombre** est `[BL]`.

### Champs à finaliser au baselining (`[BL]`)

- Nombre de coupures ; phases d'injection ; combinaisons support × cohérence ;
  méthode de vérification (checksum/génération).

## Plan d'essai (matériel + code)

- **`n_dut`** ≥ 2 · coupures **par phase** · support × modèle **appariés** ·
  **`n_campaigns`** ≥ 2 · vérification checksum ; commit firmware exact.

## Critères d'arrêt immédiat

Corruption non détectée reproductible → consigner (échec structurant).

## Données brutes attendues

Journal (phase de coupure → état repris), checksums, état de validité de l'heure ;
SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

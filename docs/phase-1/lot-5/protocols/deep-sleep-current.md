<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L5-SLEEP — Deep-sleep système (µA)

> **Statut : Brouillon (en attente de baselining).** Essai **matériel + mesure
> fine**. [Conventions](README.md) · [définitions](event-definitions.md). Seuils
> `[P]/[BL]` ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Décision / risque visés

Alimente **`DEC-L1-002`** (choix du cœur) **et** le risque **R3** ; **preuve
réutilisable** pour les décisions power/domaines de puissance/autonomie (cf.
[deep-sleep](../deep-sleep-measurement.md)).

## Objectif (une question mesurable)

Quel est le **courant deep-sleep du système** (cœur monté), et l'**écart
socketé − soudé** (contribution du support) ?

## Conditions

Cœur en **deep-sleep** réel, périphériques au repos ; configurations **socketé**
(par famille) **et** **soudé (baseline)** ; alimentation représentative.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Courant deep-sleep système | ≤ cible (dérivée L6) | **[P]/[BL]** |
| Contribution du support (socketé − soudé) | ≤ `[BL]` | **[P]/[BL]** |

### Champs à finaliser au baselining (`[BL]`)

- **Cible d'autonomie** (dérivée L6) → seuil deep-sleep ; définition de « repos »
  des périphériques ; instrument et calibration.

## Plan d'essai (matériel + mesure fine)

- **`n_dut`** ≥ 2 supports (par famille) + baseline soudée · **`n_campaigns`** ≥ 2 ·
  µA-mètre (PPK2/Joulescope) ; commit firmware exact.
- **Méthode** : moyenne/percentiles ; comparaison socketé/soudé appariée.

## Critères d'arrêt immédiat

Courant anormalement élevé (défaut d'alim/soudure) → consigner.

## Données brutes attendues

Traces courant deep-sleep par config/support, horodatées ; SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

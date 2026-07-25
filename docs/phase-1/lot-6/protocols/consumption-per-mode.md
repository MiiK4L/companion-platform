<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L6-CONSO — Consommation par mode

> **Statut : Brouillon (en attente de baselining).** Essai **matériel + mesure
> fine**. [Conventions](README.md) · [définitions](event-definitions.md). Seuils
> **`[BL]`** ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L6-001`** (architecture) et **`DEC-L6-002`** (budget).

## Objectif (une question mesurable)

Quelle est la **consommation par mode** (deep-sleep, veille écran, actif, radio par
salves) **à chaque coin de tension `VBAT`** (4,2 / 3,7 / 3,4 / 3,0 V) ?

## Conditions

Modes figés (scénario de référence) ; mesure à chaque coin `VBAT` (alim
programmable) ; deep-sleep couplé à [L5](../../lot-5/deep-sleep-measurement.md).

## Seuils / grandeurs

| Grandeur | Cible | Étiquette |
|----------|-------|-----------|
| Conso par mode × coin `VBAT` | mesurée (entrée budget) | valeur `banc` |
| Conso moyenne pondérée | dérive le budget | **[BL]** (scénario) |

### Champs à finaliser au baselining (`[BL]`)

- **Scénario d'usage** (durées par mode) ; liste des modes ; coins `VBAT`.

## Plan d'essai (matériel + mesure fine)

- **`n_dut`** ≥ 2 · **`n_campaigns`** ≥ 2 · µA/mA-mètre (PPK2/Joulescope) · commit
  firmware exact ; méthode moyenne/percentiles par mode.

## Données brutes attendues

Traces courant par mode/coin, horodatées ; SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

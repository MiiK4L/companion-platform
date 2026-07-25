<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L8-FEAS — Faisabilité runtime (RAM/flash, install dynamique) — *spécifié, non exécuté*

> **Statut : Brouillon (en attente de baselining + POC embarqué).** **Hors
> périmètre du squelette host** : nécessite un POC ESP-IDF + runtime embarqué.
> Seuils `[BL]` ; **aucune `[M]`**. Risque **R7**.

## Décisions alimentées

**`DEC-L8-001`** (runtime) et **`DEC-L8-002`** (distribution) — via faisabilité
mesurée.

## Objectif (mesurable, ultérieur)

Le **runtime scripté candidat** embarqué tient-il dans le **budget RAM/flash par
app**, et l'**installation dynamique** (chargement depuis LittleFS) est-elle
**faisable** ?

## Grandeurs (seuils `[BL]`)

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| RAM par app (runtime candidat) | ≤ **`[BL]`** | **[BL]** |
| Flash par app | ≤ **`[BL]`** | **[BL]** |
| Chargement d'une app depuis LittleFS | démontré | `banc` |
| Faisabilité install dynamique (R7) | oui / **limites documentées** | **[BL]** |

## Repli (R7)

Install dynamique infaisable dans le budget → **recadrer vers bundle OTA** en
**gardant l'abstraction `IRuntime`** (cf. [distribution](../app-distribution-model.md)).

## Plan d'essai (type POC embarqué — ultérieur)

- POC ESP-IDF : boot → composition root **cible** → 1 port réel.
- Runtime candidat embarqué ; mesure RAM/flash par app ; chargement LittleFS.
- **Non exécuté dans cette PR** (hors périmètre host).

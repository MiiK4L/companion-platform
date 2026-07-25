<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 8 — Modèle de distribution des applications (*Proposé*, `DEC-L8-002`)

> **Statut : Ouvert / Proposé.** **Indépendant du runtime** (`DEC-L8-001`) : le
> modèle de distribution **ne présume pas** du moteur retenu. Valeurs `[H]/[BL]` ;
> **aucune `[M]`**. Risque **R7** (install dynamique). → **ADR future non réservée**.

## Approches comparées (au même niveau)

| Approche | Principe | Vigilance |
|----------|----------|-----------|
| **(D1) Installation dynamique « sans reflash »** | charger une app depuis le stockage (ex. LittleFS) à l'exécution | **faisabilité RAM/flash à démontrer (R7)** |
| **(D2) Bundle OTA** | apps embarquées dans une image mise à jour OTA | pas d'install « à chaud » ; mise à jour globale |
| **(D3) Hybride** | cœur OTA + extensions dynamiques | complexité |

## Critères de comparaison

| Critère | Nature |
|---------|--------|
| Faisabilité dans le budget RAM/flash | **[BL]** (mesure ultérieure, R7) |
| Sécurité / confiance (lien identification L3) | **[H]** |
| Expérience de mise à jour | **[H]** |
| Complexité firmware | **[H]** |
| Réversibilité / rollback | **[H]** |

## Indépendance vis-à-vis du runtime

> Le **format de distribution** (unité d'app opaque) transite par le port
> `IStorage` puis `IRuntime` : **changer de runtime ne change pas le modèle de
> distribution**, et **choisir un modèle de distribution n'impose aucun runtime**.

## Repli documenté (R7)

> Si l'**installation dynamique** s'avère **infaisable dans le budget**, **recadrer
> la promesse** vers le **bundle OTA** tout en **gardant l'abstraction de runtime**
> (`IRuntime`). Décision tracée le moment venu.

## Alimente

- **`DEC-L8-002`** — modèle de distribution arbitré **après** démonstration de
  faisabilité (R7). **Aucun modèle retenu.** **Indépendant** de `DEC-L8-001`.

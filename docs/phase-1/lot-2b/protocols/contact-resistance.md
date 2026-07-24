<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2B-RCONTACT — Résistance de contact (mesure 4 fils)

> **Statut : Brouillon (en attente de baselining + annexe fixture par famille).**
> Générique. [Conventions communes](README.md) ·
> [définitions d'événements](event-definitions.md). Seuils `[P]/[BL]` ; **aucune
> [M]**. Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 2B · alimente **`DEC-L2-002`** (résistance de contact et sa dérive).

## Objectif (une question mesurable)

La **résistance de contact** de chaque contact reste-t-elle dans sa borne, et sa
**dérive** reste-t-elle **< 50 mΩ tous les 100 cycles** au cours de l'endurance ?

## Méthode 4 fils (Kelvin)

Source de courant et mesure de tension **séparées**, appliquées **par contact
identifié** ; points de mesure définis dans l'**annexe fixture** de la famille
(créée à l'essai). Courant de mesure `[BL]` ; correction de la résistance des
amenées.

## Grandeurs (définitions non ambiguës)

Pour **chaque contact identifié** :

- **`R0`** — résistance de contact **initiale** (avant cyclage).
- **`Rn`** — résistance **au palier n** (ex. tous les 100 cycles).
- **`ΔR = Rn − R0`** — **dérive cumulée** depuis l'origine.
- **`ΔR_step = Rn − Rn₋₁`** — variation **entre deux paliers successifs**.
- **`ΔR% = ΔR / R0`** — dérive **relative**.

> ⚠️ La formulation « dérive < 50 mΩ /100 cycles » est **ambiguë** (elle
> autoriserait jusqu'à ~500 mΩ à 1000 cycles) et **n'est pas** utilisée comme
> verdict. Le verdict porte sur des grandeurs **explicites** ci-dessous.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| **Borne absolue** `Rn` (à tout palier) | ≤ `[BL]` mΩ | **[BL]** |
| **Dérive cumulée** `ΔR` (depuis `R0`, fin d'endurance) | ≤ `[BL]` mΩ | **[BL]** |
| **Dérive relative** `ΔR%` (optionnelle) | ≤ `[BL]` % | **[BL]** |
| **Variation inter-palier** `ΔR_step` (détection de saut) | ≤ `[BL]` mΩ | **[BL]** |
| Dispersion `R0` entre contacts | ≤ `[BL]` mΩ | **[BL]** |

### Champs à finaliser au baselining (`[BL]`)

Les seuils **ne se déduisent pas** de la seule valeur générique « 50 mΩ » : ils
sont fixés au baselining **à partir** (a) des **séries représentatives**
([comparaison des familles](../families-comparison.md)) et (b) de la **chute de
tension admissible** sur le rail concerné (courant × `Rn` ≤ marge du rail).

- **Borne absolue `Rn`**, **dérive cumulée `ΔR`** max, **`ΔR%`** éventuelle,
  **`ΔR_step`** max — chiffrés + justifiés (source/série + chute de tension).
- **Courant de mesure** 4 fils (A) et **incertitude** associée.

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 · **contacts** : **tous** les contacts mesurés (traçés
  individuellement) · **`n_runs`** = relevé tous les 100 cycles · **`n_campaigns`**
  ≥ 2 · couplé au [protocole d'endurance](endurance-insertion.md).
- **Méthode** : min/max/percentiles par contact ; règle d'aberrants pré-définie.

## Critères d'arrêt immédiat

R_contact hors borne franche répétée ; mesure 4 fils incohérente (contact des
amenées) ; échauffement au point de mesure.

## Remise en état / éléments destructibles

Non destructif en soi, mais couplé à l'endurance (usure). Re-vérifier
l'étalonnage 4 fils et la propreté des points de contact entre campagnes
(nettoyage **selon la règle figée**, cf. conventions).

## Données brutes attendues

R_contact par contact et par palier de 100 cycles, incertitude, température ;
SHA-256 par fichier au rapport.

## Sécurité opérateur

Faibles tensions ; précautions ESD standard ; attention aux pointes de mesure.

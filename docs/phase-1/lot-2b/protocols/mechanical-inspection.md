<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2B-INSPECT — Inspection mécanique

> **Statut : Brouillon (en attente de baselining + annexe fixture par famille).**
> Générique. [Conventions communes](README.md) ·
> [définitions d'événements](event-definitions.md). Critères `[P]/[BL]` ; **aucune
> [M]**. Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 2B · alimente **`DEC-L2-002`** (intégrité mécanique après endurance).

## Objectif (une question mesurable)

Après endurance, les contacts et le corps du connecteur présentent-ils une
**usure**, **déformation**, **oxydation** ou **contamination** corrélée à une
**dérive de résistance** ou à une **perte de rétention** ?

## Méthode

Inspection visuelle **avant / pendant (paliers) / après** endurance, au
**grossissement** défini (`[BL]`) ; photos de référence par contact identifié ;
corrélation avec les relevés [4 fils](contact-resistance.md) et
[force](insertion-extraction-force.md).

## Critères (qualitatifs + corrélés)

| Observation | Critère | Étiquette |
|-------------|---------|-----------|
| Usure / marquage des contacts | pas de perte de matière compromettant le contact | **[P]** |
| Déformation / jeu | dans tolérance mécanique `[BL]` | **[BL]** |
| Oxydation / contamination | absence, **ou** présence **corrélée** à une dérive R | **[P]** |
| Intégrité du corps / détrompeur | pas de fissure ni perte de fonction | **[P]** |

### Champs à finaliser au baselining (`[BL]`)

- **Grossissement** et méthode photographique ; **paliers** d'inspection.
- **Tolérances** de déformation/jeu ; grille de cotation de l'usure.
- **Règle de nettoyage** avant inspection (autorisé/interdit) — cf. conventions.

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 · inspection à `[BL]` paliers (ex. 0 / 500 / 1000 cycles) ·
  **`n_campaigns`** ≥ 2 ; **tous** les contacts photographiés et tracés.

## Critères d'arrêt immédiat

Défaillance mécanique visible (fissure, perte de rétention) → arrêt et
consignation.

## Remise en état / éléments destructibles

Non destructif ; peut nécessiter le **démontage** (destructif) en fin de campagne
pour inspection interne — à documenter comme tel.

## Données brutes attendues

Photos horodatées par contact et par palier, grille de cotation, corrélation
R/force ; SHA-256 par fichier au rapport.

## Sécurité opérateur

Précautions standard ; outils de démontage ; ESD lors de la manipulation.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L4-PERCU — Performances perçues

> **Statut : Brouillon (en attente de baselining).** Essai **matériel + code**.
> Couvre les critères de **performance perçue**. [Conventions](README.md) ·
> [définitions](event-definitions.md). Seuils `[P]/[BL]` ; **aucune `[M]`**.
> Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 4 · alimente **`DEC-L4-001`** (matériel : réveil/init/tearing) et
**`DEC-L4-002`** (moteur : fluidité/stabilité/latence).

## Objectif (une question mesurable)

L'expérience perçue est-elle **acceptable** : réveil et initialisation rapides,
**pas de tearing** hors spec, animations **fluides et stables**, latence d'entrée
bornée ?

## Grandeurs mesurées (instrumentées — voir [définitions](event-definitions.md))

| Grandeur | Seuil | Étiquette | Méthode |
|----------|-------|-----------|---------|
| **Temps de réveil** (veille → 1ʳᵉ frame valide) | ≤ `[BL]` ms | **[P]/[BL]** | horodatage réveil→frame |
| **Temps d'initialisation** (boot → écran opérationnel) | ≤ `[BL]` ms | **[P]/[BL]** | horodatage boot→frame |
| **Tearing** | 0 hors spec | **[P]/[BL]** | **caméra haute vitesse** / signal TE |
| **Fluidité** (FPS effectif ressenti) | ≥ ≈ 30 fps | **[P]** | cf. [FPS](rendering-fps.md) |
| **Stabilité d'animation** (jitter inter-frame) | ≤ `[BL]` ms d'écart-type | **[P]/[BL]** | intervalles inter-frames |
| **Latence perçue** (entrée → réaction), si applicable | ≤ `[BL]` ms | **[P]/[BL]** | horodatage entrée→frame |

### Champs à finaliser au baselining (`[BL]`)

- Seuils **ms** (réveil, init, latence, jitter) ; **définition du « hors spec »**
  de tearing ; méthode d'observation de sortie (caméra/TE).

## Plan d'essai (type **matériel + code**)

- **`n_dut`** ≥ 2 panneaux · scénarios d'animation/entrée figés · **`n_campaigns`**
  ≥ 2 · commit firmware exact ; **observation de la sortie** obligatoire (pas
  seulement compteur de frames).

## Données brutes attendues

Captures caméra/TE + journaux d'horodatage (réveil, init, entrée→frame, jitter) ;
SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

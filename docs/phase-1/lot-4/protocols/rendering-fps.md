<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L4-FPS — FPS / rendu

> **Statut : Brouillon (en attente de baselining).** Essai **matériel + code**.
> [Conventions](README.md) · [définitions](event-definitions.md). Seuils
> `[P]/[BL]` ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 4 · alimente **`DEC-L4-002`** (débit de rendu vs cible).

## Objectif (une question mesurable)

Sous des **scénarios d'UI définis** (défilement, animation, plein écran), le
couple **moteur × contrôleur × interface** atteint-il **≈ 30 fps** dans le budget
mémoire ?

## Scénarios (figés)

Liste versionnée : rafraîchissement plein écran ; défilement de liste ; animation
de widget ; transition d'écran. Interface **SPI** et/ou **QSPI** selon panneau.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| FPS (par scénario) | ≥ ≈ 30 fps | **[P]** |
| FPS sous trafic module (SPI partagé) | ≥ cible | **[P]/[BL]** (cf. [contention](spi-contention.md)) |

### Champs à finaliser au baselining (`[BL]`)

- **Scénarios** exacts + **cible FPS** par scénario ; débit d'interface.

## Plan d'essai (type **matériel + code**)

- **`n_dut`** ≥ 2 panneaux (par famille évaluée) · **stratégie de buffers** ×
  **scénario** · **`n_campaigns`** ≥ 2 · commit firmware exact.
- **Méthode** : min/max/percentiles du FPS ; règle d'aberrants pré-définie.

## Données brutes attendues

Traces FPS par scénario/config/panneau, horodatées ; SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

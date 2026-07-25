<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L4-RAM — Budget mémoire du moteur graphique

> **Statut : Brouillon (en attente de baselining).** Essai **logiciel**.
> [Conventions](README.md) · [définitions](event-definitions.md). Seuils
> `[P]/[BL]` ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 4 · alimente **`DEC-L4-002`** (stratégie mémoire du moteur vs budget).

## Objectif (une question mesurable)

Pour une **stratégie de buffers** donnée (partiel/direct/double), l'empreinte RAM
du moteur **tient-elle dans le budget** (dérivé de [L1](../../lot-1/budgets-v0.md))
à la **résolution/profondeur** de référence ?

## Configurations testées

Croisement **stratégie** (partiel 1/10, partiel 1/4, direct, double buffer) ×
**résolution/profondeur** (issues des [familles](../display-families-comparison.md)).

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| RAM du moteur (par config) | ≤ budget `[BL]` Kio | **[P]/[BL]** |
| Framebuffer (calcul) | cohérent avec [budget mémoire](../memory-and-fps-budget.md) | **[C]** |

### Champs à finaliser au baselining (`[BL]`)

- **Budget RAM cible** (Kio), **résolution/profondeur** de référence, cœur retenu.

## Plan d'essai (type **logiciel**)

- **Build/config versionnés** (commit exact) ; mesure d'empreinte (map/heap) par
  configuration ; **versions du moteur** documentées ; répétabilité du build.
- **Pas de `n_dut` matériel** (mesure d'empreinte, non de panneau).

## Données brutes attendues

Rapports d'empreinte (RAM statique/dynamique) par config, horodatés ; SHA-256 au
rapport.

## Sécurité opérateur

Essai logiciel ; sans risque électrique.

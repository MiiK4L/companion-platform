<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L6-CHRG — Charge complète & thermique

> **Statut : Brouillon (en attente de baselining).** Essai **matériel**.
> [Conventions](README.md) · [définitions](event-definitions.md). **Seuils `[BL]`** ;
> **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L6-001`** (architecture de charge) et **`DEC-L6-002`** (batterie).

## Objectif (une question mesurable)

La **charge complète** s'effectue-t-elle avec un **échauffement (ΔT)** dans une
borne, sans dépassement de courant/tension ?

## Conditions

Charge d'une cellule déchargée jusqu'à pleine charge ; **système alimenté**
(power-path) ou non selon l'architecture testée ; surveillance thermique.

## Seuils / grandeurs

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| ΔT thermique max en charge | ≤ **`[BL]`** | **[BL]** |
| Courant/tension de charge | dans plage sûre de la cellule **[DS]** | **[DS]/[BL]** |
| Temps de charge complet | mesuré | valeur `banc` |

### Champs à finaliser au baselining (`[BL]`)

- ΔT admissible ; courant de charge ; définition de « pleine charge » ;
  points de mesure thermiques.

## Plan d'essai (matériel)

- **`n_dut`** ≥ 2 cellules · **`n_campaigns`** ≥ 2 · thermocouple/caméra IR + alim
  mesurée ; **limitation** de courant impérative.

## Critères d'arrêt immédiat

Échauffement anormal, gonflement de cellule, dépassement courant/tension → arrêt.

## Données brutes attendues

Courbes courant/tension/température de charge, horodatées ; SHA-256 au rapport.

## Sécurité opérateur

**Manipulation batterie** : bac ininflammable, surveillance, limitation
courant/tension, protection thermique ; ne jamais laisser une charge sans
surveillance.

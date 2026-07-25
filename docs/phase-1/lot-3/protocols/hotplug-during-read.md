<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L3-HOTPLUG — Hot-plug pendant la lecture d'identité

> **Statut : Brouillon (en attente de baselining).** [Conventions](README.md) ·
> [définitions d'événements](event-definitions.md). Réutilise le **banc
> d'injection** du [Lot 2A](../../lot-2/hot-plug.md) (électrique). Seuils
> `[P]/[BL]` ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 3 · alimente **`DEC-L3-001`** (robustesse au hot-plug pendant l'identification).

## Objectif (une question mesurable)

Si le module est **inséré ou retiré pendant la lecture d'identité/Manifest**, le
Host termine-t-il par un **échec propre** (retour « aucun module ») **sans crash
ni blocage**, et sans **identité partielle réputée fiable** ?

## Conditions

Banc d'injection (Lot 2A) déclenchant insertion/retrait **à des phases variées**
de la lecture (avant présence, pendant l'identité, pendant le Manifest) ; lecture
d'ID **avant** alimentation complète (séquencement candidat, cf.
[compat](../low-power-hotplug-compat.md)).

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Crash / blocage Host | **0** sur ≥ `[BL]` essais | **[P]/[BL]** |
| Identité partielle réputée fiable | **0** | **[P]** |
| Retour à l'état « aucun module » | 100 % après retrait | **[P]** |
| Reprise d'une lecture correcte après ré-insertion | 100 % | **[P]** |

### Champs à finaliser au baselining (`[BL]`)

- **Nombre d'essais** par phase ; **phases** d'interruption (≥ 3).
- Lien avec le **séquencement candidat** (2B) — à confirmer, non figé.

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 · interruptions réparties sur ≥ 3 phases · **`n_runs`** ≥ `[BL]`
  par phase · **`n_campaigns`** ≥ 2.

## Critères d'arrêt immédiat

Crash/blocage répété non récupérable → consigner.

## Données brutes attendues

Journaux (phase d'interruption → état), captures bus + présence ; SHA-256 au rapport.

## Sécurité opérateur

Banc d'injection : limitation de courant, ESD standard (cf. sécurité Lot 2A).

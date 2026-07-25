<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L3-CORRUPT — Corruption & lecture partielle

> **Statut : Brouillon (en attente de baselining).** [Conventions](README.md) ·
> [définitions d'événements](event-definitions.md). Seuils `[P]/[BL]` ; **aucune
> `[M]`**. Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 3 · alimente **`DEC-L3-001`** (comportement en corruption / identité partielle)
et **`DEC-L3-002`** (mode dégradé du Manifest).

## Objectif (une question mesurable)

Face à une **corruption** (CRC invalide), une **lecture partielle** ou une
**identité partiellement lisible**, le Host **rejette / dégrade** correctement et
**ne complète jamais** l'information manquante ?

## Injection de défauts

Corruption de bits (mémoire/bus), coupure en cours de lecture (partiel), octets
manquants, CRC faux, identité tronquée, **écriture / mise à jour du Manifest
interrompue** (coupure en cours d'écriture — cf.
[atomicité](../manifest-format.md)).

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| CRC invalide → Manifest rejeté | 100 % | **[P]** |
| Lecture partielle → rejet/dégradé (jamais « deviné ») | 100 % | **[P]** |
| Identité partielle traitée comme fiable | **0** | **[P]** |
| Capacités publiées sur Manifest corrompu | **0** | **[P]** |
| État exposé correct (illisible/dégradé) | 100 % | **[P]** |
| **MAJ interrompue distinguée d'une corruption permanente** | 100 % | **[P]** |
| **Retour à la dernière copie valide** — **uniquement si** une stratégie A/B/génération est **réellement dans la campagne** | 100 % | **[P]/[BL]** (conditionnel) |

> Les **stratégies d'atomicité candidates non retenues** (cf.
> [Manifest §5](../manifest-format.md)) **ne deviennent pas** des critères
> obligatoires : le « retour à la dernière copie valide » n'est évalué **que si**
> la campagne teste effectivement une telle stratégie.

### Champs à finaliser au baselining (`[BL]`)

- **Jeu de défauts** figé ; **définition du mode dégradé** (ce qui reste permis).
- **Périmètre CRC** et politique de rejet (cf. [Manifest](../manifest-format.md)).

## Plan de couverture (par **type** d'essai)

Ce protocole mêle des cas de **natures différentes** : la couverture est **séparée
par type** (cf. [types d'essai](README.md)).

| Sous-plan | Nature | Couverture |
|-----------|--------|-----------|
| **Vecteurs logiciels** (corruption de trame, CRC faux, lecture partielle) | logiciel | **corpus versionné** + **mutations**, nombre de cas + campagnes (commit exact) |
| **Injections bus** (corruption au niveau bus) | matériel | **fixture** + `n_dut` ≥ 2 + répétitions |
| **Coupures pendant écriture** | matériel | **DUT physiques**, **phases d'injection** (début/milieu/fin d'écriture), **n_cycles** |
| **Stratégie A/B** (retour copie valide) | conditionnel | **seulement si** la stratégie entre réellement dans la campagne |

## Données brutes attendues

Journaux (défaut injecté → état résultant), captures bus ; SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

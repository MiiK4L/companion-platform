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
manquants, CRC faux, identité tronquée.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| CRC invalide → Manifest rejeté | 100 % | **[P]** |
| Lecture partielle → rejet/dégradé (jamais « deviné ») | 100 % | **[P]** |
| Identité partielle traitée comme fiable | **0** | **[P]** |
| Capacités publiées sur Manifest corrompu | **0** | **[P]** |
| État exposé correct (illisible/dégradé) | 100 % | **[P]** |

### Champs à finaliser au baselining (`[BL]`)

- **Jeu de défauts** figé ; **définition du mode dégradé** (ce qui reste permis).
- **Périmètre CRC** et politique de rejet (cf. [Manifest](../manifest-format.md)).

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 · **vecteurs de corruption** figés · **`n_runs`** = vecteurs ×
  répétitions · **`n_campaigns`** ≥ 2.

## Données brutes attendues

Journaux (défaut injecté → état résultant), captures bus ; SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

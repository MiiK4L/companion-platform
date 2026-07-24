<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L3-IDREAD — Détection & lecture d'identité

> **Statut : Brouillon (en attente de baselining).** [Conventions](README.md) ·
> [définitions d'événements](event-definitions.md). Seuils `[P]/[BL]` ; **aucune
> `[M]`**. Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 3 · alimente **`DEC-L3-001`** (détection de présence + lecture d'identité +
résolution non ambiguë).

## Objectif (une question mesurable)

Le Host détecte-t-il la **présence**, lit-il l'**identité** (produit/instance) et
**résout-il sans ambiguïté** le module présent, dans un délai borné ?

## Conditions & instrumentation

Module de test (architecture évaluée) ; Host de test (firmware + commit exact) ;
analyseur de bus (I²C / 1-Wire) ; lecture d'ID **sans alimenter le reste du
module** si l'architecture le permet.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| **Découvrabilité logique** (réponse ID ; **≠ présence physique**) | 100 % (module inséré) | **[P]** |
| Lecture d'identité valide | 100 % (CRC/validation OK) | **[P]** |
| Résolution ambiguë non gérée | 0 | **[P]** |
| Temps de détection + lecture | ≤ `[BL]` ms | **[P]/[BL]** |

### Champs à finaliser au baselining (`[BL]`)

- **Délai borné** de détection/lecture (ms) ; **débit bus** (I²C 100/400 kHz…).
- **Procédure de résolution** par bus (recherche ROM 1-Wire, adresses I²C, énumération µC).

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 (exemplaires) · **`n_runs`** ≥ `[BL]` lectures · **`n_campaigns`**
  ≥ 2 · par **architecture** évaluée.
- **Méthode** : taux de succès + distribution des délais (min/max/percentiles).

## Données brutes attendues

Captures bus + journaux Host (présence, identité, délais), horodatés ; SHA-256 au
rapport.

## Sécurité opérateur

Faibles tensions ; précautions ESD standard.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 3 — Définitions instrumentées des événements (identification)

> **Statut : Brouillon.** Référentiel **commun** aux
> [protocoles d'identification](README.md). Chaque événement est **observable et
> mesurable**. Valeurs `[BL]` ; **aucune `[M]`**.

## Événements et définition instrumentée

| Événement | Définition instrumentée | Observable | Seuil |
|-----------|-------------------------|------------|-------|
| **Présence détectée** | réponse au niveau bus (ACK I²C / présence 1-Wire / réponse µC) | analyseur de bus + firmware Host | binaire |
| **Identité lue & valide** | trame d'identité lue **et** CRC/validation OK | log Host + capture bus | conforme au format |
| **CRC invalide (corruption)** | CRC calculé ≠ CRC lu (périmètre défini) | firmware Host | rejet |
| **Lecture partielle** | longueur lue < longueur attendue **ou** trame tronquée | log + capture | rejet / dégradé |
| **Taille annoncée incohérente** | longueur déclarée > **limite Host** | firmware Host | rejet **sans allocation** |
| **Résolution ambiguë / conflit** | ≥ 2 réponses, adresse non conforme, collision | analyseur de bus | 0 ambiguïté non gérée |
| **Doublon d'identité** | identité déjà présente au **registre** de provisioning | registre production | refus |
| **Absence** | pas de présence dans un délai borné | timeout Host | ≤ `[BL]` ms |
| **Réponse lente / bloqué** | dépassement du délai borné (clock-stretch, no-release) | timeout + analyseur | ≤ `[BL]` ms puis échec |
| **Interruption hot-plug** | perte de présence pendant une transaction | présence + analyseur | échec **propre** (pas de blocage) |

## Règles instrumentées

- **Jamais de complétion « devinée »** : une identité **incomplète** = **non
  fiable** (état d'échec explicite).
- **Parser borné** : toute longueur déclarée est **plafonnée** avant lecture /
  allocation (cf. [Manifest](../manifest-format.md)).
- **États exposés** distincts : *aucun module* / *reconnu* / *inconnu lisible* /
  *illisible-défaillant* (cf. [pannes & UX](../failure-and-ux.md)).
- **Corrélation** : chaque événement horodaté et rattaché au DUT + à la capture bus.

## Renvois

- [Index des protocoles & cycle de vie](README.md) · [Cadre de validation](../../validation-framework.md)

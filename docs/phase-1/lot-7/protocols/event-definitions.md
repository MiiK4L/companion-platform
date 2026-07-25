<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 7 — Définitions instrumentées des événements (temps & persistance)

> **Statut : Brouillon.** Référentiel **commun** aux
> [protocoles](README.md). Chaque événement est **observable et mesurable**.
> **Seuils `[BL]`** ; **aucune `[M]`**.

## Événements et définition instrumentée

| Événement | Définition instrumentée | Observable | Seuil |
|-----------|-------------------------|------------|-------|
| **Dérive** | écart d'horloge vs **référence de temps** sur une durée | comparaison à une référence (GPS/atomic/labo) | ≤ **[BL]** ppm |
| **Consommation base de temps** | courant du RTC (+ sauvegarde) | µA/nA-mètre | ≤ **[BL]** |
| **Erreur de Δt au réveil** | écart entre Δt réel et Δt calculé après deep-sleep | référence + horodatage | 0 sur `[BL]` cycles |
| **Heure invalide non signalée** | heure présentée comme « valide » alors qu'inconnue/fausse | état de validité firmware | **0** (interdit) |
| **Corruption non détectée (persistance)** | état restauré incohérent sans détection | checksum/génération | **0** |
| **Reprise après coupure brutale** | retour à un état cohérent après power-off non contrôlé | vérification checksum + état | 100 % |

## Notes

- **Heure fausse marquée « valide »** = **échec** absolu : le firmware doit exposer
  **inconnue/resynchronisée** plutôt que fabriquer une heure (cf.
  [validité](../timebase-comparison.md)).
- **Distinguer** coupure d'écriture (récupérable) et **corruption** permanente
  (marqueur/génération, cf. [persistance](../persistence-comparison.md)).
- Corrélation : chaque événement horodaté, rattaché au DUT + config + commit
  firmware ; SHA-256 au rapport.

## Renvois

- [Index des protocoles & cycle de vie](README.md) · [Cadre de validation](../../validation-framework.md)

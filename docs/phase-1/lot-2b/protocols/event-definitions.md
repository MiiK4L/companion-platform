<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2B — Définitions instrumentées des événements (mécaniques)

> **Statut : Brouillon.** Référentiel **commun** aux
> [protocoles connecteur](README.md). Chaque événement est **observable et
> mesurable**. Valeurs `[BL]` finalisées au baselining ; **aucune [M]**.

## Événements et définition instrumentée

| Événement | Définition instrumentée | Observable / capteur | Seuil |
|-----------|-------------------------|----------------------|-------|
| **Cycle d'insertion** | 1 insertion complète + 1 retrait complet, engagement/désengagement vérifiés | Compteur mécanique + capteur de position | comptage |
| **Dérive de résistance de contact** | `ΔR = Rn − R0` (cumulée) et `Rn` (absolue) du **même contact** ; définitions dans le [protocole R_contact](contact-resistance.md) | **Mesure 4 fils** (Kelvin), par contact | borne absolue `Rn` + dérive cumulée `ΔR` **[BL]** (pas « 50 mΩ/100 cycles ») |
| **Intermittence** | Ouverture transitoire d'un contact sous charge | Détecteur d'intermittence (seuil de tension/impédance, fenêtre temporelle) | événement > **1 µs** **[P]** |
| **Défaillance de contact** | R_contact hors borne **ou** intermittence répétée **ou** perte de continuité | 4 fils + détecteur d'intermittence + continuité | R > `[BL]` **ou** ≥ `[BL]` intermittences |
| **Défaillance mécanique** | Perte de rétention, déformation, fissure, jeu excessif | Inspection + mesure de force de rétention | force < `[BL]` N **ou** défaut visuel |
| **Contamination / oxydation** | Dépôt ou oxydation modifiant R_contact | Inspection (grossissement) + corrélation 4 fils | qualitatif + corrélation R |

## Règle après intermittence (figée avant essai)

- Une intermittence **isolée** (< `[BL]` occurrences) est **comptée et
  journalisée** ; le cyclage **se poursuit**.
- Au-delà de `[BL]` intermittences (ou une intermittence > durée `[BL]`) →
  **défaillance de contact** : **arrêt** de la série sur ce contact, consignation,
  pas de reprise rétroactive.
- Toute intermittence est **corrélée** au numéro de cycle et au contact concerné.

## Traçabilité (obligatoire)

- **DUT** et **contact** identifiés par une **numérotation stable** ; chaque
  relevé 4 fils, chaque intermittence et chaque inspection y sont rattachés.
- Base de temps / index de cycle **commun** aux mesures électriques et mécaniques.
- Données brutes conservées dans le dossier du lot ; **SHA-256 par fichier** au rapport.

## Renvois

- [Index & conventions communes](README.md) · [Cadre de validation](../../validation-framework.md)

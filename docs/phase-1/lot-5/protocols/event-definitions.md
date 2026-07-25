<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 5 — Définitions instrumentées des événements (cœur & support)

> **Statut : Brouillon.** Référentiel **commun** aux
> [protocoles cœur/socket](README.md). Chaque événement est **observable et
> mesurable**. Valeurs `[BL]` ; **aucune `[M]`**.

## Événements et définition instrumentée

| Événement | Définition instrumentée | Observable | Seuil |
|-----------|-------------------------|------------|-------|
| **Courant deep-sleep** | courant système en veille profonde | µA-mètre (PPK2/Joulescope) | ≤ cible L6 **[BL]** |
| **Contribution du support à la conso** | écart socketé − soudé | µA-mètre, même config | ≤ `[BL]` |
| **Résistance de contact** | R d'un contact du support | **mesure 4 fils**, par contact | ≤ `[BL]` mΩ |
| **Dérive de contact** | `ΔR = Rn − R0` (cumulée) et `Rn` (absolue) | 4 fils, même contact | borne + dérive **[BL]** |
| **Intermittence sous vibration** | ouverture transitoire d'un contact sous charge | détecteur d'intermittence (résolution ≤ 1/10 du seuil) | 0 > `[BL]` µs |
| **Perte d'alignement** | désalignement du cœur dans le support | mesure/inspection | ≤ `[BL]` |
| **Déformation / détérioration du support** | déformation, marquage, perte de rétention | inspection (grossissement) + force de rétention | défaut visuel / force < `[BL]` N |
| **GPIO indéterminé (cœur absent/éteint)** | ligne flottante hors état défini | analyseur logique + mesure | 0 état nuisible |
| **Back-powering (cœur éteint)** | ré-alimentation parasite via bus | tension/courant sur rail cœur | ≤ `[BL]` |

## Notes

- **Épaisseur ajoutée** : mesure dimensionnelle (pied à coulisse/comparateur),
  consignée par support.
- Corrélation : chaque événement horodaté, rattaché au **support (DUT)** + à la
  **carte cœur** utilisée + au numéro de cycle/remplacement ; SHA-256 au rapport.

## Renvois

- [Index des protocoles & cycle de vie](README.md) · [Cadre de validation](../../validation-framework.md)

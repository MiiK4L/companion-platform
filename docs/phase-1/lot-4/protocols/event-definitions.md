<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 4 — Définitions instrumentées des événements (affichage)

> **Statut : Brouillon.** Référentiel **commun** aux
> [protocoles d'affichage](README.md). Chaque événement est **observable et
> mesurable**. Valeurs `[BL]` ; **aucune `[M]`**.

## Événements et définition instrumentée

| Événement | Définition instrumentée | Observable | Seuil |
|-----------|-------------------------|------------|-------|
| **Image rendue (frame)** | trame complète rendue puis envoyée | compteur firmware + signal GPIO en fin de frame | comptage |
| **FPS** | frames envoyées par seconde sur un scénario | mesure sur fenêtre temporelle | ≈ 30 fps **[P]** |
| **Tearing** | déchirure : rafraîchissement pendant l'envoi | **caméra haute vitesse** / signal de sync (TE) | 0 hors spec **[BL]** |
| **Temps de réveil** | veille → premier affichage cohérent | horodatage réveil → frame valide | ≤ `[BL]` ms |
| **Temps d'initialisation** | mise sous tension → écran opérationnel | horodatage boot → 1ʳᵉ frame | ≤ `[BL]` ms |
| **Stabilité d'animation** | variance/jitter du temps entre frames | écart-type des intervalles | ≤ `[BL]` ms |
| **Latence perçue** (si applicable) | événement d'entrée → réaction affichée | horodatage entrée → frame | ≤ `[BL]` ms |
| **Glitch écran (contention SPI)** | corruption visible lors d'accès simultané | caméra/CRC framebuffer + compteur erreurs SPI | 0 hors spec |
| **RAM du moteur** | empreinte mémoire mesurée | traces build/firmware | ≤ budget `[BL]` |

## Notes

- **Tearing** et **stabilité d'animation** exigent une **observation de la sortie**
  (caméra/signal TE), pas seulement un compteur de frames (cf. principe établi en
  L2A : sortie affichée ≠ état interne).
- Corrélation : chaque événement horodaté, rattaché au DUT (panneau) + config
  moteur + commit firmware ; données brutes SHA-256 au rapport.

## Renvois

- [Index des protocoles & cycle de vie](README.md) · [Cadre de validation](../../validation-framework.md)

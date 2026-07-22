<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Guide — Charte graphique

Ce guide définit la charte graphique de Companion Platform : comment le
personnage/mascotte agit comme interface, la palette de couleurs, les
conventions d'animation, et la cohérence visuelle attendue à l'écran. Le
principe accepté est une **interface graphique abstraite** (rendu de
widgets/sprites, aucune app ne dépend du contrôleur) ; l'écran **TFT IPS**
et le moteur graphique **LVGL** sont **pressentis** (à valider en Phase 1/2,
voir ADR-0003).

## 1. Principes directeurs

- À compléter — Phase 2 : philosophie générale de l'interface (le
  personnage/mascotte comme point d'entrée central de l'expérience,
  cohérence entre apps malgré la diversité des cas d'usage — Tamagotchi,
  outils réseau, homelab, etc.).
- Rappel : le personnage/mascotte **officiel** relève de l'identité
  visuelle réservée du projet — voir [`LICENSING.md`](../../LICENSING.md)
  et `assets/README.md`. Ce guide décrit la charte à respecter, pas une
  licence d'utilisation de la mascotte elle-même.

## 2. Le personnage comme interface

- À compléter — Phase 2 : rôle du personnage dans la navigation
  (feedback d'état, réactions contextuelles, transition entre apps),
  limites de ce qu'il peut exprimer selon la taille d'écran disponible.

## 3. Palette de couleurs

- À compléter — Phase 2 : palette officielle (couleurs primaires,
  secondaires, couleurs d'état — succès/alerte/erreur), contraintes liées
  à l'écran (TFT IPS pressenti : gamme de couleurs, lisibilité au soleil).

## 4. Animations

- À compléter — Phase 2 : conventions de framerate et de durée pour les
  animations du personnage et de l'interface, format de sprite attendu
  (voir `assets/README.md`), budget mémoire/CPU indicatif par animation.

## 5. Cohérence de l'interface (moteur graphique pressenti : LVGL)

- À compléter — Phase 2 : composants standards à privilégier (widgets,
  thèmes — LVGL pressenti), règles d'espacement et de typographie, comment une
  app tierce peut rester visuellement cohérente sans dupliquer les
  assets officiels.

## 6. Ce que les apps tierces peuvent/doivent faire

- À compléter — Phase 3 : recommandations pour qu'une app tierce
  s'intègre visuellement sans utiliser l'identité visuelle réservée du
  projet (mascotte officielle, logo) sans autorisation.

## Voir aussi

- [`assets/README.md`](../../assets/README.md)
- [`LICENSING.md`](../../LICENSING.md)
- `docs/adr/` (ADR-0003)

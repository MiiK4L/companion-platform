# Guide — Charte graphique

Ce guide définit la charte graphique de Companion Platform : comment le
personnage/mascotte agit comme interface, la palette de couleurs, les
conventions d'animation, et la cohérence visuelle attendue sur l'écran TFT
IPS piloté par **LVGL** (voir ADR-0003).

## 1. Principes directeurs

- À compléter — Phase 2 : philosophie générale de l'interface (le
  personnage/mascotte comme point d'entrée central de l'expérience,
  cohérence entre apps malgré la diversité des cas d'usage — Tamagotchi,
  outils réseau, homelab, etc.).
- Rappel : le personnage/mascotte **officiel** relève de l'identité
  visuelle réservée du projet — voir [`LICENSING.md`](https://github.com/MiiK4L/companion-platform/blob/main/LICENSING.md)
  et `assets/README.md`. Ce guide décrit la charte à respecter, pas une
  licence d'utilisation de la mascotte elle-même.

## 2. Le personnage comme interface

- À compléter — Phase 2 : rôle du personnage dans la navigation
  (feedback d'état, réactions contextuelles, transition entre apps),
  limites de ce qu'il peut exprimer selon la taille d'écran disponible.

## 3. Palette de couleurs

- À compléter — Phase 2 : palette officielle (couleurs primaires,
  secondaires, couleurs d'état — succès/alerte/erreur), contraintes liées
  à l'écran TFT IPS cible (gamme de couleurs, lisibilité au soleil).

## 4. Animations

- À compléter — Phase 2 : conventions de framerate et de durée pour les
  animations du personnage et de l'interface, format de sprite attendu
  (voir `assets/README.md`), budget mémoire/CPU indicatif par animation.

## 5. Cohérence UI LVGL

- À compléter — Phase 2 : composants LVGL standards à privilégier
  (widgets, thèmes), règles d'espacement et de typographie, comment une
  app tierce peut rester visuellement cohérente sans dupliquer les
  assets officiels.

## 6. Ce que les apps tierces peuvent/doivent faire

- À compléter — Phase 3 : recommandations pour qu'une app tierce
  s'intègre visuellement sans utiliser l'identité visuelle réservée du
  projet (mascotte officielle, logo) sans autorisation.

## Voir aussi

- [`assets/README.md`](https://github.com/MiiK4L/companion-platform/blob/main/assets/README.md)
- [`LICENSING.md`](https://github.com/MiiK4L/companion-platform/blob/main/LICENSING.md)
- `docs/adr/` (ADR-0003)

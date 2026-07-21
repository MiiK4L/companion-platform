# `assets/` — Ressources graphiques

Ce dossier rassemble les ressources graphiques utilisées par les apps et
l'interface système : sprites, animations et rendus destinés à être
affichés sur l'écran TFT IPS via LVGL (voir ADR-0003).

## Ce qui va ici

- Sprites et feuilles d'animation utilisés par les apps officielles et le
  système (icônes, curseurs, transitions d'interface).
- Rendus (exports d'illustrations sources) prêts à être intégrés dans une
  app ou l'OS, dans un format compatible LVGL.
- Ressources graphiques **génériques**, réutilisables par plusieurs apps
  (par exemple une bibliothèque d'icônes système).

## Ce qui ne va pas ici

- Les assets propres à une seule app (sprites spécifiques au Tamagotchi,
  par exemple) : ils vivent avec l'app concernée dans `apps/<app>/`.
- Le code de rendu ou d'animation lui-même : voir `firmware/` (moteur
  LVGL, services graphiques) ou l'app concernée.

## Attention : licence de l'identité visuelle

**Ce dossier ne suit pas une licence unique.** La plupart des assets
graphiques génériques peuvent être publiés sous **CC-BY-4.0**, comme le
reste de la documentation et des ressources ouvertes du projet.

En revanche, tout ce qui constitue l'**identité visuelle officielle** de
Companion Platform — logo, mascotte officielle, illustrations de marque —
reste **réservé** (tous droits réservés), et ne doit pas être confondu
avec des assets génériques réutilisables. Avant d'ajouter un asset ici,
vérifiez s'il relève de l'identité de marque ou d'une ressource
générique, et documentez-le en conséquence (en-tête ou fichier
`LICENSE` local). Voir [`LICENSING.md`](../LICENSING.md) pour le détail
de la stratégie multi-licences.

## Phase de roadmap

Ce dossier se remplit au fur et à mesure que les apps officielles
(Tamagotchi en premier) et le système définissent leurs besoins
graphiques concrets, en cohérence avec la charte définie dans
`docs/guides/graphic-style.md`.

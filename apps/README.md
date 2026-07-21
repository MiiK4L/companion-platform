# `apps/` — Applications Companion Platform

Ce dossier contient les **apps** de la plateforme : des programmes
utilisateur écrits en **Lua**, exécutés par le runtime hybride au-dessus du
**Companion SDK** (voir ADR-0002 pour le runtime, ADR-0001/0007 pour
l'inversion des dépendances matérielles). Une app ne dépend jamais
directement d'ESP-IDF, de FreeRTOS ni de la HAL : elle passe exclusivement
par les API exposées par le Companion SDK.

## Organisation

Chaque app vit dans son propre sous-dossier, nommé d'après l'app :

```
apps/
  tamagotchi/     # première app officielle de la plateforme
  <autre-app>/
```

Le **Tamagotchi** est la première app de la plateforme, pas une
exception : toute nouvelle app (réseau, homelab, électronique, capteurs,
jeux...) suit la même structure et les mêmes règles.

## Ce qui va ici

- Le code Lua de l'app et son manifest (métadonnées, permissions,
  ressources requises — voir `docs/guides/app-development.md`).
- Les assets propres à l'app (sprites, sons) qui ne sont pas partagés.
- La documentation spécifique à l'app (README local, notes de conception).

## Ce qui ne va pas ici

- Le code natif de la plateforme (kernel, HAL, drivers) : voir
  `firmware/`.
- Les modules matériels CX-Bus : voir `modules/`.
- Les assets d'identité visuelle officielle de Companion Platform (logo,
  mascotte globale) : voir `assets/` et `LICENSING.md`.

## Licence

Les apps **officielles** maintenues dans ce dépôt (comme `tamagotchi`)
sont publiées sous **Apache-2.0**, au même titre que le reste du cœur de
la plateforme. Les apps **tierces**, si elles sont un jour hébergées dans
un dossier communautaire distinct, peuvent choisir n'importe quelle
licence libre — voir [`LICENSING.md`](../LICENSING.md).

## Phase de roadmap

Ce dossier se remplit progressivement à mesure que le Companion SDK
stabilise ses API (runtime Lua, cycle de vie d'app, accès aux
périphériques via HAL). Le Tamagotchi sert de première implémentation de
référence ; les apps suivantes arrivent au fur et à mesure des phases
ultérieures de la roadmap. Consultez `docs/roadmap.md` pour le calendrier
détaillé.

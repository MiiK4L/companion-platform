<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# `tools/` — Outillage du projet

Ce dossier rassemble les **scripts et utilitaires** qui soutiennent le
développement, la fabrication et la maintenance de Companion Platform,
sans faire partie du firmware ni des apps eux-mêmes.

## Ce qui va ici

- Scripts de **build et de flash** (wrapper autour d'ESP-IDF, automatiser
  la compilation multi-cibles, flasher un appareil connecté).
- **Générateurs** de code ou de fichiers (par exemple génération de
  squelettes de module, de manifest d'app, de headers depuis une
  spécification CX-Bus).
- Outillage de **provisionnement du support d'identification CX-Bus**
  (EEPROM I²C si retenue) — **à définir en Phase 1** : écriture du manifest
  d'identification d'un module CX-Bus (identifiant, version, capacités) sur
  le support d'identification du module lors de sa fabrication ou de son
  développement.
- Utilitaires divers de développement (linters custom, scripts de
  vérification de cohérence entre dossiers, aide à la release).

## Ce qui ne va pas ici

- Le code embarqué exécuté sur l'appareil (kernel, HAL, drivers, apps,
  modules) : ces outils *manipulent* ce code, ils n'en font pas partie.
- Les workflows CI (GitHub Actions) : voir `.github/workflows/` — un
  script réutilisable par la CI peut néanmoins vivre ici et être appelé
  depuis un workflow.

## Licence

Ce dossier est publié sous **Apache-2.0**, comme le reste du cœur logiciel
de la plateforme — voir [`LICENSING.md`](../LICENSING.md).

## Phase de roadmap

Les premiers outils attendus sont les scripts de build/flash ESP-IDF et
l'outillage de provisionnement du support d'identification CX-Bus (EEPROM I²C
si retenue — à définir en Phase 1), dès que le standard CX-Bus
(`standards/cx-bus/`) fige le mécanisme normatif et le format du manifest
d'identification. Les
générateurs plus avancés (scaffolding d'app/module) arrivent en phase
ultérieure, une fois les guides `docs/guides/app-development.md` et
`docs/guides/module-development.md` stabilisés. Consultez
`docs/roadmap.md` pour le détail.

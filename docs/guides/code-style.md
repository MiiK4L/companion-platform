# Guide — Style de code

Ce guide définit les conventions de style attendues pour le code de
Companion Platform. Deux langages cohabitent dans le monorepo : **C** pour
le firmware/HAL/drivers, et **Lua** pour les apps exécutées au-dessus du
Companion SDK (voir ADR-0002 pour le runtime hybride).

## 1. C — Firmware, HAL, drivers

### 1.1 Principes généraux

- À compléter — Phase 1 : version de C ciblée, standard (C11 ou
  supérieur), en-têtes autorisés.
- À compléter — Phase 1 : rappel de l'inversion des dépendances
  matérielles — le code au-dessus de la HAL ne doit jamais inclure
  directement des en-têtes ESP-IDF/FreeRTOS.

### 1.2 Conventions de nommage

- À compléter — Phase 1 : convention de nommage des fonctions publiques
  de la HAL et du Companion SDK (préfixe par domaine, ex. `hal_gpio_*`,
  `sdk_display_*`).
- À compléter — Phase 1 : convention pour les types, constantes, macros,
  fichiers d'en-tête (guards, `#pragma once` ou non).
- À compléter — Phase 2 : convention pour les erreurs et codes de retour.

### 1.3 Formatage automatique

- À compléter — Phase 1 : `clang-format` prévu, avec un fichier
  `.clang-format` versionné à la racine (style de base, largeur de
  colonne, indentation).
- À compléter — Phase 2 : `clang-tidy` prévu pour l'analyse statique,
  règles activées/désactivées et justification.
- À compléter — Phase 2 : intégration de ces outils dans la CI et en
  pre-commit local.

## 2. Lua — Apps

### 2.1 Principes généraux

- À compléter — Phase 2 : version de Lua ciblée par le runtime, sandbox
  et limitations d'accès (une app ne parle qu'au Companion SDK, jamais à
  la HAL directement).

### 2.2 Conventions de nommage et structure

- À compléter — Phase 2 : convention de nommage des fichiers et modules
  Lua d'une app, structure attendue du manifest (voir
  `docs/guides/app-development.md`).

### 2.3 Formatage et lint

- À compléter — Phase 2 : outil de formatage/lint Lua retenu (ex.
  StyLua, luacheck), configuration versionnée.

## 3. Documentation en ligne (commentaires)

- À compléter — Phase 1 : convention de commentaires pour les API
  publiques de la HAL et du Companion SDK (style Doxygen ou équivalent).

## Voir aussi

- [`docs/guides/contributing-code.md`](contributing-code.md)
- [`docs/guides/app-development.md`](app-development.md)
- [`docs/guides/module-development.md`](module-development.md)

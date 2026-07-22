# Guide — Contribuer du code

Ce guide détaille, côté technique, comment préparer, tester et soumettre
une contribution de code à Companion Platform. Pour le workflow Git
général, les Conventional Commits et le style de PR, voir d'abord
[`CONTRIBUTING.md`](../../CONTRIBUTING.md) à la racine du dépôt — ce guide
le complète avec des détails techniques.

## 1. Préparer son environnement

- À compléter — Phase 1 : prérequis système, installation d'ESP-IDF,
  outils KiCad, dépendances Python/Node éventuelles.
- À compléter — Phase 1 : lien vers `docs/guides/build-and-flash.md` pour
  la première compilation.

## 2. Choisir où contribuer

- À compléter — Phase 1 : arbre de décision simplifié (firmware ? app ?
  module ? doc ?) renvoyant vers le README du dossier concerné.

## 3. Écrire le code

- À compléter — Phase 1 : rappel des règles de dépendance (apps → SDK →
  HAL → drivers → silicium), interdiction des dépendances directes
  ESP-IDF/FreeRTOS hors HAL/drivers.
- À compléter — Phase 2 : lien vers `docs/guides/code-style.md` pour les
  conventions de style C et du langage de script (Lua candidat en V1).

## 4. Tester localement

- À compléter — Phase 2 : comment lancer les tests unitaires host
  (`tests/unit/`) contre la HAL mockée avant de soumettre une PR.
- À compléter — Phase 3 : comment lancer les tests d'intégration si le
  changement le nécessite.

## 5. Documenter le changement

- À compléter — Phase 1 : rappel de la règle « aucune fonctionnalité
  importante sans documentation » (README local, guide, ADR selon le
  cas).

## 6. Soumettre la Pull Request

- À compléter — Phase 1 : renvoi vers le modèle de PR et la checklist du
  dépôt (`.github/PULL_REQUEST_TEMPLATE.md`).

## 7. Revue et intégration

- À compléter — Phase 2 : attentes en matière de revue, critères de
  fusion, gestion des retours.

## Voir aussi

- [`CONTRIBUTING.md`](../../CONTRIBUTING.md)
- [`docs/guides/code-style.md`](code-style.md)
- [`docs/guides/build-and-flash.md`](build-and-flash.md)
- `docs/adr/` pour le processus de décision d'architecture

# Guide — Développer une app

Ce guide explique comment écrire une **app** Companion Platform en Lua,
au-dessus du **Companion SDK**. Le Tamagotchi est la première app de
référence de la plateforme ; ce guide vise à généraliser son
fonctionnement pour toute app future (réseau, homelab, électronique,
capteurs, jeux...).

## 1. Principes généraux

- À compléter — Phase 2 : rappel que les apps sont scriptées en **Lua**
  et ne parlent qu'au **Companion SDK**, jamais directement à la HAL ni à
  ESP-IDF/FreeRTOS (voir ADR-0001/0002/0007).
- À compléter — Phase 2 : ce qu'une app peut et ne peut pas faire
  (permissions, isolation entre apps).

## 2. Anatomie d'une app

- À compléter — Phase 2 : structure de fichiers attendue dans
  `apps/<nom-app>/` (point d'entrée, ressources, assets locaux).
- À compléter — Phase 2 : conventions de nommage héritées de
  `docs/guides/code-style.md`.

## 3. Le manifest d'app

- À compléter — Phase 2 : format du manifest (nom, version SemVer
  `app-x.y.z`, auteur, licence, permissions/capacités requises,
  dépendances au Companion SDK par version).
- À compléter — Phase 2 : comment le manifest est validé au chargement
  de l'app par le runtime.

## 4. Cycle de vie d'une app

- À compléter — Phase 2 : étapes du cycle de vie (installation,
  démarrage, premier plan/arrière-plan si applicable, mise en veille,
  arrêt, désinstallation) et les points d'entrée Lua correspondants.
- À compléter — Phase 3 : gestion des événements système (appui bouton,
  notification, connexion/déconnexion d'un module CX-Bus pertinent pour
  l'app).

## 5. Accéder au matériel via le Companion SDK

- À compléter — Phase 2 : panorama des API du Companion SDK disponibles
  pour une app (écran/LVGL, stockage, capteurs exposés via CX-Bus,
  réseau si applicable).
- À compléter — Phase 3 : comment une app découvre et utilise un module
  CX-Bus connecté, en lien avec `docs/guides/module-development.md`.

## 6. Tester une app

- À compléter — Phase 2 : comment exécuter une app en local (émulateur
  ou HAL mockée) avant déploiement sur matériel réel.

## 7. Publier une app

- À compléter — Phase 3 : différence entre app officielle (ce dépôt,
  Apache-2.0) et app tierce (licence au choix de l'auteur) — voir
  [`LICENSING.md`](../../LICENSING.md).
- À compléter — Phase 3 : processus de soumission d'une app tierce, si un
  canal de distribution communautaire est mis en place.

## Voir aussi

- [`apps/README.md`](../../apps/README.md)
- [`docs/guides/code-style.md`](code-style.md)
- [`docs/guides/module-development.md`](module-development.md)
- `docs/adr/` (ADR-0001, ADR-0002, ADR-0007)

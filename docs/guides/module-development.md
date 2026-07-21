# Guide — Développer un module CX-Bus

Ce guide explique comment concevoir un module d'extension conforme au
standard **CX-Bus (Companion eXpansion Bus)** (voir ADR-0005/0006 et
`standards/cx-bus/`), qu'il s'agisse d'un module officiel ou tiers.

## 1. Principes généraux

- À compléter — Phase 3 : rappel du rôle du CX-Bus comme standard unique
  d'extension matérielle de la plateforme, et pourquoi la mainboard
  n'embarque volontairement aucun capteur applicatif (voir
  `hardware/README.md`).

## 2. Mécanique et connectique

- À compléter — Phase 3 : dimensions, empreinte de connecteur, contraintes
  d'empilement/accostage sur la mainboard et le boîtier (`case/`),
  tolérances mécaniques attendues.

## 3. Identification via EEPROM — le CX-Bus Manifest

- À compléter — Phase 3 : format du **manifest CX-Bus** stocké en EEPROM
  sur chaque module (identifiant unique de type de module, version
  matérielle, capacités déclarées, table de fonctions exposées).
- À compléter — Phase 3 : comment ce manifest est écrit lors de la
  fabrication ou du développement, via l'outil de provisioning EEPROM
  (voir `tools/README.md`).
- À compléter — Phase 3 : comment le firmware de la mainboard lit et
  valide ce manifest au moment de l'énumération des modules connectés.

## 4. Protocole de communication

- À compléter — Phase 3 : couche physique/logique du CX-Bus (bus utilisé,
  adressage, format des trames), renvoi vers `standards/cx-bus/` pour la
  spécification complète.

## 5. Firmware et driver du module

- À compléter — Phase 3 : structure attendue du firmware/driver associé
  au module dans `modules/<nom-module>/firmware/`, respect de la HAL et
  de l'inversion des dépendances matérielles.

## 6. Apps exposées par un module

- À compléter — Phase 3 : comment un module déclare les capacités qu'une
  app peut consommer via le Companion SDK (par exemple : un module
  capteur expose une mesure lisible par n'importe quelle app
  compatible), en lien avec `docs/guides/app-development.md`.

## 7. Tester un module

- À compléter — Phase 3 : stratégie de test (HAL mockée pour la logique,
  banc de test matériel pour la validation électrique/mécanique) — voir
  `tests/README.md`.

## 8. Publier un module

- À compléter — Phase 3 : module officiel (ce dépôt, matériel
  CERN-OHL-S-2.0 + firmware Apache-2.0) vs module tiers (licence au choix
  de l'auteur, à déclarer explicitement) — voir
  [`LICENSING.md`](https://github.com/MiiK4L/companion-platform/blob/main/LICENSING.md).

## Voir aussi

- [`modules/README.md`](https://github.com/MiiK4L/companion-platform/blob/main/modules/README.md)
- [`hardware/README.md`](https://github.com/MiiK4L/companion-platform/blob/main/hardware/README.md)
- [`tools/README.md`](https://github.com/MiiK4L/companion-platform/blob/main/tools/README.md)
- `standards/cx-bus/`
- `docs/adr/` (ADR-0005, ADR-0006)

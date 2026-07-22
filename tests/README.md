<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# `tests/` — Tests

Ce dossier rassemble les tests automatisés de Companion Platform : tests
unitaires exécutés **sur machine hôte (PC)** et tests d'intégration.

## Organisation

```
tests/
  unit/           # tests unitaires host, HAL mockée
  integration/    # tests d'intégration (sur cible ou banc de test)
```

(La structure précise se stabilisera avec le choix définitif du framework
de test ; ce README sera mis à jour en conséquence.)

## Pourquoi les ports permettent de tester sur PC

Le firmware suit un modèle **ports / adaptateurs** (voir ADR-0007 et
`docs/architecture/dependency-inversion.md`). Parce que le code métier
(kernel, services, Companion SDK) ne dépend que des **ports abstraits** —
et jamais directement d'ESP-IDF, de FreeRTOS ou d'un driver concret — il
devient possible de fournir des **adaptateurs host (mocks)** qui
implémentent ces ports et tournent sur machine hôte, sans matériel
ESP32-S3 réel. Le **point de composition** injecte, au démarrage des
tests, les adaptateurs host à la place des adaptateurs cible.

Cela permet :

- D'exécuter la majorité des tests unitaires en quelques secondes, dans
  la CI, sans matériel physique ni émulateur.
- De tester des scénarios difficiles à reproduire sur cible réelle
  (conditions d'erreur, timeouts, séquences d'événements précises).
- De garder une boucle de rétroaction rapide pour les contributeur·rice·s
  qui n'ont pas forcément un appareil Companion Platform sous la main.

Les **tests d'intégration**, eux, s'exécutent contre du matériel réel (ou
un banc de test proche du réel) pour valider ce que la HAL mockée ne peut
pas garantir : timing réel, comportement électrique, interaction avec de
vrais modules CX-Bus.

## Ce qui va ici

- Tests unitaires du kernel, des services, du Companion SDK, contre la
  HAL mockée.
- Tests d'intégration validant le comportement sur cible réelle ou banc
  de test.
- Fixtures et mocks partagés entre suites de tests.

## Ce qui ne va pas ici

- Les **adaptateurs cible de production** (ESP32-S3) : ils vivent dans
  `firmware/drivers/`, pas ici. Ce dossier n'héberge que les adaptateurs
  host (mocks) et les suites de tests.
- Les tests spécifiques à une app ou un module, qui peuvent vivre
  localement dans le dossier de l'app/module concerné si plus pertinent.

## Licence

Ce dossier est publié sous **Apache-2.0** — voir
[`LICENSING.md`](../LICENSING.md).

## Phase de roadmap

Les tests unitaires host démarrent dès que la HAL expose une interface
stable (première phase). Les tests d'intégration sur matériel réel
arrivent une fois `hardware/mainboard-v1/` disponible en quantité
suffisante pour un banc de test. Consultez `docs/roadmap.md` pour le
calendrier détaillé.

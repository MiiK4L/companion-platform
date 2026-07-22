<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# `kernel/` — Noyau & point de composition

> Statut : **Phase 0 — Fondations**. Les mécanismes détaillés sont marqués « ⏳ À définir ».

Le **kernel** fournit les mécanismes centraux de la plateforme et héberge le **point de
composition** (composition root). Il orchestre le système sans jamais exposer les détails du
RTOS aux couches supérieures. Voir le modèle ports/adaptateurs
([dependency-inversion](../../docs/architecture/dependency-inversion.md)).

Références de décision : ADR-0007, ADR-0008, ADR-0009.

## Responsabilités

- **Point de composition** : au **démarrage**, le kernel choisit les **adaptateurs** concrets
  (cible ESP32-S3, ou mocks host en test) et les injecte derrière les **ports** que les services
  utilisent. C'est le seul endroit qui connaît à la fois interfaces et implémentations.
- **Ordonnancement** : modèle de tâches présenté aux services, au-dessus d'un **port
  `scheduler`**. FreeRTOS n'est **jamais exposé** aux services ni aux apps ; il n'est atteint
  qu'au travers d'un adaptateur.
- **Gestion d'énergie / deep sleep** : politiques de basse consommation via le port `power`,
  coordination des réveils (bouton, RTC, mouvement — ADR-0008).
- **Bus d'événements** : diffusion d'événements système entre services.
- **Gestion d'état & persistance** : sauvegarde/restauration de l'état via le port `storage`,
  support du calcul du delta temps au réveil (ADR-0009).

## Règles de dépendance

- Le kernel dépend des **ports abstraits** (`hal/`) — pas d'ESP-IDF ni de FreeRTOS en direct
  (ADR-0007), sauf au sein du point de composition qui, par nature, référence les adaptateurs
  concrets à assembler.
- Le kernel **NE DOIT PAS** dépendre des services, du SDK ou des apps.
- Les services s'appuient sur les mécanismes du kernel ; les apps n'y accèdent **jamais**
  directement — seulement via le Companion SDK.

> ⏳ **À définir — Phases ultérieures** : modèle de tâches exposé, API du bus d'événements,
> format de persistance, politiques d'énergie, mécanique exacte du point de composition.

## Licence

Firmware sous **Apache-2.0**. Documentation sous **CC-BY-4.0**.

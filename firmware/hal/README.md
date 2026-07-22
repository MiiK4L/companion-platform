<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# `hal/` — Ports de plateforme (interfaces abstraites)

> Statut : **Phase 0 — Fondations**. Les interfaces détaillées sont marquées « ⏳ À définir ».

Ce dossier contient les **ports de plateforme** : les **interfaces abstraites** qui définissent
**quoi** le firmware attend du matériel et du système, indépendamment de **comment** c'est
réalisé. C'est le cœur du modèle ports/adaptateurs (voir
[dependency-inversion](../../docs/architecture/dependency-inversion.md)).

> Le nom historique « HAL » (*Hardware Abstraction Layer*) est conservé pour le dossier, mais
> son contenu est strictement **des interfaces** — pas des implémentations.

Références de décision : ADR-0001, ADR-0007.

## Contenu

Interfaces abstraites (ports) pour :

- **display** : surface de rendu (support du moteur graphique, ADR-0003).
- **input** : boutons et événements d'entrée.
- **bus** : I²C, SPI, UART (transport, arbitrage des bus partagés).
- **power** : modes basse consommation, deep sleep, sources de réveil, power-gating.
- **clock** : horloge monotone, temporisations, alarmes.
- **storage** : mémoire non volatile abstraite.
- **scheduler** : abstraction des tâches (au-dessus du RTOS, jamais exposée telle quelle).

## Règles de dépendance

- Les ports sont **portables** : ils **NE connaissent PAS** ESP-IDF/FreeRTOS et **ne dépendent
  d'aucune implémentation** ni d'aucun driver (règle 5 du modèle).
- Ce sont les **adaptateurs** (dossier [`drivers/`](../drivers/README.md), cible ; et les mocks
  host sous [`tests/`](../../tests/README.md)) qui **implémentent** ces ports.
- Les services, le kernel et le SDK dépendent de ces **interfaces**, jamais d'une implémentation
  concrète. Le choix de l'implémentation est fait par le **point de composition** au démarrage.

## Testabilité

Parce que les ports sont abstraits et portables, on peut fournir des **adaptateurs host (mocks)**
qui les implémentent sur PC : services et SDK se compilent et se testent **sans matériel ni
ESP-IDF**. C'est un objectif structurant (voir [`tests/`](../../tests/README.md)).

> ⏳ **À définir — Phases ultérieures** : signatures exactes des ports, conventions d'erreur,
> modèle de configuration, injection par le point de composition.

## Licence

Firmware sous **Apache-2.0**. Documentation sous **CC-BY-4.0**.

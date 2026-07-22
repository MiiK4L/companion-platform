<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Companion Platform

**Companion Platform** est une plateforme électronique de poche **open source** et **modulaire**, conçue pour durer des années plutôt que pour suivre une mode passagère. Construite autour du microcontrôleur **Seeed XIAO ESP32-S3** — cœur de calcul **remplaçable** (non soudé définitivement ; socketage **pressenti**, à valider) — elle est pensée comme un petit ordinateur de poche capable d'accueillir de nombreuses applications au fil du temps : un Tamagotchi pour commencer, puis des outils réseau, un homelab miniature, de l'électronique, des capteurs, des jeux, et bien d'autres usages imaginés par la communauté.

Ce projet fait le pari inverse de l'obsolescence programmée : chaque choix technique — matériel, firmware, logiciel, documentation — est évalué d'abord sur sa **robustesse**, sa **maintenabilité** et sa **capacité à évoluer**, jamais sur la rapidité de mise en œuvre.

## Quatre décisions structurantes

L'architecture de Companion Platform repose sur quelques décisions structurantes, documentées sous forme d'[ADR](adr/README.md) :

1. **Un runtime d'applications hybride et extensible** ([ADR-0002](adr/README.md)) — un cœur natif pour les applications critiques ou performantes, et des applications scriptées, plus légères à distribuer, le tout derrière une **abstraction de runtime**. Le langage de script **Lua** est le **candidat** pressenti pour la V1 (à valider par prototype). L'isolation des apps scriptées (permissions, limites de ressources) **reste à concevoir et à prouver** — Lua n'est pas une sandbox par défaut.
2. **Un écran couleur entièrement abstrait** ([ADR-0003](adr/README.md)) — les applications dessinent leur interface via une abstraction graphique, sans jamais dépendre du contrôleur d'écran réellement présent sur la carte. Un **écran TFT IPS couleur** et le moteur graphique **LVGL** sont les choix **candidats** (à valider en Phase 1/2).
3. **Un standard d'extension matériel unifié, le CX-Bus** *(Companion eXpansion Bus)* ([ADR-0005](adr/README.md) / [ADR-0006](adr/README.md)) — chaque module d'extension (capteur, connectivité, affichage...) s'identifie et se déclare de façon standardisée. Le mécanisme d'identification est **candidat** : une **EEPROM I²C** embarquée est l'option pressentie, à valider.
4. **Un framework ESP-IDF encapsulé derrière des ports abstraits et un Companion SDK** ([ADR-0001](adr/README.md) / [ADR-0007](adr/README.md)) — aucune application ne dépend directement d'ESP-IDF ni de FreeRTOS ; ceux-ci sont confinés aux adaptateurs cible.

## Le principe directeur : un modèle ports / adaptateurs

Companion Platform applique un modèle **ports / adaptateurs** (inversion des dépendances) : les **abstractions** (les *ports*) ne dépendent d'aucune implémentation concrète ; ce sont les *adaptateurs* concrets qui dépendent des ports qu'ils implémentent.

```
apps → Companion SDK → services → ports abstraits ◄─ adaptateurs (ESP32-S3/ESP-IDF · host/mock)
                                                          └─ drivers · ESP-IDF · FreeRTOS · silicium
```

Une application ne connaît que le **Companion SDK**, qui est un **contrat / une façade** fournie par les services. Les services dépendent de **ports** (interfaces portables, sans ESP-IDF), implémentés par des **adaptateurs** (cible ESP32-S3, ou mocks pour les tests). Ce modèle **vise à préserver la compatibilité applicative dans les limites du contrat garanti par le SDK** lorsqu'une pièce bas niveau change (écran, capteur, voire microcontrôleur) — c'est un **objectif de conception, pas une garantie absolue** : un tel changement demande d'écrire ou d'adapter un adaptateur, et peut modifier des performances ou des capacités disponibles.

Pour aller plus loin, consultez la page [Inversion des dépendances](architecture/dependency-inversion.md).

## Pour aller plus loin

- [Vue d'ensemble de l'architecture](architecture/overview.md) — comment le matériel, le firmware et les applications s'articulent.
- [Décisions d'architecture (ADR)](adr/README.md) — l'historique complet des choix structurants du projet.
- [Roadmap](roadmap.md) — les phases de développement prévues.
- [Guides](guides/contributing-code.md) — pour contribuer au code, développer une application ou un module, ou flasher votre carte.

## Licences

Companion Platform est un projet de référence, publié sous plusieurs licences selon la nature du contenu : **Apache-2.0** pour le firmware, l'OS, le SDK et les outils ; **CERN-OHL-S-2.0** pour le matériel ; **CC-BY-4.0** pour la documentation. L'identité visuelle du projet reste réservée. Voir [ADR-0010](adr/README.md) pour le détail des licences retenues.

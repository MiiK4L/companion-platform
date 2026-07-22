<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Architecture logicielle / OS

Le firmware est conçu comme un **petit système d'exploitation** : le Tamagotchi
n'est jamais dans le noyau, c'est une application. Décisions liées :
[ADR-0001](../adr/0001-framework-firmware-esp-idf.md) (ESP-IDF),
[ADR-0002](../adr/0002-modele-execution-applications.md) (runtime),
[ADR-0007](../adr/0007-hal-et-companion-sdk.md) (HAL/SDK).

## Couches

Le firmware suit un modèle **ports / adaptateurs** (et non une pile stricte) —
voir [inversion des dépendances](dependency-inversion.md) :

```
Applications (scriptées, Lua candidat, chargées par le runtime · natives compilées dans l'image)
        │  ne dépendent que du
Companion SDK — façade/contrat fournie par les services
        │
Services — Module Manager · App Manager · UI (moteur graphique, LVGL candidat) · Power · Storage · Connectivity · Companion
        │  dépendent des
Ports abstraits (hal/) — display · input · clock · storage · bus · power · scheduler
        ▲  implémentés par
        │
Adaptateurs — cible ESP32-S3 / ESP-IDF   ·   host/mock (tests)
        │  s'appuient sur
Drivers · ESP-IDF · FreeRTOS · silicium
```

Le **point de composition** (`kernel/`) assemble ports ↔ adaptateurs au
démarrage. Le bootloader ESP-IDF est propre à la cible ; **Secure Boot, Flash
Encryption et OTA signé sont des cibles de production, non requis en
prototypage** (voir [`SECURITY.md`](../../SECURITY.md)).

Détail des responsabilités dans les READMEs de couches :
[`firmware/`](../../firmware/README.md), [`hal`](../../firmware/hal/README.md),
[`drivers`](../../firmware/drivers/README.md),
[`kernel`](../../firmware/kernel/README.md),
[`services`](../../firmware/services/README.md),
[`companion-sdk`](../../firmware/companion-sdk/README.md).

## Services clés

- **Module Manager** — détecte l'insertion/retrait d'un module CX-Bus, lit et
  valide le **CX-Bus Manifest** (support d'identification, EEPROM I²C candidate),
  charge les drivers et **publie les capacités**. Il ne lance rien : l'**App
  Manager** décide ensuite, selon la **politique de confiance**, quelles apps
  compatibles proposer — **aucune app n'est lancée sur la seule foi du Manifest**.
- **App Manager** — cycle de vie des applications (installation, lancement,
  suspension, sauvegarde), exécution via un **runtime de script** (**Lua**
  candidat en V1), derrière une **abstraction de runtime** ouverte à WASM plus
  tard.
- **UI** — moteur graphique (**LVGL** candidat) : widgets, sprites, animations, transitions ;
  aucune app ne touche le pixel ni le contrôleur d'écran ([ADR-0003](../adr/0003-technologie-et-abstraction-ecran.md)).
- **Power** — deep sleep, extinction auto de l'écran, réveils (bouton, alarme
  RTC, mouvement de l'accéléromètre), power-gating des modules ([ADR-0008](../adr/0008-architecture-alimentation.md)).
- **Storage** — état/réglages en NVS, données/sauvegardes en LittleFS.
- **Connectivity** — Wi-Fi / Bluetooth, exposés aux apps via le SDK.
- **Companion** — service **transverse** : traduit un état système en
  expression du personnage. C'est ce qui unifie l'expérience (« CPU 80 % » →
  personnage fatigué). N'importe quelle app peut piloter le personnage.

## Modèle d'application

Une application est **conçue pour être installable indépendamment du firmware**
([ADR-0002](../adr/0002-modele-execution-applications.md)) — objectif visé pour
les apps scriptées, dont la **faisabilité d'installation dynamique reste à
valider** (P8) ; les apps natives, elles, sont compilées dans l'image. Une app
est décrite par un manifeste (identité, capacités matérielles requises,
écrans/capacités exposés). L'App Manager peut la proposer/activer si ses
exigences sont satisfaites — par exemple lorsqu'un module CX-Bus fournissant la
capacité requise est présent — dans le cadre de la politique de confiance.

## Persistance & évolution hors tension

Le système privilégie un **deep sleep réel** : au réveil, il lit la **base de
temps** (RTC externe **candidat**, PCF8563 pressenti) et applique l'évolution
correspondant au **Δt** écoulé (le Tamagotchi « vieillit » sans tâche de fond
permanente). La base de temps fiable et l'évolution par Δt sont des principes
acquis ; le composant RTC reste à valider. Voir
[ADR-0009](../adr/0009-rtc-externe-et-persistance-etat.md).

## Sécurité (périmètre)

Secure Boot, Flash Encryption (fonctions ESP-IDF) et OTA signé sont des **cibles
de production**, non requis en prototypage, et **ne constituent pas à eux seuls
la sécurité des apps** : la confiance envers les modules physiques, les Manifests
et les paquets Lua relève d'un **modèle de menace distinct**. Détail et périmètre :
[`SECURITY.md`](../../SECURITY.md).

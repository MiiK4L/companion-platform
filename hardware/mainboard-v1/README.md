# Carte mère V1 — CX-Bus Host

> Statut : **Phase 0 — Fondations**. Ce document fige le rôle et le périmètre de la carte
> mère. Toute valeur électrique, tout schéma et toute cote sont marqués « ⏳ À définir » avec
> la phase concernée. Aucun brochage ni aucune valeur figurés ici n'est définitif.

La carte mère V1 est l'implémentation de référence du **CX-Bus Host** (voir
[`standards/cx-bus/`](../../standards/cx-bus/README.md)). Elle constitue le socle durable de
la Companion Platform.

Références de décision : ADR-0001, ADR-0003, ADR-0004, ADR-0008, ADR-0009.

## Rôle

Fournir la plateforme matérielle stable : calcul, alimentation, écran, entrées utilisateur,
temps réel, mouvement, et le connecteur d'extension CX-Bus. La carte mère **héberge la
plateforme**, pas la logique applicative : les fonctions métier (capteurs applicatifs, etc.)
appartiennent aux **modules CX-Bus**.

## Contenu autorisé

- **MCU socketé** : Seeed XIAO ESP32-S3, monté sur socket (remplaçable, ADR-0004).
- **Alimentation** : régulation des rails, gestion de la charge batterie via **USB-C**.
- **Gestion batterie** : jauge de charge (candidats MAX17048 / LC709203F).
- **Écran** : TFT IPS couleur, contrôleur interchangeable (ST7789 / GC9A01 / ILI9341),
  piloté via LVGL (ADR-0003).
- **Entrées** : boutons (via expander I²C, voir budget GPIO).
- **Retour haptique/sonore** : buzzer, moteur vibrant.
- **Temps réel** : RTC externe basse consommation (candidat PCF8563).
- **Mouvement** : accéléromètre (réveil sur mouvement).
- **Extension** : connecteur **CX-Bus** avec power-gating et détection de présence.

## Contenu INTERDIT

- **Aucun capteur applicatif soudé** sur la carte mère. Les capteurs et fonctions métier
  relèvent exclusivement des **modules CX-Bus**. Cette règle garantit la modularité et la
  durabilité : la carte mère reste un socle générique.

## Analyse du budget GPIO (XIAO ESP32-S3, 11 GPIO)

**Contrainte majeure.** Le XIAO ESP32-S3 n'expose que **11 GPIO**, alors que la carte doit
piloter écran, entrées, RTC, accéléromètre, jauge, EEPROM de modules et le connecteur CX-Bus.
La stratégie de bus suivante est une **contrainte de conception à valider en Phase 1/2** :

- **I²C pour tout ce qui est lent** : RTC, accéléromètre, jauge de batterie, expander de
  boutons, EEPROM d'identification des modules. Un **GPIO expander I²C** (candidats
  PCA9555 / TCA9535) étend le nombre de lignes disponibles sans consommer de GPIO du MCU.
- **SPI partagé écran + CX-Bus** : le même bus SPI sert l'écran et le connecteur module,
  avec chip-selects distincts et arbitrage géré par le firmware (SPEC CX-Bus §9).
- **GPIO réservés** : quelques lignes du MCU dédiées aux interruptions (`IRQ`), à l'enable
  d'alimentation module (`MOD_EN`, power-gating) et à la détection de présence.

> ⏳ **À définir — Phase 1/2** : affectation exacte des 11 GPIO, choix de l'expander,
> nombre de chip-selects, lignes d'interruption partagées vs dédiées, faisabilité du budget.

## Alimentation & autonomie

> ⏳ **À définir — Phase 2/3** : topologie de régulation, rails, budget de consommation,
> autonomie visée, modes basse consommation. (Placeholder Phase 0.)

## Gestion batterie & jauge

> ⏳ **À définir — Phase 1/2** : choix de la jauge (MAX17048 vs LC709203F), circuit de charge
> USB-C, protections, chimie et capacité de la batterie. (Placeholder Phase 0.)

## RTC & persistance

Le RTC externe basse consommation (candidat PCF8563) maintient le temps même MCU éteint. Il
sert de base au calcul du **delta temps au réveil**, permettant à l'état applicatif (ex.
Tamagotchi) d'évoluer hors tension (ADR-0009).

> ⏳ **À définir — Phase 1/2** : choix du RTC, pile/alimentation de secours, précision,
> stratégie de persistance d'état associée. (Placeholder Phase 0.)

## Réveils (bouton / RTC / mouvement)

La carte doit supporter le réveil depuis les modes basse consommation par : appui bouton,
alarme RTC, et détection de mouvement (accéléromètre).

> ⏳ **À définir — Phase 2/3** : sources de réveil câblées, latences, consommation en veille.
> (Placeholder Phase 0.)

## Licence

Le matériel de la carte mère V1 (`hardware/`) est publié sous **CERN-OHL-S-2.0**.
Le firmware associé relève d'**Apache-2.0** ; la documentation de **CC-BY-4.0**.

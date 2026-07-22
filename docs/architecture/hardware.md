<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Architecture matérielle

Vue d'ensemble du matériel. Le détail de conception (schémas, valeurs, PCB) est
produit aux phases 2–4 ; ce document fixe les principes. Décisions liées :
[ADR-0004](../adr/0004-coeur-de-calcul-socket.md) (cœur remplaçable),
[ADR-0005](../adr/0005-standard-cx-bus-et-identification.md) (CX-Bus),
[ADR-0008](../adr/0008-architecture-alimentation.md) (alimentation),
[ADR-0009](../adr/0009-rtc-externe-et-persistance-etat.md) (RTC).

## La carte mère « CX-Bus Host »

```
┌──────────────────────────────────────────────────────────┐
│  MODULE CX-Bus (cartouche) — hors carte, interchangeable   │
├──────────────────────────────────────────────────────────┤
│  Connecteur d'extension unique (alim + I²C + SPI + UART +  │
│  GPIO/IRQ + détection présence + EEPROM Manifest)          │
├───────────────┬────────────────┬───────────────────────────┤
│ Cœur calcul   │  Alimentation   │  IHM                      │
│ XIAO ESP32-S3 │  Charge USB-C   │  Écran TFT IPS*           │
│ (REMPLAÇABLE) │  Jauge batt.*   │  Boutons, buzzer, vibreur │
│               │  Power-gating   │                           │
├───────────────┴────────────────┴───────────────────────────┤
│  Capteurs de plateforme : base de temps (RTC externe*)     │
│  + accéléromètre                                            │
│  (* composants candidats, à valider en Phase 1/2)          │
│  (AUCUN capteur applicatif soudé)                           │
└──────────────────────────────────────────────────────────┘
```

Détail et statut : [`hardware/mainboard-v1/README.md`](../../hardware/mainboard-v1/README.md).

## Principes non négociables

1. **Aucun capteur applicatif soudé.** GPS, CO₂, caméra, LoRa, NFC, etc. sont
   **toujours** des modules CX-Bus. La carte mère ne contient que
   l'infrastructure générique.
2. **Cœur de calcul remplaçable** (XIAO non soudé définitivement ; socketage pressenti, à valider — [ADR-0004](../adr/0004-coeur-de-calcul-socket.md)).
3. **Un connecteur d'extension unique**, identique pour tous les modules
   ([standard CX-Bus](../../standards/cx-bus/README.md)).
4. **Écran abstrait** : contrôleur interchangeable, jamais vu par les apps
   ([ADR-0003](../adr/0003-technologie-et-abstraction-ecran.md)).

## Contrainte structurante : le budget GPIO

Le XIAO ESP32-S3 n'expose que **11 GPIO**. Or il faut alimenter : écran (SPI),
boutons, buzzer, vibreur, RTC (I²C), accéléromètre (I²C) **et** un connecteur
d'extension riche. On ne tient pas dans 11 broches en connectant tout
directement. Stratégie de bus **pressentie** (à figer en Phase 1/2) :

- **I²C** pour tout ce qui est lent : base de temps (RTC), accéléromètre, jauge
  batterie, mécanisme d'identification des modules, et boutons ; un **GPIO
  expander I²C** (PCA9555 / TCA9535, **candidats**) **pourra être nécessaire**
  pour étendre les lignes disponibles ;
- **SPI** partagé entre l'**écran** et le **CX-Bus** (avec lignes de sélection
  distinctes) ;
- quelques **GPIO** réservés aux interruptions (accéléromètre, module),
  à l'**enable** d'alimentation (power-gating) et à la **détection de présence**
  du module.

Ce budget est un **critère de validation** des schémas (Phase 2) et du PCB
(Phase 3–4).

## Alimentation & autonomie

- Recharge **USB-C** (via le XIAO en V1, à optimiser en Phase 1).
- **Jauge de batterie** dédiée (I²C) **candidate** pour un niveau fiable et un
  comportement « produit » (à valider en Phase 1/2).
- **Power-gating** du connecteur CX-Bus (un module ne consomme pas en veille).
- Réveils : **bouton**, **alarme RTC**, **mouvement** (accéléromètre).
- Un **budget énergétique par mode** (actif / écran éteint / deep sleep /
  module actif) sera établi en Phase 1 et suivi comme métrique de qualité.

Voir [ADR-0008](../adr/0008-architecture-alimentation.md).

## Le standard CX-Bus

Le connecteur, le brochage, l'alimentation, la détection, le protocole, le
format mécanique et l'identification (CX-Bus Manifest en EEPROM) sont définis
par le standard : [`standards/cx-bus/`](../../standards/cx-bus/README.md). La
spécification détaillée est produite en **Phase 1**.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Glossaire

Vocabulaire de référence du projet. À maintenir à jour : tout terme ambigu doit
être défini ici.

| Terme | Définition |
|-------|------------|
| **Companion Platform** | Le projet global : la plateforme matérielle + logicielle. |
| **CX-Bus** | *Companion eXpansion Bus* — le **standard d'extension** (connecteur, brochage, alimentation, détection, identification, protocole, format mécanique). Ne désigne **que** le standard, pas le projet. Voir [ADR-0006](../adr/0006-nommage-cx-bus.md). |
| **CX-Bus Host** | La **carte mère** qui reçoit les modules. |
| **CX-Bus Module** | La **cartouche** d'extension conforme au standard. |
| **CX-Bus Manifest** | Le **descripteur d'identification** d'un module (identifiants, capacités, **recommandations d'apps compatibles**…). Le support de stockage est un mécanisme **candidat** : une **EEPROM I²C** est pressentie, inspirée de la spec EEPROM des HAT Raspberry Pi. |
| **CX-Bus SDK** | L'**outillage pour créer des modules matériels** conformes (gabarits, provisionnement du support d'identification, validation). ⚠️ À ne pas confondre avec le Companion SDK. |
| **Companion SDK** | L'**API applicative stable** vue par les applications (natives et scriptées) : affichage/widgets, entrées, stockage, réseau, tâches, accès modules par capacités, personnage. C'est le contrat public du firmware. Voir [ADR-0007](../adr/0007-hal-et-companion-sdk.md). |
| **HAL / Ports** | *Hardware Abstraction Layer* — dans ce projet, désigne les **ports de plateforme** : interfaces **abstraites et portables**, **sans ESP-IDF**. Ce sont les **adaptateurs** (drivers) qui connaissent ESP-IDF/FreeRTOS et **implémentent** ces ports — adaptateurs **cible** (ESP32-S3) et **host/mock** (tests PC). Voir [ADR-0007](../adr/0007-hal-et-companion-sdk.md). |
| **Companion (service)** | Le service transverse qui traduit un état système en **expression du personnage** (fatigue, inquiétude…). |
| **App / Application** | Paquet installable indépendamment du firmware, exécuté par le runtime de script (**Lua** candidat en V1). Le Tamagotchi est une app. |
| **Module Manager** | Service qui détecte, identifie (via le Manifest) et intègre les modules CX-Bus. |
| **App Manager** | Service qui gère le cycle de vie des applications et le runtime de script. |
| **ADR** | *Architecture Decision Record* — décision d'architecture tracée et immuable. Voir [registre](../adr/README.md). |
| **Δt (delta de temps)** | Durée écoulée entre l'arrêt et le réveil, lue à l'horloge (RTC externe candidat) pour faire évoluer les apps hors tension. Voir [ADR-0009](../adr/0009-rtc-externe-et-persistance-etat.md). |
| **Power-gating** | Coupure logicielle de l'alimentation d'un sous-ensemble (ex. connecteur module) pour économiser l'énergie en veille. |
| **MADR** | *Markdown Any Decision Records* — format des ADR du projet. |
| **SPDX** | Identifiants de licence normalisés utilisés en en-tête de fichier (`SPDX-License-Identifier`). |

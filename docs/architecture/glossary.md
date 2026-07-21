# Glossaire

Vocabulaire de référence du projet. À maintenir à jour : tout terme ambigu doit
être défini ici.

| Terme | Définition |
|-------|------------|
| **Companion Platform** | Le projet global : la plateforme matérielle + logicielle. |
| **CX-Bus** | *Companion eXpansion Bus* — le **standard d'extension** (connecteur, brochage, alimentation, détection, identification, protocole, format mécanique). Ne désigne **que** le standard, pas le projet. Voir [ADR-0006](../adr/0006-nommage-cx-bus.md). |
| **CX-Bus Host** | La **carte mère** qui reçoit les modules. |
| **CX-Bus Module** | La **cartouche** d'extension conforme au standard. |
| **CX-Bus Manifest** | Le **descripteur d'identification** stocké dans l'EEPROM d'un module (nom, version, auteur, capacités, apps exposées…). Inspiré de la spec EEPROM des HAT Raspberry Pi. |
| **CX-Bus SDK** | L'**outillage pour créer des modules matériels** conformes (gabarits, provisionnement EEPROM, validation). ⚠️ À ne pas confondre avec le Companion SDK. |
| **Companion SDK** | L'**API applicative stable** vue par les applications (natives et Lua) : affichage/widgets, entrées, stockage, réseau, tâches, accès modules par capacités, personnage. C'est le contrat public du firmware. Voir [ADR-0007](../adr/0007-hal-et-companion-sdk.md). |
| **HAL** | *Hardware Abstraction Layer* — abstraction du matériel et du RTOS. Seule couche (avec les drivers) autorisée à connaître ESP-IDF/FreeRTOS. Implémentations **cible** (ESP32-S3) et **hôte/mock** (tests PC). |
| **Companion (service)** | Le service transverse qui traduit un état système en **expression du personnage** (fatigue, inquiétude…). |
| **App / Application** | Paquet installable indépendamment du firmware, exécuté par le runtime (Lua en V1). Le Tamagotchi est une app. |
| **Module Manager** | Service qui détecte, identifie (via le Manifest) et intègre les modules CX-Bus. |
| **App Manager** | Service qui gère le cycle de vie des applications et le runtime de script. |
| **ADR** | *Architecture Decision Record* — décision d'architecture tracée et immuable. Voir [registre](../adr/README.md). |
| **Δt (delta de temps)** | Durée écoulée entre l'arrêt et le réveil, lue au RTC pour faire évoluer les apps hors tension. Voir [ADR-0009](../adr/0009-rtc-externe-et-persistance-etat.md). |
| **Power-gating** | Coupure logicielle de l'alimentation d'un sous-ensemble (ex. connecteur module) pour économiser l'énergie en veille. |
| **MADR** | *Markdown Any Decision Records* — format des ADR du projet. |
| **SPDX** | Identifiants de licence normalisés utilisés en en-tête de fichier (`SPDX-License-Identifier`). |

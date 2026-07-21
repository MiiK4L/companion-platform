<!--
SPDX-License-Identifier: CC-BY-4.0
-->

# Companion Platform

> Une **plateforme électronique de poche**, open source, modulaire et
> documentée — conçue comme un produit, pensée pour durer des années.
> Le Tamagotchi n'est que la **première application**.

**Companion Platform** n'est pas un Tamagotchi. C'est une plateforme matérielle
et logicielle capable d'accueillir des dizaines d'applications — réseau,
homelab, électronique, capteurs, domotique, jeux — tout en gardant une interface
ludique où un **personnage sert d'interface**. On pourrait croire un produit du
commerce ; tout est conçu maison : matériel, firmware, boîtier, PCB, logiciel et
documentation.

> ⚠️ **Statut : Phase 0 (fondations).** Le dépôt contient aujourd'hui
> l'architecture, les décisions (ADR), la documentation et l'organisation du
> projet. **Aucun firmware n'est encore développé** : c'est volontaire. On
> construit d'abord des fondations solides. Voir la [feuille de route](docs/roadmap.md).

## Philosophie

> **qualité > robustesse > maintenabilité > évolutivité > documentation**,
> jamais la rapidité de développement.

On avance selon : **Vision → Architecture → Documentation → Validation →
Implémentation → Tests → Optimisation**, sans sauter d'étape. La documentation
est une **fonctionnalité à part entière**, et chaque décision importante est
tracée sous forme d'[ADR](docs/adr/README.md).

## Les quatre décisions fondatrices

| Domaine | Décision | ADR |
|---------|----------|-----|
| Exécution des apps | Hybride **natif + Lua** (abstraction ouverte à WASM) | [0002](docs/adr/0002-modele-execution-applications.md) |
| Affichage | **TFT IPS couleur** abstrait par un moteur graphique (LVGL) | [0003](docs/adr/0003-technologie-et-abstraction-ecran.md) |
| Modularité | Standard d'extension **CX-Bus** + identification EEPROM | [0005](docs/adr/0005-standard-cx-bus-et-identification.md) · [0006](docs/adr/0006-nommage-cx-bus.md) |
| Fondation firmware | **ESP-IDF** confiné sous **HAL + Companion SDK** | [0001](docs/adr/0001-framework-firmware-esp-idf.md) · [0007](docs/adr/0007-hal-et-companion-sdk.md) |

Le principe qui tient tout ensemble — **l'inversion des dépendances
matérielles** :

```
apps  →  Companion SDK  →  HAL  →  drivers  →  silicium (ESP-IDF / FreeRTOS)
```

Aucune couche ne saute une autre : **MCU, RTOS, écran et runtime sont
remplaçables sans réécrire les applications**.
[En savoir plus](docs/architecture/dependency-inversion.md).

## Matériel en bref

Une carte mère unique, le **CX-Bus Host** : cœur **XIAO ESP32-S3 socketé**
(remplaçable), alimentation/charge USB-C, écran, entrées, RTC, accéléromètre, et
**un connecteur d'extension unique**. **Aucun capteur applicatif soudé** : tout
capteur est un **module CX-Bus** auto-identifié qui fait apparaître ses
applications automatiquement.

## Organisation du dépôt

| Dossier | Contenu | Licence |
|---------|---------|---------|
| [`docs/`](docs/) | Documentation (docs-as-code, MkDocs) + [ADR](docs/adr/README.md) | CC-BY-4.0 |
| [`standards/cx-bus/`](standards/cx-bus/) | Le standard d'extension CX-Bus | CC-BY-4.0 / CERN-OHL-S |
| [`hardware/`](hardware/) | Schémas, PCB (KiCad), BOM, Gerbers | CERN-OHL-S-2.0 |
| [`firmware/`](firmware/) | Noyau, HAL, drivers, services, Companion SDK | Apache-2.0 |
| [`apps/`](apps/) | Une application par dossier (Lua) | Apache-2.0 (officielles) |
| [`modules/`](modules/) | Modules CX-Bus (doc + KiCad + firmware) | CERN-OHL-S / au choix (tiers) |
| [`case/`](case/) | Boîtier imprimable 3D (sources + STL) | CERN-OHL-S-2.0 |
| [`tools/`](tools/) | Scripts, provisionnement EEPROM, utilitaires | Apache-2.0 |
| [`tests/`](tests/) | Tests host (HAL mockée) + intégration | Apache-2.0 |
| [`assets/`](assets/) | Sprites, animations, rendus | CC-BY-4.0 (hors identité, réservée) |

## Par où commencer

- 🧭 [Vue d'ensemble de l'architecture](docs/architecture/overview.md)
- 🧩 [Le standard CX-Bus](standards/cx-bus/README.md)
- 🗂️ [Les décisions d'architecture (ADR)](docs/adr/README.md)
- 🛣️ [La feuille de route](docs/roadmap.md)
- 🤝 [Contribuer](CONTRIBUTING.md) · [Code de conduite](CODE_OF_CONDUCT.md) · [Sécurité](SECURITY.md)
- 📖 [Glossaire](docs/architecture/glossary.md)

## Licences

Projet multi-licences (voir [LICENSING.md](LICENSING.md)) : **Apache-2.0**
(firmware/OS/SDK/outils), **CERN-OHL-S-2.0** (matériel), **CC-BY-4.0**
(documentation), identité visuelle **réservée**. Les apps et modules tiers
peuvent choisir leur propre licence.

---

*Companion Platform — construit lentement, pour durer.*

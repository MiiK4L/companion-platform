# Inversion des dépendances matérielles

C'est **le** principe directeur du projet. Il conditionne la longévité, la
testabilité et l'évolutivité de toute la plateforme. Décision fondatrice :
[ADR-0007](../adr/0007-hal-et-companion-sdk.md).

## Le principe

```
┌─────────────────────────────────────────────────────────┐
│  APPLICATIONS & JEUX (natifs ou Lua)                       │
│  ne connaissent QUE le Companion SDK                       │
└───────────────────────────┬───────────────────────────────┘
                            │  dépend de
┌───────────────────────────▼───────────────────────────────┐
│  COMPANION SDK — API applicative stable (contrat public)   │
│  affichage/widgets, entrées, stockage, réseau, tâches,     │
│  accès modules par capacités, personnage (Companion)       │
└───────────────────────────┬───────────────────────────────┘
                            │  dépend de
┌───────────────────────────▼───────────────────────────────┐
│  HAL — abstraction matérielle (display, input, i2c, spi,   │
│  uart, power, clock/alarm, storage)                         │
└───────────────────────────┬───────────────────────────────┘
                            │  dépend de
┌───────────────────────────▼───────────────────────────────┐
│  DRIVERS — implémentations concrètes (ST7789, PCF8563,     │
│  jauge, GPIO expander, EEPROM CX-Bus, …)                    │
└───────────────────────────┬───────────────────────────────┘
                            │  dépend de
┌───────────────────────────▼───────────────────────────────┐
│  SILICIUM — ESP-IDF / FreeRTOS / ESP32-S3                  │
└─────────────────────────────────────────────────────────┘
```

**Règle d'or : aucune flèche ne saute une couche.** Une application n'inclut
jamais un en-tête ESP-IDF, ne parle jamais à un contrôleur d'écran, n'appelle
jamais FreeRTOS directement. Elle passe par le Companion SDK. Point.

## Deux frontières, deux rôles

- **La HAL** abstrait le **matériel et le RTOS**. Elle a (au moins) deux
  implémentations : une **cible** (ESP32-S3 via ESP-IDF) et une **hôte (mock)**
  pour exécuter l'OS et les tests sur un PC, sans matériel.
- **Le Companion SDK** abstrait la **plateforme pour les développeurs d'apps**.
  C'est une API de plus haut niveau, stable et versionnée, liable à la fois
  depuis du C natif et depuis Lua.

> ⚠️ Ne pas confondre **Companion SDK** (l'API vue par les apps) et
> **CX-Bus SDK** (l'outillage pour *créer des modules matériels* conformes au
> standard). Voir le [glossaire](glossary.md).

## Ce que le principe nous achète

| Changement bas niveau | Impact sur les apps |
|-----------------------|---------------------|
| Nouveau contrôleur d'écran (GC9A01 → ILI9341) | **Aucun** (nouveau driver) |
| Nouveau MCU / nouveau RTOS | **Aucun** (réimplémenter la HAL) |
| Nouveau runtime (Lua → +WASM) | **Aucun** (derrière l'abstraction de runtime, [ADR-0002](../adr/0002-modele-execution-applications.md)) |
| Nouvelle jauge de batterie | **Aucun** (nouveau driver) |

Et, tout aussi important : **on teste l'OS et les apps sur PC** via la HAL
mockée — CI rapide, sans matériel (voir [`tests/`](https://github.com/MiiK4L/companion-platform/blob/main/tests/README.md)).

## Comment on le fait respecter

- Découpage physique en composants (`firmware/hal`, `firmware/drivers`,
  `firmware/services`, `firmware/companion-sdk`, `apps/`).
- Revue de PR : toute inclusion d'un en-tête bas niveau hors HAL/drivers est
  refusée (checklist de PR dédiée).
- À terme, vérification automatisée en CI (règles d'inclusion).

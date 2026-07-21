# Firmware — Companion Platform

> Statut : **Phase 0 — Fondations**. Ce document fige l'organisation en couches et les règles
> de dépendance. Les API détaillées sont marquées « ⏳ À définir » dans les couches concernées.

Le firmware de la Companion Platform est organisé en **couches strictement empilées**,
gouvernées par le **principe d'inversion des dépendances matérielles**. C'est la clé de la
durabilité : MCU, RTOS, écran et runtime doivent tous être remplaçables sans réécrire les apps.

Références de décision : ADR-0001, ADR-0002, ADR-0007.

## Principe d'inversion des dépendances

Les dépendances vont **du haut vers le bas, sans jamais sauter une couche** :

```
        ┌─────────────────────────────────────────────┐
        │                   apps/                      │   Lua & natives
        │        (Tamagotchi, apps de modules…)        │
        └───────────────────────┬─────────────────────┘
                                 │  ne dépendent QUE du SDK
        ┌───────────────────────▼─────────────────────┐
        │              companion-sdk/                  │   API stable
        └───────────────────────┬─────────────────────┘
        ┌───────────────────────▼─────────────────────┐
        │                 services/                    │   Module/App Manager, UI, Power…
        └───────────────────────┬─────────────────────┘
        ┌───────────────────────▼─────────────────────┐
        │                  kernel/                     │   ordonnancement, énergie, événements, état
        └───────────────────────┬─────────────────────┘
        ┌───────────────────────▼─────────────────────┐
        │                   hal/                       │   abstraction matérielle
        └───────────────────────┬─────────────────────┘
        ┌───────────────────────▼─────────────────────┐
        │                 drivers/                     │   ST7789, PCF8563, jauge, expander…
        └───────────────────────┬─────────────────────┘
        ┌───────────────────────▼─────────────────────┐
        │            silicium / ESP-IDF / FreeRTOS     │   fondation confinée
        └─────────────────────────────────────────────┘
```

Règle fondamentale : **apps → Companion SDK → HAL → drivers → silicium.** Aucune couche ne
saute une autre, et les dépendances ne remontent jamais.

## ESP-IDF confiné aux couches basses

**ESP-IDF est la fondation** du firmware (ADR-0001), mais son usage est **confiné aux couches
basses** : seules la **HAL** et les **drivers** connaissent ESP-IDF/FreeRTOS. Les couches
supérieures (kernel, services, SDK, apps) n'incluent **jamais** un en-tête ESP-IDF ni un appel
FreeRTOS directement (ADR-0007). Cela permet de porter la plateforme vers un autre MCU/RTOS en
ne réécrivant que HAL + drivers.

## Organisation des couches

| Couche | Rôle | Connaît ESP-IDF ? |
|--------|------|-------------------|
| [`hal/`](./hal/README.md) | Abstraction matérielle (écran, entrées, bus, énergie, timers, stockage). | Oui |
| [`drivers/`](./drivers/README.md) | Pilotes concrets implémentant les interfaces HAL. | Oui |
| [`kernel/`](./kernel/README.md) | Ordonnancement, énergie, bus d'événements, état & persistance. | Non |
| [`services/`](./services/README.md) | Module Manager, App Manager, UI, Power, Storage, Connectivity, Companion. | Non |
| [`companion-sdk/`](./companion-sdk/README.md) | Surface d'API stable exposée aux apps. | Non |

## Licence

Le firmware est publié sous **Apache-2.0**. La documentation sous **CC-BY-4.0**.

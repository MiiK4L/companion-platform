# Firmware — Companion Platform

> Statut : **Phase 0 — Fondations**. Ce document fige le **modèle de dépendances** et le rôle
> de chaque composant. Les API détaillées sont marquées « ⏳ À définir » dans les couches concernées.

Le firmware suit un modèle **ports / adaptateurs** (voir
[docs/architecture/dependency-inversion.md](../docs/architecture/dependency-inversion.md)),
et non une pile de couches strictes. Les **ports** (interfaces abstraites) sont portables et ne
connaissent aucune implémentation ; les **adaptateurs** concrets dépendent des ports qu'ils
implémentent. Objectif : préserver la compatibilité applicative dans les limites du contrat du
Companion SDK lorsqu'une pièce bas niveau change — un **objectif**, pas une garantie absolue.

Références de décision : ADR-0001, ADR-0002, ADR-0007.

## Modèle de dépendances

```
   Applications (Lua chargées par le runtime · ou natives compilées)
                    │ ne dépendent que du
   Companion SDK    ▼   (façade/contrat fournie par les services)
                    │
   Services         ▼   dépendent des ports abstraits
                    │
   Ports (hal/)     ▼   interfaces PORTABLES, sans ESP-IDF
                    ▲   implémentés par
   Adaptateurs      │   cible ESP32-S3/ESP-IDF  +  host/mock (tests)
                    ▼   s'appuient sur
   Drivers / ESP-IDF / FreeRTOS / silicium
```

Règles essentielles (détail dans le document d'architecture) :

1. Les apps ne dépendent que du **Companion SDK**.
2. Le SDK est une **façade fournie par les services** ; il ne dépend pas des drivers.
3. Les **services** dépendent des **ports abstraits**, jamais d'ESP-IDF ni de drivers concrets.
4. Les **adaptateurs/drivers** dépendent des ports qu'ils **implémentent**.
5. Le paquet des **ports (`hal/`) reste portable**, sans ESP-IDF.
6. Le **point de composition** (au démarrage, dans `kernel/`) assemble ports ↔ adaptateurs.
7. Une dépendance **peut contourner un niveau** s'il n'y a pas de raison fonctionnelle de le
   traverser ; ce qui est interdit, c'est qu'une abstraction dépende d'un détail concret.

## ESP-IDF confiné aux adaptateurs

**ESP-IDF est la fondation** du firmware (ADR-0001), mais son usage est **confiné aux
adaptateurs cible et aux drivers**. Les **ports**, les **services**, le **SDK** et les **apps**
n'incluent **jamais** un en-tête ESP-IDF ni un appel FreeRTOS directement (ADR-0007). Porter la
plateforme vers un autre MCU/RTOS revient alors à écrire un nouvel **adaptateur cible**.

## Organisation

| Dossier | Rôle | Connaît ESP-IDF ? |
|---------|------|-------------------|
| [`hal/`](./hal/README.md) | **Ports de plateforme** : interfaces abstraites (display, input, clock, storage, bus, power, scheduler…). | **Non** |
| [`drivers/`](./drivers/README.md) | **Adaptateurs cible + drivers concrets** implémentant les ports (ST7789, PCF8563, jauge, expander…). | Oui |
| [`kernel/`](./kernel/README.md) | Mécanismes centraux (ordonnancement, énergie, événements, état) + **point de composition**. | Non |
| [`services/`](./services/README.md) | Module Manager, App Manager, UI, Power, Storage, Connectivity, Companion. | Non |
| [`companion-sdk/`](./companion-sdk/README.md) | Façade/contrat d'API stable exposée aux apps. | Non |

Les **adaptateurs host** (mocks des ports pour les tests PC) vivent sous [`tests/`](../tests/README.md).

## Licence

Firmware sous **Apache-2.0**. Documentation sous **CC-BY-4.0**.

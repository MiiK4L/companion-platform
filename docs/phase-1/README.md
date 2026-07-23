<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Phase 1 — Cadrage, cadre de validation & modèle de preuves

> **Statut : PLANIFICATION.** Cette section **cadre** la Phase 1. Elle ne
> démarre **aucun prototype**, ne crée **aucune ADR de décision** (0013+) et ne
> **fige aucun composant** : toutes les technologies restent **candidates
> (Proposé)** jusqu'à preuve reproductible.

La Phase 1 transforme les **principes acceptés** de la Phase 0 en une plateforme
**prototypée et mesurée**, et ne gèle que ce qui est démontré. Sa sortie maximale
est une **release candidate** de la spécification (`spec-v1.0.0-rc.1`), pas une
version finale.

## Documents

| Document | Rôle |
|----------|------|
| [Plan consolidé](plan.md) | Les 9 lots (objectifs, questions, hypothèses non figées, mesures, candidats, critères chiffrés, dépendances, risques, livrables, Proposé→Accepté, effort, matériel vs code) |
| [Ordre d'exécution & critères de sortie](execution.md) | Graphe, dépendances, parallélisation, une PR par lot, critères de sortie (`rc.1` max) |
| [Matrice des exigences](requirements-matrix.md) | Besoins plateforme **indépendants du MCU** + comparaison d'architectures de cœur + **déclencheurs d'arbitrage** du cœur |
| [Registre des risques](risk-register.md) | Registre initial et vivant (R1–R9) |
| [Sourcing & BOM](sourcing-and-bom.md) | Stratégie multi-source + ≥ 2 alternatives par composant critique + BOM prototype / **BOM V1 candidate** |
| [Cadre de validation](validation-framework.md) | Données brutes, traçabilité ADR ↔ preuves, critères Proposé→Accepté, exploratoire vs reproductible |
| [Modèle de protocole de test](templates/test-protocol-template.md) | Gabarit à renseigner **avant** l'essai (seuils chiffrés, incertitude, équipement) |
| [Modèle de rapport de mesure](templates/measurement-report-template.md) | Gabarit de restitution des résultats |
| [ADR prévisionnelles 0013+](adr-plan.md) | Liste **prévisionnelle** et conditions de création — **aucune créée ici** |

## Règles de la Phase 1

- **Principes acceptés** (Phase 0) : ne se rejouent pas, sauf preuve contraire d'un prototype.
- **Technologies candidates** : XIAO ESP32-S3 (candidat de référence), Lua, TFT/LVGL, EEPROM I²C (baseline), jauge, RTC externe, GPIO expander, connecteur — **toutes Proposé**.
- **Rien n'est figé sans preuve reproductible** atteignant des **seuils chiffrés**.
- **Une PR par lot**, relisible et fusionnable indépendamment.

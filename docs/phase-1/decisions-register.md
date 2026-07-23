<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Registre des questions de décision (`DEC-*`)

Registre central des **questions de décision** de la Phase 1, conformément au
[cadre de validation](validation-framework.md) (§2). Une question reçoit un
identifiant `DEC-<lot>-NNN` **à l'ouverture du lot** ; l'**ADR** correspondante
est créée **après** les mesures, lorsqu'un arbitrage est possible.

**Statuts d'un `DEC-*`** : **Ouvert** (question posée, non tranchée) · **Arbitré →
ADR-XXXX** (une ADR a été créée) · **Abandonné** (question retirée, avec trace).

> À ce stade (**Lot 1, documentaire**), les décisions ci-dessous sont **Ouvertes**
> et leurs éléments **Proposés** : aucune n'est arbitrée, **aucune ADR n'est
> créée** (les validations reproductibles nécessaires relèvent des lots suivants).

## Lot 1 — Exigences & architecture du cœur

| `DEC-*` | Question | Options ouvertes | Validation principale | ADR (prévue) | Statut |
|---------|----------|------------------|-----------------------|--------------|--------|
| **DEC-L1-001** | Allocation GPIO/bus de référence du Host (et recours éventuel à un GPIO expander / partage de bus) | Directe (~11 E/S) · autre module · expander I²C · partage SPI écran/CX-Bus · échelle ADC boutons · évolution de périmètre | **L2** (bring-up) | 0013 | **Ouvert** — allocation de **référence** proposée (L1) |
| **DEC-L1-002** | Architecture de cœur de calcul retenue | (A) XIAO ESP32-S3 · (B) autre module ESP32-S3 · (C) carte custom (ESP32-S3 nu) | **L4 + L5** (RAM/écran, deep-sleep/socket) | (0018 / dédiée) | **Ouvert** — comparaison documentaire (L1) |

> Les identifiants `DEC-*` des lots suivants seront ajoutés ici à l'ouverture de
> chaque lot (une PR par lot).

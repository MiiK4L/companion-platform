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

> À ce stade (**lots documentaires** L1 et L2A), les décisions ci-dessous sont
> **Ouvertes** et leurs éléments **Proposés** : aucune n'est arbitrée, **aucune
> ADR n'est créée** (les validations reproductibles nécessaires relèvent des
> campagnes de mesure à venir).

## Lot 1 — Exigences & architecture du cœur

| `DEC-*` | Question | Options ouvertes | Validation principale | ADR (prévue) | Statut |
|---------|----------|------------------|-----------------------|--------------|--------|
| **DEC-L1-001** | Allocation GPIO/bus de référence du Host (et recours éventuel à un GPIO expander / partage de bus) | Directe (~11 E/S) · autre module · expander I²C · partage SPI écran/CX-Bus · échelle ADC boutons · évolution de périmètre | **L2** (bring-up) | 0013 | **Ouvert** — allocation de **référence** proposée (L1) |
| **DEC-L1-002** | Architecture de cœur de calcul retenue | (A) XIAO ESP32-S3 · (B) autre module ESP32-S3 · (C) carte custom (ESP32-S3 nu) | **L4 + L5** (RAM/écran, deep-sleep/socket) | (0018 / dédiée) | **Ouvert** — comparaison documentaire (L1) |

## Lot 2A — Sûreté électrique du CX-Bus

| `DEC-*` | Question | Options ouvertes | Validation principale | ADR (prévue) | Statut |
|---------|----------|------------------|-----------------------|--------------|--------|
| **DEC-L2-001** | Sûreté électrique du CX-Bus : principe d'isolation/commutation, protections, séquencement ; seuils inrush / court-circuit / bus-stuck / hot-plug électrique / retrait en transaction | Load switch (dédié / P-MOSFET / eFuse) · isolation I²C (répéteur / level-shifter / bus switch) · séquence d'alimentation · récupération bus (SCL / power-cycle / switch) | **L2** (banc : injection, oscillo, alim limitée) | 0014 | **Ouvert** — analyse + protocoles figés (L2A), **aucune mesure exécutée** |

> **Lot 2B (PR ultérieure)** ouvrira **`DEC-L2-002`** (→ ADR-0015) : **connecteur**
> (famille, brochage, **rails exposés dont `VBAT`**, endurance, résistance de
> contact). L'[analyse back-powering](lot-2/back-powering.md) du Lot 2A en est une
> **entrée**. Non ouvert ici.

> Les identifiants `DEC-*` des lots suivants seront ajoutés ici à l'ouverture de
> chaque lot (une PR par lot).

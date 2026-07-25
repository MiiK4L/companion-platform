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

> À ce stade (**lots documentaires** L1, L2A, L2B et L3), les décisions ci-dessous
> sont **Ouvertes** et leurs éléments **Proposés** : aucune n'est arbitrée,
> **aucune ADR n'est créée** (les validations reproductibles nécessaires relèvent
> des campagnes de mesure à venir).

## Lot 1 — Exigences & architecture du cœur

| `DEC-*` | Question | Options ouvertes | Validation principale | ADR (prévue) | Statut |
|---------|----------|------------------|-----------------------|--------------|--------|
| **DEC-L1-001** | Allocation GPIO/bus de référence du Host (et recours éventuel à un GPIO expander / partage de bus) | Directe (~11 E/S) · autre module · expander I²C · partage SPI écran/CX-Bus · échelle ADC boutons · évolution de périmètre | **L2** (bring-up) | 0013 | **Ouvert** — allocation de **référence** proposée (L1) |
| **DEC-L1-002** | Architecture de cœur de calcul retenue | (A) XIAO ESP32-S3 · (B) autre module ESP32-S3 · (C) carte custom (ESP32-S3 nu) | **L4 + L5** (RAM/écran, deep-sleep/socket) | (0018 / dédiée) | **Ouvert** — comparaison documentaire (L1) |

## Lot 2A — Sûreté électrique du CX-Bus

| `DEC-*` | Question | Options ouvertes | Validation principale | ADR (prévue) | Statut |
|---------|----------|------------------|-----------------------|--------------|--------|
| **DEC-L2-001** | Sûreté électrique du CX-Bus : principe d'isolation/commutation, protections, séquencement ; seuils inrush / court-circuit / bus-stuck / hot-plug électrique / retrait en transaction | Load switch (dédié / P-MOSFET / eFuse) · isolation I²C (répéteur / level-shifter / bus switch) · séquence d'alimentation · récupération bus (SCL / power-cycle / switch) | **L2** (banc : injection, oscillo, alim limitée) | 0014 | **Ouvert** — analyse + protocoles **rédigés (Brouillon, baselining requis)** (L2A), **aucune mesure exécutée** |

## Lot 2B — Connecteur & endurance

| `DEC-*` | Question | Options ouvertes | Validation principale | ADR (prévue) | Statut |
|---------|----------|------------------|-----------------------|--------------|--------|
| **DEC-L2-002** | Famille de connecteurs, contraintes mécaniques, endurance et **mapping physique candidat** des contacts | Mezzanine · card-edge · FPC/FFC · pogo · filaire verrouillable (témoin) | **L2** (banc mécanique : endurance, R_contact 4 fils, intermittence, force) | 0015 | **Ouvert** — comparaison documentaire (L2B) |
| **DEC-L2-003** | Rails d'alimentation **effectivement exposés** (dont `VBAT`) et **stratégie de puissance** des modules | P1 (`VMOD` seul) · P2 (`VBAT` exposé, analyse sécurité) · P3 (`VUSB`) | **L2** (entrée : back-powering 2A) | **future — n° attribué à la création** | **Ouvert** — analyse séparée (L2B) |

> Le **brochage** distingue l'**allocation logique** (contrainte **SPEC**
> révisable, **hors** `DEC-L2-002`) du **mapping physique candidat** (relève de
> `DEC-L2-002`) — voir [pinout](lot-2b/pinout.md). `DEC-L2-002` et `DEC-L2-003`
> **évoluent indépendamment**. **Aucun MPN, aucune famille retenue, aucune ADR.**

## Lot 3 — Identification des modules

| `DEC-*` | Question | Options ouvertes | Validation principale | ADR (prévue) | Statut |
|---------|----------|------------------|-----------------------|--------------|--------|
| **DEC-L3-001** | Architecture de découverte et d'identification logique (découvrabilité logique, identité produit/instance, provisioning, comportement en panne ; présence physique = mécanisme dédié éventuel, hors résultat implicite) | EEPROM+EUI · EEPROM simple · 1-Wire · µC actif · Secure Element (+ mém.) · **hybrides** | **L3** (banc HW+CODE : lecture sûre, résolution, provisioning) | 0016 | **Ouvert** — comparaison documentaire (L3) |
| **DEC-L3-002** | Format et contenu du **Manifest** (structure, versionnement, capacités, compatibilité, extensibilité) | TLV borné · champs oblig./opt. · politique dégradé | **L3** (parsing borné prouvé) | **future — n° attribué à la création** | **Ouvert** — format candidat (L3) |

> **Authentification** : **aucune DEC ouverte** (hors V1) — exigence
> d'**évolutivité** documentée ([évolutivité](lot-3/authentication-evolutivity.md)) ;
> une DEC dédiée ne sera ouverte que si une vraie décision se présente. Les trois
> couches **Identité / Description / Authentification** restent distinctes. **Aucun
> MPN, aucune architecture retenue, aucune ADR.**

> Les identifiants `DEC-*` des lots suivants seront ajoutés ici à l'ouverture de
> chaque lot (une PR par lot).

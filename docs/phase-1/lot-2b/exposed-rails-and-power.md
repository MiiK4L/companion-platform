<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2B — Rails exposés & stratégie de puissance (*Proposé*, `DEC-L2-003`)

> **Statut : Ouvert / Proposé.** Décision **séparée** de la famille de connecteur :
> elle peut évoluer **indépendamment** de `DEC-L2-002`. **Entrée principale = le
> [back-powering du Lot 2A](../lot-2/back-powering.md)** et la
> [power-architecture 2A](../lot-2/power-architecture.md). Valeurs
> **[DS]/[H]/[P]/[BL]** ; **aucune [M]**. Le plan de projet exige **« pas de
> `VBAT` sur connecteur sans analyse de sécurité »**.

## 1. Question de décision

**Quels rails le slot expose-t-il, et comment le module est-il alimenté ?**
Options **au même niveau** (reprises de la [power-architecture 2A](../lot-2/power-architecture.md) §3) :

| Option | Rails exposés | Alimentation module | Sûreté (entrée 2A) |
|--------|---------------|---------------------|--------------------|
| **P1** | `VMOD` commuté + `GND` + bus | Host fournit `VMOD` régulé/gated | Pas d'exposition `VBAT`/`VUSB` → back-powering minimal |
| **P2** | `VBAT` + `GND` + bus | Module régule lui-même | Exposition `VBAT` → **analyse sécurité requise** |
| **P3** | `VUSB` + `GND` + bus | Module alimenté en session filaire | `VUSB` intermittent ; chemins de reflux à vérifier |

## 2. Critères d'arbitrage (entrées, non tranchés)

| Critère | Source |
|---------|--------|
| Courant de fuite / réveil parasite (`VMOD` coupé) | [back-powering 2A](../lot-2/back-powering.md) (`[BL]`) |
| Tenue au court-circuit selon le rail exposé | [risques électriques 2A](../lot-2/electrical-risk-analysis.md) |
| Courant requis par le module vs courant/contact | [exigences connecteur](connector-requirements.md) |
| Complexité de régulation (Host vs module) | conception |
| Sûreté d'exposition `VBAT` (protection reflux, fusible) | analyse dédiée (ci-dessous) |

## 3. Exposition de `VBAT` — conditions (le cas échéant)

Si `VBAT` devait être exposé (option P2), l'**analyse de sécurité** (condition du
plan) devra couvrir **au minimum** : protection contre le **reflux**
(idéal-diode / diode), **limitation / fusible**, **détrompage** empêchant un
court entre `VBAT` et une autre broche, comportement en **insertion partielle**.
**Aucune de ces protections n'est figée** ; elles sont **candidates** et
conditionnent l'acceptation de P2.

## 4. Indépendance vis-à-vis de `DEC-L2-002`

Le **choix de la famille** (`DEC-L2-002`) et le **choix des rails exposés**
(`DEC-L2-003`) sont **découplés** : une même famille peut accueillir P1, P2 ou P3.
Ainsi, stabiliser l'un n'oblige pas à figer l'autre.

## 5. Alimente

- **`DEC-L2-003`** — rails exposés + stratégie de puissance, arbitrés **après**
  mesures (2A) et analyse sécurité. **Aucune option retenue ici.**

## Renvois

- [Back-powering 2A](../lot-2/back-powering.md) · [Power-architecture 2A](../lot-2/power-architecture.md)
- [Exigences connecteur](connector-requirements.md) · [Registre des décisions](../decisions-register.md)

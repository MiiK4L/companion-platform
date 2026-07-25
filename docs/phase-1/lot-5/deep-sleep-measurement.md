<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 5 — Mesure du deep-sleep système (*Proposé*, risque R3)

> **Statut : Ouvert / Proposé — MESURE, pas une DEC.** Le deep-sleep système est
> une **mesure** (risque **R3**). Valeurs `[BL]` ; **aucune `[M]`**.

## Rôle de la mesure (élargi)

> Cette mesure alimente **principalement `DEC-L1-002`** (choix du cœur) **et**
> constitue **également une preuve pouvant alimenter les décisions ultérieures**
> liées à l'**alimentation**, aux **domaines de puissance** et à l'**autonomie**
> (L6, future décision power/veille). Elle **ne sert pas qu'au choix du MCU.**

## Question mesurable

Quel est le **courant deep-sleep du système** (cœur monté sur le support), et
est-il **compatible d'une cible d'autonomie** dérivée de **L6** ?

## Cadre de mesure (spécifié, non exécuté)

| Élément | Contenu | Étiquette |
|---------|---------|-----------|
| Grandeur | courant système en deep-sleep (µA) | `banc` |
| Instrument | µA-mètre dédié (ex. PPK2 / Joulescope) | **[H]** |
| Configuration | cœur **socketé** vs **soudé (baseline)** ; périphériques au repos | **[BL]** |
| Cible | seuil dérivé de l'autonomie (**L6**) | **[BL]** |
| Contribution du support | écart socketé − soudé (résistance série/fuites) | `banc` |

## Conséquence (R3)

> **Échec** = deep-sleep **> cible** ⇒ **décision structurante** : réduction de
> conso, ou **arbitrage pouvant rouvrir [ADR-0004](../../adr/0004-coeur-de-calcul-socket.md)
> / `DEC-L1-002`** (autre module, carte custom) — voir
> [déclencheurs d'arbitrage](arbitration-triggers.md).

## Alimente

- **`DEC-L1-002`** (cœur) — **principalement** ; **R3** (registre des risques).
- **Décisions ultérieures power / domaines de puissance / autonomie** — comme
  **preuve** réutilisable. Détails : [protocole deep-sleep](protocols/deep-sleep-current.md).

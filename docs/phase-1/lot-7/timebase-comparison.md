<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 7 — Base de temps : source & validité (*Proposé*, `DEC-L7-001`)

> **Statut : Ouvert / Proposé.** **Deux axes distincts** : la **source de temps**
> et la **validité de l'heure**. Comparaison **par fonction** ; les composants
> **illustrent**. Valeurs `[DS]/[H]/[BL]` ; **aucune `[M]`**.

## Axe 1 — Source de temps (architecture)

| Source | Principe | Illustré par (non retenu) |
|--------|----------|---------------------------|
| **(T1) RTC interne MCU** | horloge interne + resync ; **pas de tenue hors alim** sans domaine RTC alimenté | RTC ESP32-S3 (repli) |
| **(T2) RTC externe standard** | RTC I²C + quartz externe, **sauvegarde par pile/supercap** | PCF8563 |
| **(T3) RTC compensé (TCXO)** | oscillateur **compensé en température** → dérive faible | DS3231 (±2 ppm **[DS]**), RV-3028-C7 (±3 ppm, 45 nA **[DS]**) |

### Critères communs (dont **sources de réveil**, comme critère — pas une famille)

| Critère | Nature |
|---------|--------|
| Dérive / précision (ppm) | `[DS]` → **mesure** (banc) |
| Consommation (µA/nA) | `[DS]` → mesure ; entrée [budget L6](../lot-6/energy-budget-method.md) |
| **Sources de réveil disponibles** | alarme RTC · GPIO · **accéléromètre (mouvement)** · timer · USB — **capacité** de l'architecture |
| Budget IRQ / GPIO | ↔ [L1](../lot-1/gpio-bus-budget.md) (`DEC-L1-001`) |
| Tenue de l'heure hors alimentation | pile/supercap/switchover **[DS]** |
| Δt au réveil | mesure (banc) |
| Disponibilité / secondes sources | sourcing |

> **Sources de réveil** = **critère** de chaque architecture (elles découlent des
> capacités matérielles : RTC, GPIO, accéléromètre, timer, USB), **pas** une famille
> de comparaison indépendante.

## Axe 2 — Validité de l'heure (distinct de la source)

> **Choisir un RTC ne garantit pas qu'une heure soit fiable après une perte
> d'alimentation.** La **validité** est un **état** géré par le firmware, orthogonal
> à la source.

| État | Signification | Conséquence |
|------|---------------|-------------|
| **Valide** | heure tenue (sauvegarde ok) ou resynchronisée récemment | utilisable |
| **Inconnue** | 1er démarrage, sauvegarde perdue, RTC jamais réglé | **ne pas présenter d'heure fausse** ; marquer « inconnue » |
| **Resynchronisée** | corrigée via une [stratégie de resync](resync-strategy.md) | validité restaurée + tracée |

- Le firmware **expose l'état de validité** (valide/inconnue/resynchronisée) et ne
  **fabrique jamais** une heure « plausible ».
- La **transition** inconnue → valide passe par une **resynchronisation** (voir doc dédiée).

## Alimente

- **`DEC-L7-001`** — source **et** gestion de validité, arbitrées **après** mesures
  (dérive/conso/Δt). **Aucune source retenue.**

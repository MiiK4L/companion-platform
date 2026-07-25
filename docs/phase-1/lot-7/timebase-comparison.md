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
| **(T2) RTC externe standard (quartz externe)** | RTC I²C + **quartz externe** ; sauvegarde pile/supercap | PCF8563 |
| **(T3) RTC module ULP (quartz intégré + calibration usine)** | module **ultra-basse conso**, **quartz intégré**, **étalonnage usine + correction programmable** ; **non présenté comme TCXO** | RV-3028-C7 (±3 ppm, 45 nA **[DS]**) |
| **(T4) RTC compensé en température (TCXO)** | **oscillateur compensé en température intégré** → dérive faible sur plage | DS3231 (±2 ppm **[DS]**) |

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

## Axe 2 — Validité de l'heure (ÉTAT : **valide / inconnue**)

> **Choisir un RTC ne garantit pas qu'une heure soit fiable après une perte
> d'alimentation.** La **validité** est un **état** (à **deux valeurs**) géré par le
> firmware, orthogonal à la source.

| État | Signification | Conséquence |
|------|---------------|-------------|
| **heure valide** | heure fiable (source tenue **ou** resynchronisée) | utilisable |
| **heure inconnue** | 1ᵉʳ démarrage, sauvegarde perdue, RTC jamais réglé | **ne jamais présenter d'heure fausse** ; exposer « inconnue » |

> **« resynchronisée » n'est PAS un état** : c'est un **événement** (transition
> `inconnue → valide`, ou mise à jour d'une heure valide) qui **met à jour la
> provenance** (axe 3). Le firmware **n'expose que deux états** (valide / inconnue)
> et ne **fabrique jamais** une heure « plausible ».

## Axe 3 — Provenance / niveau de confiance de l'heure (distinct de l'état)

Quand l'heure est **valide**, sa **provenance** (comment elle a été obtenue)
qualifie le **niveau de confiance** — dimension **distincte** de l'état :

| Provenance | Niveau de confiance (indicatif) |
|------------|---------------------------------|
| **RTC conservé** (sauvegarde/switchover) | selon dérive de la source |
| **Resync USB** (hôte/réseau) | élevé au moment de la resync |
| **Source externe** (réseau, GNSS, module) | élevé |
| **Saisie utilisateur** | selon l'utilisateur |

> La **provenance** est **horodatée et tracée** ; elle est **mise à jour par un
> événement de resynchronisation** ([politique de resync](resync-strategy.md)).

## Alimente

- **`DEC-L7-001`** — source **et** gestion de validité, arbitrées **après** mesures
  (dérive/conso/Δt). **Aucune source retenue.**

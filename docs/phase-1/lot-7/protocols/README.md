<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 7 — Protocoles d'essai (base de temps & persistance)

> **Statut : Brouillon (rédigés, en attente de baselining).** Protocoles
> **génériques**, **un objectif par protocole**. **Aucune campagne exécutée**,
> **aucune `[M]`**. **Tous les seuils sont `[BL]`** (dérive, conso, Δt, reprise)
> — définis au **baselining**, **pas** figés ici.

## Cette PR sélectionne, elle n'arbitre pas

> Cette PR **documentaire** aboutit à une **liste d'options à prototyper** (source
> de temps, resync, persistance), **sans les arbitrer**. Prototypes et mesures en
> **PR/campagne ultérieure**. **Aucun `Accepté`** sur documentaire seul.

## Cycle de vie (identique aux lots précédents)

```
Brouillon → Revu → Baseline d'essai → Exécuté → Rapporté
```

Passage en **Baseline d'essai** = tous les `[BL]` levés + **fiche de baseline**
(commit, date, approbateur, campagne autorisée, `[BL]` levés, hash firmware/banc)
— même modèle qu'en [Lot 2A](../../lot-2/protocols/README.md). **Aucune campagne
tant qu'un `[BL]` subsiste.**

## Type d'essai

| Type | Grandeurs de couverture |
|------|-------------------------|
| **Matériel + mesure fine** (dérive, conso RTC) | référence de temps, µA-mètre, durée |
| **Matériel + code** (Δt/cycles, reprise) | cycles deep-sleep, coupures brutales, checksum |

## Protocoles

| Protocole | Objectif | Décision |
|-----------|----------|----------|
| [Dérive vs référence](drift-vs-reference.md) | dérive (ppm) sur durée | `DEC-L7-001` |
| [Consommation RTC](rtc-consumption.md) | conso de la base de temps | `DEC-L7-001` (↔ L6) |
| [Δt sur cycles deep-sleep](delta-t-wake-cycles.md) | exactitude du temps au réveil | `DEC-L7-001` |
| [Reprise d'état sur coupures brutales](state-recovery-brutal-cuts.md) | 0 corruption, heure non fausse | `DEC-L7-002` |

> Verdicts instrumentés : voir [définitions des événements](event-definitions.md).
> [Modèle de rapport](../../templates/measurement-report-template.md) ; **aucun
> rapport vide versionné**.

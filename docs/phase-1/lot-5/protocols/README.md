<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 5 — Protocoles d'essai (cœur & socketage)

> **Statut : Brouillon (rédigés, en attente de baselining).** Protocoles
> **génériques**, **un objectif par protocole**. **Aucune campagne exécutée**,
> **aucune `[M]`**. Seuils `[P]/[BL]`.

## Cette PR sélectionne, elle n'arbitre pas

> Cette PR **documentaire** aboutit à une **liste d'options à prototyper** (support
> + baseline soudée), **sans les arbitrer**. Les **prototypes et mesures** relèvent
> d'une **PR/campagne ultérieure**. **Aucun `Accepté`** sur documentaire seul.

## Cycle de vie (identique aux lots précédents)

```
Brouillon → Revu → Baseline d'essai → Exécuté → Rapporté
```

Passage en **Baseline d'essai** = tous les `[BL]` levés + **fiche de baseline**
(commit, date, approbateur, campagne autorisée, `[BL]` levés, hash firmware/banc)
— même modèle qu'en [Lot 2A](../../lot-2/protocols/README.md). Gel : modif
post-baseline = **écart tracé, non rétroactif** ; **aucune campagne tant qu'un
`[BL]` subsiste**.

## Type d'essai

| Type | Grandeurs de couverture |
|------|-------------------------|
| **Matériel** (contact, endurance, vibration, usure, épaisseur) | `n_dut` (supports), révisions, `n_campaigns` |
| **Matériel + mesure fine** (deep-sleep µA) | µA-mètre, config socketé/soudé, `n_campaigns` |
| **Matériel + code** (GPIO cœur absent) | fixture, états cœur, analyseur de bus |

## Protocoles

| Protocole | Objectif | Alimente |
|-----------|----------|----------|
| [Deep-sleep système (µA)](deep-sleep-current.md) | conso deep-sleep socketé vs soudé | `DEC-L1-002` + R3 (+ power/autonomie) |
| [Résistance de contact du support](contact-resistance-socket.md) | R_contact + dérive | `DEC-L5-001` |
| [Endurance d'insertion + vibration](insertion-endurance-vibration.md) | cycles, continuité sous vibration, épaisseur | `DEC-L5-001` |
| [Usure du support (remplacements successifs)](support-wear-repeated-swap.md) | **cas d'usage principal** : plusieurs cartes cœur différentes | `DEC-L5-001` |
| [GPIO cœur absent/éteint/remplacé](gpio-core-absent.md) | intégrité bus, back-powering | `DEC-L5-001` (↔ L2A) |

> Verdicts instrumentés : voir [définitions des événements](event-definitions.md).
> [Modèle de rapport](../../templates/measurement-report-template.md) ; **aucun
> rapport vide versionné**.

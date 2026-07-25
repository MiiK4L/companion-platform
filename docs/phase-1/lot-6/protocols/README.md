<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 6 — Protocoles d'essai (énergie)

> **Statut : Brouillon (rédigés, en attente de baselining).** Protocoles
> **génériques**, **un objectif par protocole**. **Aucune campagne exécutée**,
> **aucune `[M]`**. **Tous les seuils sont `[BL]`** (précision, fuite, ΔT,
> autonomie) — **définis au baselining**, **pas** figés ici.

## Cette PR sélectionne, elle n'arbitre pas

> Cette PR **documentaire** aboutit à une **liste d'options à prototyper** (charge,
> régulation, estimation, stockage), **sans les arbitrer**. Les **prototypes et
> mesures** relèvent d'une **PR/campagne ultérieure**. **Aucun `Accepté`** sur
> documentaire seul.

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
| **Matériel + mesure fine** (conso, fuite) | µA-mètre, coins `VBAT`, `n_campaigns` |
| **Matériel** (charge/thermique, décharge) | alim/charge, thermocouple, `n_dut` cellules |
| **Matériel + référence** (précision d'estimation) | référence **coulomb** vs approche testée |

## Protocoles

| Protocole | Objectif | Décision |
|-----------|----------|----------|
| [Conso par mode](consumption-per-mode.md) | conso aux modes × coins `VBAT` | `DEC-L6-001/002` |
| [Précision d'estimation d'état](state-estimation-accuracy.md) | erreur vs référence coulomb | `DEC-L6-003` |
| [Fuite module coupé](module-leakage.md) | fuite `VMOD` coupé | `DEC-L6-001` (↔ L2-003) |
| [Charge & thermique](charge-thermal.md) | charge complète + ΔT | `DEC-L6-001` |
| [Décharge réelle](real-discharge.md) | capacité utile vs estimation | `DEC-L6-002` |

> Verdicts instrumentés : voir [définitions des événements](event-definitions.md).
> [Modèle de rapport](../../templates/measurement-report-template.md) ; **aucun
> rapport vide versionné**.

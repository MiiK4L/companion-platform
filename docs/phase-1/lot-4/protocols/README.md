<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 4 — Protocoles d'essai (écran & moteur graphique)

> **Statut : Brouillon (rédigés, en attente de baselining).** Protocoles
> **génériques**, **un objectif par protocole**. **Aucune campagne exécutée**,
> **aucune `[M]`**. Seuils `[P]/[BL]`.

## Cette PR sélectionne, elle n'arbitre pas

> Cette PR **documentaire** aboutit à une **liste d'options à prototyper**
> (contrôleur/famille + moteur), **sans les arbitrer**. Les **prototypes et
> mesures** relèvent d'une **PR/campagne ultérieure**. **Aucun `Accepté`** sur la
> seule comparaison documentaire.

## Cycle de vie (identique aux lots précédents)

```
Brouillon → Revu → Baseline d'essai → Exécuté → Rapporté
```

Passage en **Baseline d'essai** = tous les `[BL]` levés + **fiche de baseline**
(commit, date, approbateur, campagne autorisée, `[BL]` levés, hash firmware/banc)
— même modèle qu'en [Lot 2A](../../lot-2/protocols/README.md). Gel : modif
post-baseline = **écart tracé, non rétroactif** ; **aucune campagne tant qu'un
`[BL]` subsiste**.

## Type d'essai (le plan d'échantillonnage s'y adapte)

| Type | Grandeurs de couverture |
|------|-------------------------|
| **Logiciel** (budget RAM du moteur) | build/config versionnés, mesures d'empreinte, versions moteur |
| **Matériel + code** (FPS, perçu, conso, SPI) | `n_dut` (panneaux), révisions, `n_campaigns`, instrumentation |

## Protocoles

| Protocole | Objectif | `DEC-*` |
|-----------|----------|---------|
| [Budget RAM du moteur](ram-budget.md) | empreinte mémoire vs budget L1 | `DEC-L4-002` |
| [FPS / rendu](rendering-fps.md) | débit d'images sous scénarios | `DEC-L4-002` |
| [Performances perçues](perceived-performance.md) | réveil, init, tearing, débit d'images (FPS), jitter, latence | `DEC-L4-001/002` |
| [Conso & rétroéclairage](backlight-power.md) | conso par niveau ; extinction/reprise | `DEC-L4-001` |
| [Contention SPI](spi-contention.md) | partage écran ↔ CX-Bus (R2) | `DEC-L4-001` |

> Verdicts instrumentés : voir [définitions des événements](event-definitions.md).
> [Modèle de rapport](../../templates/measurement-report-template.md) ; **aucun
> rapport vide versionné**.

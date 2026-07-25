<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 5 — Composants candidats (séries représentatives documentaires, *Proposé*)

> **Statut : Ouvert / Proposé — PRÉ-SHORTLIST DOCUMENTAIRE.** **≥ 2 séries
> sourcées par famille socketable** (définition en [Lot 2B](../lot-2b/README.md)).
> **Aucune retenue, aucun MPN final.** `[DS]` = fiche ; `à relever`/`inconnu`
> sinon ; **aucune `[M]`**. Statut de caractérisation par ligne ; cycle de vie à
> confirmer au sourcing (`R8`).

## (S1) Embases 2,54 mm (barrettes)

| Série | Fab. | Type | Contact | Source | Caractérisation |
|-------|------|------|---------|--------|-----------------|
| **D01** (IC socket strips) | Harwin | embase femelle SIL 2,54 mm | doré, accepte broches 0,40–0,56 mm **[DS]** | Harwin D01 | partielle |
| **SSW / SSQ** | Samtec | socket strips .100" square post | à relever | Samtec SSW/SSQ | partielle |

## (S2) Embases de précision (tulipe)

| Série | Fab. | Type | Contact | Source | Caractérisation |
|-------|------|------|---------|--------|-----------------|
| **Pin receptacles** (machined) | Mill-Max | embase tulipe usinée | contact **béryllium multi-doigts**, large plage de broches **[DS]** | Mill-Max (récept. usinées) | partielle |
| **Précision (tulipe)** | Preci-Dip | embase tulipe usinée | à relever | Preci-Dip | à compléter avant shortlist |

## (S3) Board-to-board / mezzanine (réutilise L2B)

| Série | Fab. | Type | Source | Caractérisation |
|-------|------|------|--------|-----------------|
| **DF40** | Hirose | mezzanine 0,4 mm | (cf. [L2B](../lot-2b/families-comparison.md)) | partielle |
| **SlimStack** | Molex | mezzanine 0,4 mm | (cf. [L2B](../lot-2b/families-comparison.md)) | à compléter |

## (S4) Socket ZIF/LIF

| Série | Fab. | Type | Remarque | Source | Caractérisation |
|-------|------|------|----------|--------|-----------------|
| **ZIF/LIF** (0,5–2,54 mm) | Aries Electronics | socket ZIF/LIF module/board | 1–99 contacts | Aries ZIF/LIF | à compléter avant shortlist |
| **WR-FPC (ZIF/LIF)** | Würth | ZIF/LIF | **souvent orienté FFC** ; **support de module à sourcer** | Würth WR-FPC | à compléter avant shortlist |

> Le **support de module** ZIF/LIF (par opposition au ZIF pour **FFC/FPC**) est une
> **niche** : séries **à confirmer** au sourcing.

## (B0) Baseline non démontable — *pas une série MPN*

> La baseline est un **module directement soudé** (**castellations ou équivalent**,
> cf. [README](README.md)) : une **implémentation**, **pas** une famille de
> connecteur — **aucun MPN** à retenir. Sert de **référence** conso/contact/épaisseur.

## Sources datasheet (traçabilité)

> Consultées le **2026-07-23** ; `[DS]` issues de ces documents ; références « à
> compléter » **sans `[DS]`** tant qu'une série précise n'est pas documentée.

| Réf. | Fabricant | Document | Date | Référence stable |
|------|-----------|----------|------|------------------|
| D01 | Harwin | IC socket strip 2,54 mm (D01-99xxx) | 2026-07-23 | `harwin.com` (D01) |
| SSW/SSQ | Samtec | .100" socket strips | 2026-07-23 | `samtec.com` (SSW/SSQ) |
| Pin receptacles | Mill-Max | Precision Machined PCB Pin Receptacles | 2026-07-23 | `mill-max.com` (receptacles) |
| ZIF/LIF | Aries Electronics | ZIF/LIF connectors | 2026-07-23 | `arieselec.com` (ZIF/LIF) |
| DF40 / SlimStack | Hirose / Molex | (cf. [L2B](../lot-2b/families-comparison.md)) | 2026-07-23 | cf. L2B |

## Alimente

- **`DEC-L5-001`** — ces séries **alimentent partiellement** la comparaison ;
  **aucune retenue**, aucun MPN. Familles à disponibilité **à confirmer**
  (ZIF/LIF de module) explicitement notées.

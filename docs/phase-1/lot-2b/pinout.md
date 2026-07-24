<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2B — Brochage : allocation logique vs mapping physique (*Proposé*)

> **Statut : Ouvert / Proposé.** Le brochage a **deux dimensions distinctes** à ne
> pas confondre. Un **changement de famille** de connecteur **ne doit pas**
> forcer à redécider la **sémantique du bus**. Valeurs **[H]/[P]/[BL]** ;
> **aucune [M]**.

## 1. Allocation logique des signaux (contrainte SPEC — révisable, **hors `DEC-L2-002`**)

L'**allocation logique** (quels signaux composent le CX-Bus et leur rôle) est une
**contrainte de la SPEC** et une **entrée de conception révisable**, **pas** une
décision de connecteur. Elle survivra à un changement de famille.

Signaux logiques (référence de travail, révisable) : `GND` · `VMOD` (alim
commutée) · I²C (`SDA`/`SCL`) · SPI (`SCK`/`MOSI`/`MISO` + `CS`) · IRQ module ·
présence module · (rails additionnels éventuels selon
[rails exposés](exposed-rails-and-power.md), `DEC-L2-003`).

> Cette liste **n'est pas figée** ici ; elle relève de la SPEC (au plus `rc.1` en
> Phase 1) et des budgets [L1](../lot-1/gpio-bus-budget.md). Le Lot 2B la **prend
> en entrée**, il ne la décide pas.

## 2. Mapping physique candidat (relève de `DEC-L2-002`)

Le **mapping physique** (position réelle des contacts) **dépend de la famille**
retenue et fait donc partie de `DEC-L2-002`. Il sera proposé **par famille** une
fois la comparaison instruite, en respectant :

- le **séquencement candidat** (§3) lorsque la famille le permet ;
- le **détrompage** et la **robustesse** ([exigences](connector-requirements.md)) ;
- l'**intégrité SPI** (lignes rapides, longueur/retour de masse).

> Aucun mapping physique n'est figé ici : il est **candidat**, révisable selon les
> mesures et la famille.

## 3. Séquencement de masse — **candidat, à confirmer** (ne pas présenter comme acquis)

Le [Lot 2A](../lot-2/hot-plug.md) a **étudié** l'ordre des contacts mais **n'a pas
produit de mesures**. On retient donc un **séquencement candidat visant à établir
la référence de masse avant les rails et signaux sensibles**, **à confirmer par
les essais électriques (2A) et par les possibilités réelles de la famille**.

Certaines familles **ne permettent pas** de contacts échelonnés. Il faut donc
**comparer les moyens** d'obtenir (ou de compenser) ce séquencement :

| Moyen | Principe | Familles concernées |
|-------|----------|---------------------|
| **Contacts de longueurs différentes** | `GND`/alim plus longs → contact anticipé | card-edge, pogo |
| **Guidage mécanique** | Détrompeur/guide imposant l'ordre d'engagement | selon boîtier |
| **Précharge de masse** | Contact de masse dédié pré-établi | à concevoir |
| **Séquencement électrique externe** | Load switch / logique côté Host (indépendant du connecteur) | toutes (cf. [isolation 2A](../lot-2/isolation-and-switching.md)) |
| **Absence de séquencement physique** | Compensée par les **protections du Lot 2A** (haute-Z, rampe, limitation) | toutes |

> Conclusion **ouverte** : selon la famille, le séquencement de masse sera obtenu
> **mécaniquement**, **électriquement**, ou **compensé** par les protections 2A.
> Aucun de ces moyens n'est imposé ; le choix se fera avec les mesures.

## Alimente

- **`DEC-L2-002`** — via le **mapping physique candidat** et les contraintes de
  séquencement réalisables par famille.
- L'**allocation logique** reste une **entrée SPEC révisable** (hors décision).

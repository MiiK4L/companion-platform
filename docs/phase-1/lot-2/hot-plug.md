<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2A — Comportement électrique du hot-plug (*Proposé*, `DEC-L2-001`)

> **Statut : Ouvert / Proposé.** Étude du hot-plug **au niveau électrique
> uniquement**, avec un **banc d'injection contrôlé indépendant du connecteur
> final** (le connecteur relève du Lot 2B). L'**ordre des contacts** est traité
> en **scénarios/exigences**, **non figé**. Valeurs **[DS]/[C]/[H]/[P]** ;
> **aucune [M]**.

## 1. Banc d'injection (indépendant du connecteur)

Pour découpler la sûreté électrique du choix mécanique du connecteur, le hot-plug
est reproduit par un **banc d'injection** : interrupteurs, relais ou fixture
équivalente commutant individuellement `VMOD`, `GND` et chaque ligne de bus, avec
**timing contrôlé**. Cela permet d'étudier les transitoires (inrush, rebond,
séquence de contacts) **sans** dépendre d'une famille de connecteur.

> Ce banc modélise le **comportement électrique** ; l'endurance mécanique et la
> résistance de contact d'un connecteur **réel** relèvent du **Lot 2B**.

## 2. Scénarios d'ordre des contacts (exigences fonctionnelles, non figées)

L'ordre **souhaité** exprime des exigences, sans présumer du brochage (2B) :

| Étape | Exigence fonctionnelle | Justification |
|-------|------------------------|---------------|
| 1 | **`GND` établi en premier** | Référence commune avant tout signal/alim |
| 2 | Détection présence (ligne dédiée) | Autorise la séquence de mise sous tension |
| 3 | `VMOD` (rampe contrôlée) | Inrush borné |
| 4 | Lignes de bus (après `VMOD` stable) | Évite le back-powering |
| — retrait — | ordre **inverse** : bus, puis `VMOD`, `GND` en dernier | Évite reflux et perte de référence |

Scénarios à injecter (y compris **défavorables**) : `GND` en dernier, bus avant
`VMOD`, contacts intermittents (rebond), insertion partielle.

## 3. Cas de mesure prévus

| Cas | Observable | Seuil proposé | Étiquette |
|-----|-----------|---------------|-----------|
| Insertion nominale | Pic d'inrush, rampe `VMOD` | inrush ≤ 2× I établi ; rampe ≥ 1 ms | **[P]** |
| Module non alimenté, bus actif | Réveil parasite / reflux | pas de réveil ; fuite ≤ `[BL]` µA | **[P]/[BL]** |
| Rebond de contact (N × commutations rapides) | Reset Host, corruption bus | 0 reset / 0 corruption sur ≥ 500 cycles | **[P]** |
| Ordre défavorable (bus avant `VMOD`) | Latch-up, courant anormal | pas de latch-up ; courant borné | **[P]** |

Conditions complètes : [protocole hot-plug/injection](protocols/hot-plug-injection.md).

## 4. Lien avec le SPI partagé (R2)

Le hot-plug pendant un **trafic SPI écran** est un cas critique (**R2**) : la
commutation des lignes ne doit pas glitcher l'écran. Vérifié conjointement au
protocole hot-plug (trafic I²C **et** SPI actifs) et affiné en L4.

## 5. Alimente

- **`DEC-L2-001`** — exigences hot-plug (inrush, absence de reset/corruption,
  séquencement) ; l'**ordre des contacts** devient une **contrainte de brochage**
  transmise au **Lot 2B**.

## Renvois

- [Bus-stuck & retrait en transaction](bus-stuck-and-transaction.md)
- [Isolation & commutation](isolation-and-switching.md) · [Protocoles](protocols/README.md)

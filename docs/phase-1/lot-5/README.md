<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 5 — Cœur remplaçable & stratégie de socketage

> **Statut : en cours (exécution documentaire).** Valide **comment** rendre le
> cœur remplaçable (**principe acté**, non la mise en œuvre) et **cadre la mesure
> deep-sleep**. **Aucune techno privilégiée**, **aucune ADR**, **rien en Accepté**,
> **aucune mesure inventée**, **aucun prototype exécuté**. Voir le
> [plan](../plan.md) (Lot 5) et le [cadre de validation](../validation-framework.md).

## Distinction structurante : principe acté vs mise en œuvre ouverte

| Niveau | Statut |
|--------|--------|
| **Principe** — cœur de calcul **remplaçable** | **Déjà Accepté** ([ADR-0004](../../adr/0004-coeur-de-calcul-socket.md)) — **hors** de ce lot |
| **Mise en œuvre** — format de **socket / support** concret | **Ouvert** — `DEC-L5-001` (mécanique **+** électrique) |
| **Viabilité deep-sleep** du cœur candidat | **mesure** (voir ci-dessous), **pas** une DEC |

## Rôle de la mesure deep-sleep (pas de DEC dédiée)

> Le **deep-sleep système** est une **mesure** (risque **R3**) qui alimente
> **principalement `DEC-L1-002`** (choix du cœur) **et** constitue **également une
> preuve pouvant alimenter les décisions ultérieures** liées à l'**alimentation**,
> aux **domaines de puissance** et à l'**autonomie** (L6, et toute future décision
> power/veille). Elle **ne sert pas qu'au choix du MCU**.

Aucune **DEC-L5-002** n'est ouverte. Si une véritable décision power/veille émerge
plus tard (PMIC, domaines d'alimentation, stratégie de réveil), une DEC dédiée
sera ouverte **à ce moment-là**.

## Baseline non démontable

> La **baseline non démontable** correspond à un **module directement soudé**
> (**castellations ou solution équivalente selon l'implémentation**). Les
> castellations **ne sont qu'une manière** de réaliser cette référence, **pas
> l'unique solution de référence**. Cette baseline sert de **point de comparaison**
> (électrique, mécanique, consommation) — **elle n'est pas remplaçable**.

## Terminologie & conventions

- **Série représentative documentaire** : définition unique en
  [Lot 2B](../lot-2b/README.md) (série réelle, **jamais retenue**, aucun MPN
  final) — **≥ 2 séries sourcées par famille** de support **socketable**.
- **Statut des valeurs** : `[DS]` · `[C]` · `[H]` · `[BL]` · `N/A` · `inconnu` ·
  `[M]` (**aucune `[M]`**).

## Livrables (tous *Proposé*)

| Livrable | Contenu |
|----------|---------|
| [Comparaison des supports](socket-support-comparison.md) | **5 familles** socketables + **baseline soudée** ; critères communs ; sans gagnant |
| [Contraintes élec. & méca.](electrical-mechanical-constraints.md) | Contact/signal/courant ; épaisseur/rétention/vibration |
| [Mesure deep-sleep](deep-sleep-measurement.md) | Cadrage µA système ; alimente `DEC-L1-002` **et** décisions power/autonomie |
| [GPIO cœur absent](gpio-behavior-core-absent.md) | Cœur absent/éteint/remplacé ; intégrité bus & back-powering (↔ [L2A](../lot-2/back-powering.md)) |
| [Déclencheurs d'arbitrage](arbitration-triggers.md) | Étude autre module / carte custom (peut rouvrir [ADR-0004](../../adr/0004-coeur-de-calcul-socket.md)) |
| [Composants candidats](candidate-components.md) | Séries représentatives documentaires sourcées (≥ 2/famille) |
| [Protocoles](protocols/README.md) | Typés (Brouillon, `[BL]`) : deep-sleep, contact, endurance+vibration, **usure du support (remplacements successifs)**, GPIO cœur absent |

## Ce que ce lot NE fait pas

- Ne **retient** aucun support, ne **fige** rien, ne crée **aucune ADR**.
- **Sélectionne** des options à prototyper **sans les arbitrer** ; mesures en
  **PR/campagne ultérieure**. **Aucun `Accepté`** sur documentaire seul.

## Critères de sortie

- **5 familles** + baseline soudée comparées ; **aucun gagnant**.
- Contraintes élec/méca **cadrées** ; mesure deep-sleep **spécifiée** (rôle élargi).
- Comportement GPIO cœur absent + **intégrité bus/back-powering** documenté.
- **Cas d'usage principal** (remplacements successifs de cartes cœur différentes)
  couvert par un protocole dédié.
- `DEC-L5-001` **ouvert** (socketage) ; deep-sleep **tracé comme mesure R3**.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 1 — Matrice des exigences & comparaison des architectures de cœur

> **Statut : en cours (exécution documentaire — aucun matériel).** Ce lot
> **n'engage aucun prototype** et **ne fige aucun composant** : toutes les
> décisions restent **Proposé**, **aucune ADR n'est créée** (les validations
> reproductibles relèvent des lots suivants). Voir le [plan](../plan.md) (Lot 1)
> et le [cadre de validation](../validation-framework.md).

## Objectif

Partir des **besoins de la plateforme** (indépendants du MCU), établir une
**allocation GPIO/bus de référence**, des **budgets v0** (énergie, mécanique,
coût), et une **comparaison des architectures de cœur** — sans rien décréter.

## Questions de décision ouvertes

- **`DEC-L1-001`** — allocation GPIO/bus de référence (+ expander/partage de bus éventuel).
- **`DEC-L1-002`** — architecture de cœur retenue (A/B/C).

Voir le [registre des décisions](../decisions-register.md).

## Livrables de ce lot

| Livrable | Contenu | Statut |
|----------|---------|--------|
| [Budget GPIO / bus](gpio-bus-budget.md) | Broches XIAO, confrontation au besoin, allocation de référence, constat « expander candidat » | Proposé (`DEC-L1-001`) |
| [Comparaison des architectures de cœur](core-architecture-comparison.md) | Grille A/B/C, cellules « mesure » déférées (L4/L5) | Proposé (`DEC-L1-002`) |
| [Budgets v0](budgets-v0.md) | Énergie (paper), enveloppe mécanique, coût — v0 | Proposé |

## Constat principal (documentaire)

Le budget GPIO du candidat de référence (**11 broches**) **ne tient pas** en
câblage direct : un **GPIO expander I²C** (ou une échelle ADC pour les boutons)
est **fortement pressenti** — **sans être imposé** (coût global comparé à
l'arbitrage). Ce constat est **à valider sur banc en L2**.

## Ce que ce lot NE fait pas / suite

- Aucune **mesure** (contention SPI, deep-sleep, RAM, conso) : **L2/L4/L5/L6**.
- Aucun **gel** (brochage, expander, cœur définitif) ni **ADR**.
- La **validation banc** de l'allocation (`DEC-L1-001` → ADR-0013) est en **L2**.
- Le **Lot 2** (sûreté électrique & prototype CX-Bus) **ne démarre qu'après**
  revue et fusion de ce Lot 1.

## Critères de sortie du lot

- Besoins plateforme et budgets v0 **documentés et cohérents** (avec incertitudes).
- Allocation GPIO/bus de référence **proposée** et son **point de validation
  (L2)** identifié.
- Comparaison A/B/C **cadrée**, cellules de mesure **explicitement déférées**.
- `DEC-L1-001` et `DEC-L1-002` **ouverts et tracés** ; aucune décision arbitrée.

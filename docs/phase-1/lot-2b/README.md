<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2B — Connecteur & endurance

> **Statut : en cours (exécution documentaire — aucune mesure physique).** Ce lot
> **compare, spécifie et cadre** ; il **ne fige aucun connecteur**, ne désigne
> **aucune famille gagnante**, ne cite **aucun MPN**, ne crée **aucune ADR** et ne
> produit **aucune valeur mesurée**. Les campagnes sur banc attendent le matériel
> et leur validation. Voir le [plan](../plan.md) (Lot 2) et le
> [cadre de validation](../validation-framework.md).

## Frontière avec le Lot 2A (fusionné)

Le [Lot 2A](../lot-2/README.md) a traité la **sûreté électrique** (isolation,
back-powering, hot-plug par **banc d'injection indépendant du connecteur**). Le
Lot 2B traite le **mécanique / connecteur** et **réutilise les entrées 2A**
(analyse [back-powering](../lot-2/back-powering.md), séquencement d'alimentation,
[risques électriques](../lot-2/electrical-risk-analysis.md)). **Aucune redite**
des protocoles électriques du 2A.

## Deux décisions séparées (ouvertes ici, *Proposé*)

- **`DEC-L2-002`** — **famille de connecteurs**, contraintes mécaniques,
  **endurance**, et **mapping physique candidat** des contacts. → ADR-0015.
- **`DEC-L2-003`** — **rails d'alimentation effectivement exposés** (dont `VBAT`)
  et **stratégie de puissance** des modules. → **ADR future, numéro attribué à la
  création** (voir [plan des ADR](../adr-plan.md)).

Elles partagent les analyses mais **évoluent indépendamment** : l'une peut être
close sans bloquer l'autre. Voir le [registre des décisions](../decisions-register.md).

> **Le brochage a deux dimensions distinctes**, à ne pas confondre :
>
> - **Allocation logique des signaux CX-Bus** — **contrainte de la SPEC** et
>   **entrée de conception révisable**, **hors** `DEC-L2-002` (un changement de
>   famille ne redécide pas la sémantique du bus).
> - **Mapping physique candidat** (position des contacts selon la famille) —
>   relève de **`DEC-L2-002`**.
>
> Détail dans [pinout.md](pinout.md).

## Terminologie : « série représentative documentaire »

> **Série représentative documentaire** — série de composants réelle, citée
> **uniquement** pour **établir des plages** et **vérifier qu'une famille possède
> des solutions crédibles**, sur la base de sa **fiche technique**. Une telle
> série **n'est pas retenue** pour le projet et **ne préjuge d'aucun choix**. À ne
> pas confondre avec une **série/MPN retenu(e)**, qui n'existera qu'**après**
> sélection de famille et essais. **Ce terme unique** est employé partout (pas de
> « série documentaire » ni « série candidate »).

## Convention : statut d'une valeur numérique

Identique au Lot 2A (voir la [table complète](../lot-2/README.md)) :
**[DS]** datasheet · **[C]** calculé · **[H]** hypothèse · **[P]** seuil proposé ·
**[BL]** à finaliser au baselining · **[M]** mesuré (**aucune [M] à ce stade**).

## Livrables de ce lot (tous *Proposé*)

| Livrable | Décision | Contenu |
|----------|----------|---------|
| [Exigences fonctionnelles](connector-requirements.md) | L2-002 | Besoins du connecteur (indépendants de la famille) |
| [Comparaison des familles](families-comparison.md) | L2-002 | **5 familles** au même niveau, sans gagnant |
| [Brochage](pinout.md) | L2-002 | Allocation **logique** (SPEC, révisable) vs **mapping physique candidat** ; séquencement de masse **candidat** |
| [Rails exposés & puissance](exposed-rails-and-power.md) | **L2-003** | `VBAT`/`VUSB`/`VMOD`, stratégie d'alimentation module (entrée = 2A) |
| [Protocoles](protocols/README.md) | L2-002 | Protocoles **génériques** (endurance, résistance de contact, intermittence, force, inspection) + définitions d'événements ; **annexes fixture par famille créées plus tard** |

## Ce que ce lot NE fait pas

- Aucune **mesure** ni valeur mesurée ; protocoles **génériques spécifiés**, non exécutés.
- Aucun **MPN** ni série précise (recherchés **après** qu'une famille soit retenue).
- Aucune **famille gagnante**, aucun **gel** de connecteur/brochage/rails, aucune **ADR**.
- Le **gel définitif du connecteur** reste prévu en **L9**.

## Critères de sortie du lot

- Exigences fonctionnelles **documentées** (indépendantes de la famille).
- **5 familles** comparées sur des critères homogènes ; **avantages / limites /
  risques par famille**, **sans conclusion** de sélection.
- Allocation **logique** et **mapping physique candidat** clairement **distingués**.
- Rails exposés **analysés séparément** (`DEC-L2-003`).
- Protocoles **génériques** rédigés, prêts pour baselining ; structure d'**annexe
  fixture par famille** définie.
- `DEC-L2-002` et `DEC-L2-003` **ouverts et tracés** ; aucune décision arbitrée.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 9 — Checkpoint documentaire et préparation des campagnes

> **Statut : Proposé — checkpoint documentaire.**
>
> **Les travaux documentaires initiaux (lots L1 à L9) sont terminés. La Phase 1
> reste ouverte jusqu'à l'exécution des campagnes expérimentales prévues par le
> plan.**
>
> Ce lot **prépare** les campagnes de mesure ; il **ne clôt PAS la Phase 1** : le
> **volet expérimental** (mesures, prototypes, rapports, ADR 0013+) et les
> [critères de sortie de la Phase 1](../execution.md) **restent à exécuter**.
> **Aucune DEC n'est arbitrée, aucune ADR 0013+ n'est créée ni acceptée, aucun
> composant n'est retenu, aucune mesure exécutée.** La spécification consolidée
> est un **draft** : **aucune promotion vers `rc.1`** (celle-ci relève de la
> sortie de Phase 1, après mesures).

## Pourquoi ce lot

Le **volet documentaire** de la Phase 1 a instruit **17 questions de décision**
(`DEC-*`) réparties sur 8 lots, rédigé les **protocoles reproductibles** qui les
trancheront, et sourcé des **séries représentatives** — sans jamais mesurer,
retenir ni arbitrer. Le Lot 9 **fait le point** et **prépare la suite** en
produisant la **vue consolidée** nécessaire pour :

- **relire l'ensemble d'un coup** (état des décisions, couplages transverses) ;
- **savoir dans quel ordre** les arbitrages devront être conduits ;
- **savoir objectivement** ce qui est prêt (et ce qui ne l'est pas) pour les
  campagnes de mesure ;
- disposer d'une **spécification consolidée** (draft) rassemblant l'architecture
  actée et les contraintes encore ouvertes.

Il ne **duplique pas** le contenu des lots : la synthèse **renvoie** vers L1–L8.

## Ce que le Lot 9 clôt — et ce qu'il n'est pas

| Clos par ce lot | **Toujours ouvert** (Phase 1 expérimentale) |
|-----------------|---------------------------------------------|
| Le **volet documentaire** L1–L9 (analyses, protocoles rédigés, sourcing, consolidation) | Les **campagnes de mesure** et **prototypes** |
| La **consolidation** (synthèse, roadmap, readiness, spec draft) | La **création des ADR 0013+** (après mesures) |
| — | La **promotion de la SPEC** vers `rc.1` |
| — | Les **critères de sortie de la Phase 1** ([execution](../execution.md)) |

## Livrables du lot

| Document | Rôle |
|----------|------|
| [Synthèse Phase 1 (L1–L8)](synthesis.md) | **Agrégateur** : état des 17 `DEC-*`, couplages transverses, graphe de décisions — **par liens**, sans recopie. |
| [Feuille de route des décisions](decision-roadmap.md) | Pour chaque `DEC-*` : prérequis, protocole associé, preuves attendues, **ordre recommandé** des arbitrages (campagnes de mesure de Phase 1). |
| [Préparation des campagnes de mesure](campaign-readiness.md) | Pour chaque `DEC-*` : état de préparation (doc / protocole rédigé / baseline / banc / composants / mesurabilité) — **critère objectif** de démarrage d'une campagne. |
| [Spécification consolidée de Phase 1 (draft)](consolidated-specification.md) | Assemble l'architecture **actée** (ADR 0000-0012) + les contraintes **ouvertes** (DEC, marquées provisoires) + les interfaces **candidates** + les critères d'acceptation. **Draft, non promue rc.1.** |
| [Hors périmètre](out-of-scope.md) | Ce que le Lot 9 **ne fait pas** (arbitrage, ADR, mesure, gel, clôture de Phase 1). |

Mises à jour **transverses** (là où les documents vivent déjà, sans duplication) :
[registre des décisions](../decisions-register.md), [plan des ADR](../adr-plan.md),
[registre des risques](../risk-register.md), [matrice des exigences](../requirements-matrix.md),
[sourcing & BOM](../sourcing-and-bom.md).

## Ce que le Lot 9 confirme

- **17 `DEC-*` toutes Ouvertes / Proposé** — aucune arbitrée (cf. [registre](../decisions-register.md)).
- **Aucune ADR 0013+ créée** — la règle « ADR *après* mesures reproductibles »
  ([cadre de validation](../validation-framework.md) §3) est respectée : le volet
  documentaire n'a exécuté **aucune mesure**.
- La suite est **gouvernée** : chaque `DEC-*` renvoie à son **protocole** et à son
  **état de préparation**.

## Renvois

- [Revue d'architecture intermédiaire L1–L4](../architecture-review-l1-l4.md) — modèle de la synthèse, étendu ici à L1–L8.
- [Ordre d'exécution & critères de sortie de Phase 1](../execution.md) — critères **de Phase 1**, non satisfaits par ce volet documentaire.
- [Cadre de validation & modèle de preuves](../validation-framework.md).

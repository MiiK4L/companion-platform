<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 9 — Clôture documentaire de la Phase 1 (synthèse, roadmap, spécification consolidée)

> **Statut : Proposé — lot de clôture documentaire.** Ce lot **agrège** les
> travaux des lots L1 à L8 sans rien retenir de nouveau. **Aucune DEC n'est
> arbitrée, aucune ADR 0013+ n'est créée ni acceptée, aucun composant n'est
> retenu, aucun MPN final, aucune mesure exécutée.** La promotion de la
> spécification vers `rc.1` **n'est pas** un livrable de ce lot : elle relève de
> la campagne de mesure (Phase 2).

## Pourquoi ce lot

La Phase 1 est **documentaire** : elle a instruit **17 questions de décision**
(`DEC-*`) réparties sur 8 lots, rédigé les **protocoles reproductibles** qui les
trancheront, et sourcé des **séries représentatives** — sans jamais mesurer,
retenir ni arbitrer. Le Lot 9 **ferme cette phase** en produisant la **vue
consolidée** nécessaire pour :

- **relire l'ensemble d'un coup** (état des décisions, couplages transverses) ;
- **savoir dans quel ordre** les arbitrages devront être conduits ;
- **savoir objectivement** quand la Phase 2 (mesure) peut commencer ;
- disposer d'une **spécification consolidée** (draft) rassemblant l'architecture
  actée et les contraintes encore ouvertes.

Il ne **duplique pas** le contenu des lots : la synthèse **renvoie** vers L1–L8.

## Livrables du lot

| Document | Rôle |
|----------|------|
| [Synthèse Phase 1 (L1–L8)](synthesis.md) | **Agrégateur** : état des 17 `DEC-*`, couplages transverses, graphe de décisions — **par liens**, sans recopie. |
| [Feuille de route des décisions](decision-roadmap.md) | Pour chaque `DEC-*` : prérequis, protocole associé, preuves attendues, **ordre recommandé** des arbitrages en Phase 2. |
| [Préparation à la Phase 2](phase-2-readiness.md) | Pour chaque `DEC-*` : état de préparation (doc / protocole / banc / composants / mesurabilité) — **critère objectif** de démarrage de la Phase 2. |
| [Spécification consolidée de Phase 1 (draft)](consolidated-specification.md) | Assemble l'architecture **actée** (ADR 0000-0012) + les contraintes **ouvertes** (DEC, marquées provisoires) + les interfaces **candidates** + les critères d'acceptation. **Draft, non promue rc.1.** |
| [Hors périmètre](out-of-scope.md) | Ce que le Lot 9 **ne fait pas** (arbitrage, ADR, mesure, gel). |

Mises à jour **transverses** (là où les documents vivent déjà, sans duplication) :
[registre des décisions](../decisions-register.md), [plan des ADR](../adr-plan.md),
[registre des risques](../risk-register.md), [matrice des exigences](../requirements-matrix.md),
[sourcing & BOM](../sourcing-and-bom.md).

## Ce que le Lot 9 confirme

- **17 `DEC-*` toutes Ouvertes / Proposé** — aucune arbitrée (cf. [registre](../decisions-register.md)).
- **Aucune ADR 0013+ créée** — la règle « ADR *après* mesures reproductibles »
  ([cadre de validation](../validation-framework.md) §3) est respectée : aucune
  mesure n'a été exécutée en Phase 1.
- Le passage à la Phase 2 est **gouverné** : chaque `DEC-*` renvoie à son
  **protocole** et à son **état de préparation**.

## Renvois

- [Revue d'architecture intermédiaire L1–L4](../architecture-review-l1-l4.md) — modèle de la synthèse, étendu ici à L1–L8.
- [Ordre d'exécution & critères de sortie](../execution.md) — les critères de sortie décrivent les **campagnes de mesure** (Phase 2), non satisfaites par ce lot.
- [Cadre de validation & modèle de preuves](../validation-framework.md).

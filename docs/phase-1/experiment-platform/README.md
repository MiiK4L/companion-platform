<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Socle expérimental de référence

> **Statut : Proposé — cadre commun révisable.** Ce socle est l'**infrastructure
> partagée** de **toutes** les campagnes de mesure de la Phase 1 (et au-delà). Ce
> **n'est ni une DEC ni une ADR** : c'est un **complément** méthodologique et
> outillé au cadre existant, **jamais une seconde source de vérité**. **Aucune
> mesure n'est exécutée ici, aucun pilote d'instrument réel, aucun instrument
> acheté/figé, aucune logique spécifique à une campagne.**

## Objectif & périmètre

Garantir que **toute campagne** (L1 → L8 et suivantes) produise des résultats
**reproductibles, tracés et comparables** sur une base identique. Le socle définit :

- les **capacités minimales** attendues des instruments et les règles
  d'**équivalence** et de **traçabilité** ([reference-bench](reference-bench.md)) ;
- le **modèle de données** (formats, schémas, métadonnées), indépendant de
  l'implémentation ([measurement-data-model](measurement-data-model.md)) ;
- l'**outillage commun** (orchestration / acquisition / analyse / reporting),
  avec **architecture plugin** et **mode simulation** ([tooling](tooling.md)) ;
- le **cycle d'une campagne** ([campaign-workflow](campaign-workflow.md)) ;
- l'**architecture** d'ensemble ([architecture](architecture.md)) et le
  [hors périmètre](out-of-scope.md).

Il **décrit** des interfaces, conventions, responsabilités, formats et flux — **pas**
de protocoles, de résultats de mesure, ni de choix d'instruments/composants.

## Relations avec le reste du projet (documents faisant autorité)

> Ce socle **ne redéfinit rien**. En cas de doute, les documents suivants **font
> autorité** ; le socle s'y conforme et y **renvoie**.

| Sujet | Autorité (source de vérité) | Rôle du socle |
|-------|-----------------------------|---------------|
| Modèle de preuves, exploratoire vs reproductible, `n_dut`/`n_runs`/`n_campaigns`, stockage/nommage/SHA-256 des données brutes | [validation-framework](../validation-framework.md) | **Instancie** et **outille** ces règles (ne les recopie pas). |
| Modèle de protocole de test | [test-protocol-template](../templates/test-protocol-template.md) | Fournit l'**environnement d'exécution**, pas les protocoles. |
| Modèle de rapport de mesure | [measurement-report-template](../templates/measurement-report-template.md) | **Étend** ce modèle (champs de setup/traçabilité), sans le remplacer. |
| Protocoles de chaque lot | `docs/phase-1/lot-*/protocols/` | **Exécutés** par le socle ; jamais dupliqués ici. |
| Questions de décision & ADR | [decisions-register](../decisions-register.md), [adr-plan](../adr-plan.md) | Le socle **alimente** les mesures ; il ne **crée** ni DEC ni ADR. |
| Convention de valeurs `[DS]/[C]/[H]/[P]/[BL]/[M]` | lots documentaires (ex. [lot-2](../lot-2/README.md)) | `[M]` = **run réel individuellement qualifié** (promotion contrôlée), **pas** une preuve reproductible ; l'agrégat citable par une ADR est un `evidence_bundle` **futur**. Jamais `[M]` en simulation. |
| Sourcing & maturité | [sourcing-and-bom](../sourcing-and-bom.md) | Le socle ne **retient** aucun composant/instrument. |

## Garde-fou central — la simulation n'est pas une preuve

> **Les campagnes simulées valident uniquement l'infrastructure d'outillage. Elles
> ne produisent aucune donnée `[M]`, n'alimentent aucune ADR et ne peuvent
> satisfaire aucun critère expérimental.** Voir [architecture](architecture.md) et
> [hors périmètre](out-of-scope.md) ; règle **encodée dans l'outil** (un artefact
> de nature *simulée* ne peut jamais être marqué *mesuré*).

## Renvois

- [Architecture du socle](architecture.md) · [Banc de référence](reference-bench.md)
- [Modèle de données](measurement-data-model.md) · [Outillage](tooling.md) · [Cycle d'une campagne](campaign-workflow.md)
- [Cadre de validation & modèle de preuves](../validation-framework.md)

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Cycle d'une campagne expérimentale

> **Statut : Proposé.** Décrit le **flux**, le **cycle de vie de la preuve** et
> l'**historique append-only**. Renvoie aux protocoles (**autorité**) sans les
> dupliquer.

## Description / implémentation / résultat

```text
Description   -> campaign-definition.json (question, verdict_rule, protocole, DUT vise)
Implementation-> execution-context.json + build_manifest + baseline-record.json
Resultat      -> acquisition-manifest + series + analysis + evidence-events + report
```

Reliés par identifiants et hashes ; le **résultat est immuable**.

## Étapes

```text
1. Definition ─▶ 2. Acquisition ─▶ 3. Analyse ─▶ 4. Verdict ─▶ 5. Revue ─▶ 6. Promotion
   (baselinee)     (RAW reel /       (immuable)    (lie regle    (REVIEWED)   (-> M, controlee)
                    S simule)                       + analyse)
        └────────────────── historique append-only (evidence-events/) ──────────────────┘
```

Chaque étape ajoute un **événement immuable** chaîné (`previous_event_sha256`).
L'`acquisition-manifest` est immuable ; `evidence-state` / `archive-index` sont
dérivés.

## Cycle de vie de la preuve

| État | Origine | Passage |
|------|---------|---------|
| `RAW` | acquisition **réelle** | `mark_reviewed` (identité de revue) → `REVIEWED` |
| `REVIEWED` | revue tracée | `promote_to_measured` (contrôlée) → `M` |
| `M` | promotion | **run VERROUILLÉ** (toute correction = nouveau run) |
| `S` | acquisition **simulée** | terminal (jamais `M`) |

**Revue** (obligatoire, run réel) : `reviewer`, `reviewed_at`, `review_reason`,
`review_checklist`. **Promotion** : `promoted_by`, `promoted_at`,
`promotion_reason`. Ces identités sont **consignées dans l'événement**.

**`promote_to_measured` vérifie** : acquisition réelle · statut `REVIEWED` ·
**baseline-record archivée et approuvée** · métadonnées complètes · **build
reproductible** (git propre, ou diff archivé+hashé+justifié) · **intégrité totale**
de l'archive (`verify_run`) · **analyse archivée** et **liée au verdict**
(`analysis_result_sha256`) · **verdict décisif** (`PASS`/`FAIL`).

## Verdict (états et liaison)

`PASS` / `FAIL` / `INCONCLUSIVE` (ambigu) / `INVALID` (banc/protocole/DUT/exécution)
/ `NOT_RUN`. Un résultat ambigu **n'est pas** converti en succès/échec. Le verdict
est **relié** à `verdict_rule`, à l'analyse (`analysis_result_sha256`) et à l'outil
d'analyse (`analysis_tool`/version), avec `decided_by`/`decided_at`.

## Portée de `[M]`

> `M` = **run réel individuellement qualifié**, **pas** une preuve reproductible
> suffisante pour arbitrer une DEC.

L'agrégat reproductible (`n_campaigns` indépendantes, DUT, répétitions,
statistiques, comparaison des alternatives) est un niveau **supérieur futur**
(`evidence_bundle`), **seul** citable par une ADR. Voir
[modèle de données](measurement-data-model.md) et
[cadre §4](../validation-framework.md).

## Réel vs simulé

> **Garde-fou.** **Une campagne simulée valide uniquement l'infrastructure
> d'outillage. Elle ne produit aucune donnée `[M]`, n'alimente aucune ADR et ne
> peut satisfaire aucun critère expérimental.** Son verdict est un **verdict
> d'outillage**.

## Traçabilité obligatoire (campagne réelle)

Via `execution-context.json` (cf. [reference-bench §5](reference-bench.md)) :
instrument réel, configuration, **build_manifest**, DUT + révision, fixture,
conditions, étalonnage. L'exécution **refuse** un run réel incomplet
(`N/A` explicite sinon).

## Reproductibilité

Reproductibilité **logicielle** (mêmes entrées → mêmes artefacts, hors volatils)
outillée et vérifiée en CI ; reproductibilité **méthodologique**
(`n_campaigns ≥ 2` indépendantes) relève du [cadre §4](../validation-framework.md)
et de l'`evidence_bundle` futur.

## Renvois

- [Vue d'ensemble](README.md) · [Architecture](architecture.md) · [Modèle de données](measurement-data-model.md)
- [Cadre de validation](../validation-framework.md)

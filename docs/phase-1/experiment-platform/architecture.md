<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Architecture du socle expérimental

> **Statut : Proposé.** Vue d'ensemble des **couches**, de l'**architecture
> plugin**, du **mode simulation** et du **déterminisme**. Décrit des interfaces
> et des responsabilités ; **aucune** logique spécifique à une campagne.

## Séparation stricte des responsabilités

Quatre couches **indépendantes**, communiquant par des **données** (formats
documentés dans [measurement-data-model](measurement-data-model.md)), jamais par
des détails d'implémentation :

```text
┌──────────────┐   définition &   ┌──────────────┐
│ Orchestration│── cycle de vie ──▶│  Acquisition │──▶ séries de mesure (données)
│  (campagne)  │                   │  (drivers)   │       (réelles OU simulées)
└──────┬───────┘                   └──────────────┘
       │                                  ▲
       │                          interface plugin
       │                                  │
       ▼                           ┌──────────────┐
┌──────────────┐   manifeste +     │   Analyse    │──▶ grandeurs, incertitude,
│  Reporting   │◀── métadonnées ───│ (traitement) │      validation de schéma
│ (rapport md) │                   └──────────────┘
└──────────────┘
```

| Couche | Responsabilité | Ne fait PAS |
|--------|----------------|-------------|
| **Orchestration** | Cycle de vie d'une campagne : définition, identifiants, exécution, archivage, reproduction. | Ne parle pas à un instrument ; n'analyse pas. |
| **Acquisition** | Obtenir des séries via un **driver** (réel ou simulé) derrière une **interface** unique. | N'interprète pas les données ; ne génère pas de rapport. |
| **Analyse** | Traitement, incertitude, **validation de schéma** des données. | Ne pilote pas d'instrument. |
| **Reporting** | Rendu du rapport à partir du manifeste + métadonnées (extension du template). | Ne mesure pas ; ne décide pas d'un statut de preuve. |

## Registre STATIQUE d'adaptateurs d'instruments

Chaque instrument (oscilloscope, alimentation, analyseur logique…) est un
**adaptateur** implémentant l'**interface de driver** et s'enregistrant auprès du
cœur via `@register_driver`. Le registre est **statique et assumé** (pas de
découverte dynamique) :

- le cœur ne connaît que l'**interface** (contrat), jamais un instrument concret ;
- **ajouter** un adaptateur suppose de l'**importer** dans le paquet
  `acquisition` (pour qu'il s'enregistre) — mais **ne modifie ni** l'orchestration,
  **ni** l'analyse, **ni** le reporting, **ni** la CI ;
- un mécanisme de **découverte explicite** (`entry_points`, répertoire de plugins)
  pourra être ajouté **plus tard** ; il n'est **pas** présenté ici comme existant ;
- **aucun pilote réel** n'est fourni à ce stade — seulement l'interface, des
  **stubs** et le **driver de simulation**. Chaque driver **déclare
  explicitement** sa `nature` (`measured` / `simulated`) — aucun défaut.

## Mode simulation & cycle de vie de la preuve

Un **driver de simulation** produit des **captures factices déterministes**
(dérivées de `campaign_definition_id`) et permet d'exécuter une **campagne entière
sans matériel**, pour valider orchestration, formats, schémas, rapports et
**reproductibilité logicielle**.

Une acquisition **réelle** sort en statut **`RAW`** ; elle ne devient une **preuve
`M`** que par une **promotion explicite et contrôlée** (revue, baseline approuvée,
métadonnées complètes, intégrité, analyse, verdict décisif). Une acquisition
**simulée** est terminale en **`S`**.

> **Garde-fou (encodé dans l'outil).** **Les campagnes simulées valident
> uniquement l'infrastructure d'outillage. Elles ne produisent aucune donnée
> `[M]`, n'alimentent aucune ADR et ne peuvent satisfaire aucun critère
> expérimental.** Un artefact `simulated` (`S`) **ne peut jamais** être promu en
> `M` ; une donnée réelle non qualifiée reste `RAW` et **ne peut pas** être promue
> trop tôt. Voir [modèle de données](measurement-data-model.md) et
> [cycle d'une campagne](campaign-workflow.md).

## Historique append-only & intégrité

Le **résultat est immuable**. Les transitions (`RAW → REVIEWED → M`) et les
verdicts sont des **événements append-only** (`evidence-events/`), **chaînés** par
`previous_event_sha256`. L'`acquisition-manifest.json` est immuable ;
`evidence-state.json` et `archive-index.json` sont des **vues dérivées**. Une fois
`M` atteint, le run est **verrouillé** (toute correction = nouveau run).
`verify_run` recalcule **toutes** les empreintes, valide **tous** les schémas et
vérifie la **chaîne d'événements** — c'est l'autorité d'intégrité.

Les **artefacts bruts d'acquisition** (`raw/`, source de vérité, tout type de
mesure) et `capture.json` (paramètres + traçabilité brut → série normalisée) sont
**immuables** et couverts par l'intégrité. L'axe **`variant_id`** (générique, dont
le socle ignore la sémantique) trace des variantes contrôlées d'un même
`experiment_id` ; les identifiants stables laissent la **porte ouverte** à un
`evidence_bundle` futur (agrégat reproductible citable par une ADR).

## Déterminisme

**Mêmes entrées → mêmes artefacts**, afin que la reproductibilité logicielle soit
**vérifiable en CI**. Deux notions distinctes d'identifiant (voir
[measurement-data-model](measurement-data-model.md)) :

- **`campaign_definition_id`** — **déterministe**, dérivé (hash) de la définition
  (protocole, configuration, DUT, paramètres) ;
- **`run_id`** — **unique** à chaque exécution réelle.

Les champs **volatils** (`run_id`, horodatages) sont **explicitement isolés** et
**exclus** de la comparaison lors du test de déterminisme. Toute sérialisation est
**canonique** (clés triées, séparateurs fixes) pour être hachable de façon stable.

## Renvois

- [Vue d'ensemble](README.md) · [Modèle de données](measurement-data-model.md) · [Outillage](tooling.md)
- [Cadre de validation](../validation-framework.md)

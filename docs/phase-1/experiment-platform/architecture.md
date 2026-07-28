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

## Architecture plugin (adaptateurs d'instruments)

Chaque instrument (oscilloscope, alimentation, analyseur logique…) est un
**adaptateur** implémentant l'**interface de driver** et s'**enregistrant** auprès
du cœur, **sans le modifier** :

- le cœur ne connaît que l'**interface** (contrat), jamais un instrument concret ;
- **ajouter** un adaptateur n'exige **aucune** modification de l'orchestration, de
  l'analyse, du reporting **ni de la CI** ;
- **aucun pilote réel** n'est fourni à ce stade — seulement l'interface, des
  **stubs** et le **driver de simulation**.

## Mode simulation

Un **driver de simulation** produit des **captures factices déterministes**
(dérivées de l'identifiant de définition de campagne). Il permet d'exécuter une
**campagne entière sans aucun matériel**, pour valider orchestration, formats,
schémas, rapports et **reproductibilité logicielle**.

> **Garde-fou (règle du socle, encodée dans l'outil).** **Les campagnes simulées
> valident uniquement l'infrastructure d'outillage. Elles ne produisent aucune
> donnée `[M]`, n'alimentent aucune ADR et ne peuvent satisfaire aucun critère
> expérimental.** Tout artefact porte une **nature** (`simulated` / `measured`) ;
> un artefact `simulated` **ne peut jamais** être marqué `measured`, et le
> manifeste l'affiche explicitement.

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

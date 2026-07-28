<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Outillage commun (`tools/measurement/`)

> **Statut : Proposé.** Décrit l'**organisation**, les **interfaces** et les
> **conventions** de l'outillage. Le code de référence est sous
> `tools/measurement/`. **Aucun pilote d'instrument réel** ici : interfaces,
> **stubs** et **driver de simulation** uniquement.

## Organisation (couches séparées)

```text
tools/measurement/
  measurement/
    common/         # sérialisation canonique, hachage SHA-256, identifiants
    orchestration/  # cycle de vie d'une campagne (création, run, archivage, reproduction)
    acquisition/    # interface de driver + registre plugin + driver de simulation
    analysis/       # validation de schéma, traitement, incertitude
    reporting/      # rendu du rapport (extension du template)
    cli.py          # point d'entrée en ligne de commande
  schemas/          # contrats JSON Schema (source de vérité des formats)
  golden/           # jeu de données de référence (versionné)
  tests/            # tests host (stdlib), sans matériel
```

La [séparation orchestration / acquisition / analyse / reporting](architecture.md)
est **stricte** : une couche ne connaît que des **données** et des **interfaces**.

## Interface de driver (architecture plugin)

Chaque instrument est un **adaptateur** implémentant l'interface d'acquisition et
s'**enregistrant** auprès du registre, **sans modifier le cœur** :

- contrat minimal : « fournir des **séries** conformes au schéma, à partir d'une
  **configuration** » ;
- le **driver de simulation** implémente ce contrat avec des **captures factices
  déterministes** (seed = `campaign_definition_id`) ;
- **ajouter** un adaptateur réel plus tard **n'exige aucune** modification de
  l'orchestration, de l'analyse, du reporting **ni de la CI**.

## Déterminisme (règles d'implémentation)

- **Sérialisation canonique** (clés triées, séparateurs fixes) pour tout ce qui
  est haché ;
- valeurs simulées **dérivées du hash** (pas d'aléa non contrôlé) ;
- **champs volatils** (`run_id`, horodatages) **injectables** et **isolés** des
  artefacts comparés ;
- **mêmes entrées → mêmes artefacts** (hors volatils) — vérifié en CI.

## Choix d'outillage interne (non structurants)

> Python, `pytest`/`unittest`, le linter et le formateur sont des **choix
> d'outillage interne**, **pas** l'architecture des campagnes. Le **cœur des
> formats** ([measurement-data-model](measurement-data-model.md)) **ne dépend pas**
> du framework de test : il repose sur les **JSON Schema** et les **CSV**, lisibles
> par d'autres outils. Les tests **importent** le paquet, jamais l'inverse.

- **Langage** : Python (cohérent avec `tools/` existant).
- **Tests** : bibliothèque standard (`unittest`) — aucune dépendance de test dans
  le cœur.
- **Lint/format** : outil dédié épinglé, exécuté en CI (voir workflow
  `measurement-tooling`).

## Renvois

- [Architecture](architecture.md) · [Modèle de données](measurement-data-model.md) · [Cycle d'une campagne](campaign-workflow.md)

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
    model.py        # vocabulaires, sûreté des noms, complétude des métadonnées réelles
    common/         # sérialisation canonique, hachage SHA-256, identifiants
    orchestration/  # cycle de vie : run, revue, verdict, analyse, promotion, vérification
    acquisition/    # interface de driver + registre statique + driver de simulation
    analysis/       # validation de schéma (profil documenté)
    reporting/      # rendu du rapport (extension du template)
    cli.py          # point d'entrée en ligne de commande
  schemas/          # contrats JSON Schema (source de vérité des formats)
  golden/           # jeu de données de référence (versionné)
  tests/            # tests host (stdlib) + parité jsonschema (CI), sans matériel
```

La [séparation orchestration / acquisition / analyse / reporting](architecture.md)
est **stricte** : une couche ne connaît que des **données** et des **interfaces**.

## Interface de driver (registre STATIQUE)

Chaque instrument est un **adaptateur** implémentant l'interface d'acquisition et
s'enregistrant via `@register_driver`. Registre **statique et assumé** (pas de
découverte dynamique) :

- contrat minimal : « fournir des **séries** conformes au schéma, à partir d'une
  **configuration** » ; chaque driver **déclare explicitement** sa `nature`
  (`measured`/`simulated`) — refusé sinon à l'enregistrement ;
- le **driver de simulation** implémente ce contrat avec des **captures factices
  déterministes** (seed = `campaign_definition_id`) ;
- **ajouter** un adaptateur suppose de l'**importer** dans le paquet `acquisition`
  (effet de bord d'enregistrement) — mais **ne modifie ni** l'orchestration, **ni**
  l'analyse, **ni** le reporting, **ni** la CI. Une **découverte explicite**
  (`entry_points`, répertoire de plugins) reste une évolution **future**, non
  présentée ici comme existante.
- un driver peut fournir des **artefacts bruts d'acquisition** via `capture()`
  (optionnel, générique — indépendant du type de mesure) : archivés sous
  `raw/<group>/…` (source de vérité), avec `capture.json` (paramètres +
  traçabilité brut → série). Les bruts sont fournis **par chemin** (copiés
  **octet par octet**, tout format — texte, VCD, binaire, image) ou en contenu
  inline ; le socle **ne présume rien** de leur contenu.
- **`manual-import`** est le **driver manuel de référence** : il importe une
  acquisition faite à la main depuis un **répertoire d'import éphémère** (fourni
  au **run** via `--import-dir` / `acquisition_overrides`, **jamais archivé** ni
  inclus dans `campaign_definition_id`). Il découvre bruts + descripteur de
  capture (`import-descriptor.schema.json`) + séries normalisées, **sans aucune
  logique métier** (ni verdict, ni `[BL]`, ni SPI/CX-Bus). Son API **définit
  l'interface d'acquisition** : un futur driver **automatique** produira les
  **mêmes artefacts** sans toucher schémas, `verify_run`, rapports ni analyse.

## Déterminisme (règles d'implémentation)

- **Sérialisation canonique** (clés triées, séparateurs fixes) pour tout ce qui
  est haché ;
- valeurs simulées **dérivées du hash** (pas d'aléa non contrôlé) ;
- **champs volatils** (`run_id`, horodatages) **injectables** et **isolés** des
  artefacts comparés ;
- **mêmes entrées → mêmes artefacts** (hors volatils) — vérifié en CI.

## Validation de schéma : profil maison + validateur standard

- Le **validateur maison** (`analysis/schema.py`) couvre un **profil documenté**
  de mots-clés : `type`, `enum`, `required`, `properties`, `additionalProperties`,
  `items`, `pattern`, `minLength`, `minItems` (annotations ignorées : `$schema`,
  `title`, `description`).
- La **CI** emploie **en plus** un **validateur standard** (`jsonschema`,
  draft 2020-12) pour : (1) vérifier que les **schémas eux-mêmes** sont valides ;
  (2) vérifier les **golden datasets** ; (3) garantir la **parité** (même verdict
  accept/reject sur des exemples curés).
- Un test **signale tout mot-clé hors profil** (`unsupported_keywords`) : si un
  schéma futur utilise un mot-clé que le validateur maison **ignorerait**, la CI
  échoue — pas de faux sentiment de validation.

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

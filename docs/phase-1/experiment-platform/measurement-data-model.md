<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Modèle de données de mesure

> **Statut : Proposé.** Décrit **formats, schémas et métadonnées** — **indépendamment
> de l'implémentation**. Les contrats sont des fichiers **JSON Schema**
> (`tools/measurement/schemas/`), lisibles et validables par tout outil. L'outillage
> Python n'en est qu'un consommateur ; le modèle **ne dépend d'aucun framework de test**.

## Trois niveaux distincts (description / implémentation / résultat)

Le modèle **sépare** trois choses reliées par identifiants et hashes, jamais
confondues dans un seul objet (voir [campaign-workflow](campaign-workflow.md)) :

| Niveau | Objet | Contenu | Propriété |
|--------|-------|---------|-----------|
| **Description** | `campaign-definition.json` | question, observation attendue, `verdict_rule`, protocole, DUT visé, acquisition | lisible, contrôlable humainement, **baselinable** |
| **Implémentation** | `execution-context.json` (dont `build_manifest`) | banc, instrument réel, build, fixture, conditions, étalonnage | peut évoluer |
| **Résultat** | `manifest.json`, `series/*.csv`, `analysis-result.json`, `verdict.json`, `report.md` | données, analyse, verdict, rapport | **immuable et traçable** |

## Identifiants (déterministe / unique / humain)

| Identifiant | Nature | Rôle |
|-------------|--------|------|
| **`experiment_id`** | **humain, traçable** (`^[A-Za-z0-9][A-Za-z0-9_-]*$`, ex. `EXP-L2-INRUSH-001`) | relie protocole ↔ définition ↔ script/fixture ↔ dataset ↔ rapport |
| **`campaign_definition_id`** | **déterministe** (SHA-256 de la définition canonique) | identifie la **définition** ; seed de la simulation |
| **`run_id`** | **unique** par exécution (volatil) | identifie un **run** |

`run_id` et `experiment_id` sont validés (format sûr, sans séparateur de chemin).
`run_id` et les horodatages sont **volatils** : isolés et exclus des comparaisons
de déterminisme.

## Nature d'acquisition vs statut de preuve

Deux axes **distincts** — une acquisition réelle **ne devient pas** automatiquement
une preuve :

- **`acquisition_nature`** : `measured` (instrument réel) ou `simulated`. Déclarée
  **explicitement** par chaque driver (aucun défaut).
- **`evidence_status`** : cycle de vie de la preuve.

| `evidence_status` | Signification | Transition |
|-------------------|---------------|-----------|
| `RAW` | acquisition réelle **brute** (sortie par défaut du réel) | → `REVIEWED` par revue |
| `REVIEWED` | relu/validé par le cadre | → `M` par promotion contrôlée |
| `M` | **preuve mesurée** qualifiée | opération explicite `promote_to_measured` |
| `S` | **simulé** (outillage) — **terminal** | jamais promu |

> **Garde-fou (encodé)** : une acquisition `simulated` sort en `S` et **ne peut
> jamais** devenir `M`. Une acquisition `measured` sort en `RAW` ; le passage à `M`
> vérifie : campagne réelle, revue faite, **baseline approuvée**, métadonnées
> complètes, **artefacts intègres** (SHA-256), **analyse exécutée**, **verdict
> décisif** (`PASS`/`FAIL`).

## Verdict (un résultat ambigu n'est pas forcé)

`verdict` ∈ `{PASS, FAIL, INCONCLUSIVE, INVALID, NOT_RUN}`, avec `verdict_reason` :

- `PASS` critères satisfaits · `FAIL` non satisfaits ·
- `INCONCLUSIVE` données insuffisantes/ambiguës ·
- `INVALID` campagne invalide (banc/protocole/DUT/exécution) ·
- `NOT_RUN` définie mais non exécutée (état d'un run avant analyse).

> Une campagne **simulée** peut **tester la mécanique** de ces états, mais son
> verdict reste un **verdict d'outillage** et **n'alimente aucune DEC**.

## Build manifest (identification exacte)

`build_manifest` (dans `execution-context.json`) identifie **précisément** le
firmware/logiciel réellement utilisé — une simple chaîne libre ne suffit pas :

```text
git_commit · git_dirty · toolchain · esp_idf_version · target
build_configuration_hash · artifact_sha256 · tooling_version · measurement_tooling_commit
```

Le rapport **pointe vers l'artefact exact** (`artifact_sha256`), pas vers un nom.
Pour une campagne **logicielle** (parser, runtime), le même principe s'applique.
Champs non applicables : **`N/A` explicite**, jamais objet/chaîne vide.

## Contrats (JSON Schema) & profil du validateur

Schémas : `campaign-definition`, `execution-context`, `build-manifest`,
`run-manifest`, `measurement-series`, `verdict`. Le **validateur maison** ne
couvre qu'un **profil documenté** de mots-clés (`type`, `enum`, `required`,
`properties`, `additionalProperties`, `items`, `pattern`). La **CI** emploie en
plus un **validateur standard** (`jsonschema`) pour vérifier les schémas
eux-mêmes, les golden datasets, et la **parité** ; un test signale tout mot-clé
hors profil (cf. [tooling](tooling.md), [analysis/schema.py]).

## Archive autosuffisante

```text
results/<campaign_definition_id>/<run_id>/
  campaign-definition.json   # copie EXACTE de la definition (SHA-256 au manifeste)
  execution-context.json     # contexte d'execution (SHA-256 au manifeste)
  manifest.json              # resultat : references (hashes), statut, verdict, artefacts
  verdict.json               # verdict (miroir, stade resultat)
  analysis-result.json       # (si analyse executee)
  series/*.csv               # donnees (module csv standard ; nom sur)
  report.md                  # rapport structure (extension du template)
```

La reproduction **ne dépend pas** d'un fichier externe référencé seulement par
chemin : la définition et le contexte exacts sont **copiés** et **hashés** dans le
run. `verify_run` recalcule les SHA-256 (définition, contexte, séries).

## Renvois

- [Architecture](architecture.md) · [Outillage](tooling.md) · [Cycle d'une campagne](campaign-workflow.md)
- [Cadre de validation §1/§3](../validation-framework.md) · [Modèle de rapport](../templates/measurement-report-template.md)

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Modèle de données de mesure

> **Statut : Proposé.** Décrit les **formats, schémas et métadonnées** des
> campagnes — **indépendamment de toute implémentation**. Les schémas canoniques
> sont des fichiers **JSON Schema** (`tools/measurement/schemas/`), **lisibles et
> validables par n'importe quel outil**. L'outillage Python n'est **qu'un**
> consommateur de ces schémas ; le modèle **ne dépend pas** du framework de test.

## Principe : formats indépendants de l'implémentation

- Les **contrats** de données sont les fichiers **`*.schema.json`** (standard
  ouvert), pas le code Python.
- Les **séries de mesure** sont des **CSV** (colonnes + unités documentées),
  exploitables par tout tableur ou script.
- `pytest`, le linter et le formateur sont des **choix d'outillage interne**
  ([tooling](tooling.md)) et **n'entrent pas** dans le modèle de données.

## Identifiants : définition (déterministe) vs exécution (unique)

Deux identifiants **distincts**, pour concilier « **mêmes entrées → mêmes
artefacts** » et « **run unique** » :

| Identifiant | Nature | Dérivation | Rôle |
|-------------|--------|-----------|------|
| **`campaign_definition_id`** | **Déterministe** | Hash (SHA-256) de la **définition canonique** : `protocol_ref`, `dec`, `dut`, `parameters`, exigences d'acquisition — **hors** champs volatils | Identifie la **définition** ; deux définitions identiques → même id ; **seed** de la simulation |
| **`run_id`** | **Unique** | Généré à chaque **exécution** réelle (non reproductible) | Identifie un **run** précis |

> **Champs volatils** (`run_id`, horodatages `generated_at`) : **isolés** et
> **exclus** de toute comparaison de déterminisme. La sérialisation est
> **canonique** (clés triées) pour un hachage stable.

## Objets & schémas

### 1. Définition de campagne — `campaign-definition.schema.json`

Entrée d'une campagne. Champs : `protocol_ref` (renvoi au protocole du lot, **autorité**),
`dec` (`DEC-*` alimentée), `dut` (`description`, `hardware_revision`),
`parameters`, `acquisition` (`driver`, `config`). **Aucun** champ volatil.

### 2. Manifeste de run — `run-manifest.schema.json`

Sortie d'une exécution. Champs clés :

- `campaign_definition_id`, `run_id`, `generated_at` *(volatils : `run_id`, `generated_at`)* ;
- **`nature`** : `measured` **ou** `simulated` — **garde-fou** ;
- **`data_status`** : `M` (mesuré) **uniquement si** `nature = measured` ; sinon
  `S` (simulé). **Un artefact `simulated` ne peut jamais porter `M`.**
- **Traçabilité** (cf. [reference-bench §5](reference-bench.md)) : `instrument`
  (`used` marque/modèle/firmware, `configuration`), `firmware_under_test`,
  `dut_hardware_revision`, `conditions` ;
- `artifacts` : liste `{path, sha256}` (empreintes des données brutes).

> **Règle encodée** : `nature = simulated` ⇒ `data_status = S` et **interdiction**
> de marquer `M`. Une simulation **ne satisfait aucun critère expérimental** et
> **n'alimente aucune ADR**.

### 3. Série de mesure — CSV (`measurement-series.schema.json` décrit les colonnes)

CSV avec **en-tête** ; chaque colonne a un **nom** et une **unité** documentés. Le
schéma associé décrit les **colonnes obligatoires** et leurs unités. Métadonnées
de campagne (id, nature…) portées par le **manifeste**, pas par le CSV.

### 4. Rapport de mesure

**Extension** du [modèle de rapport](../templates/measurement-report-template.md)
(renvoi, pas copie) : le rapport ajoute l'**ID de setup**, les **versions
d'outils**, le **manifeste + hachages** et les **métadonnées de traçabilité**. Le
**statut de preuve** (Proposé → Accepté) reste régi par le
[cadre de validation §3](../validation-framework.md) — **jamais** par le socle.

## Convention de stockage & nommage (instanciation)

Instancie [validation-framework §1](../validation-framework.md) (renvoi) :

```text
results/<campaign_definition_id>/<run_id>/
  manifest.json           # manifeste de run (schéma ci-dessus)
  series/*.csv            # séries de mesure
  report.md              # rapport (extension du template)
```

- **Nommage** : `<id-protocole>_<date>_<variante>.csv` (règle du cadre) sous
  `series/`. Les **hachages SHA-256** de chaque fichier sont consignés au manifeste.
- **Archivage** : l'arborescence d'un run est **auto-suffisante** (rejouable).

## Renvois

- [Architecture](architecture.md) · [Outillage](tooling.md) · [Banc de référence](reference-bench.md)
- [Cadre de validation §1/§3](../validation-framework.md) · [Modèle de rapport](../templates/measurement-report-template.md)

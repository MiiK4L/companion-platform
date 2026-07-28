<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Modèle de données de mesure

> **Statut : Proposé.** Décrit **formats, schémas et métadonnées** —
> indépendamment de l'implémentation. Les contrats sont des **JSON Schema**
> (`tools/measurement/schemas/`). Le résultat est **immuable et vérifiable** :
> transitions et verdicts vivent dans un **historique append-only**.

## Trois niveaux distincts (description / implémentation / résultat)

| Niveau | Objet(s) | Propriété |
|--------|----------|-----------|
| **Description** | `campaign-definition.json` (question, `verdict_rule`, protocole, DUT visé, acquisition) | lisible, contrôlable, **baselinable** |
| **Implémentation** | `execution-context.json` (dont `build_manifest`), `baseline-record.json` | peut évoluer ; **identifie exactement** le build |
| **Résultat** | `acquisition-manifest.json`, `series/*.csv`, `analysis-result.json`, `evidence-events/*`, `evidence-state.json`, `report.md` | **immuable et traçable** |

Reliés par identifiants et hashes, jamais fondus en un seul objet.

## Identifiants

| Identifiant | Nature | Rôle |
|-------------|--------|------|
| **`experiment_id`** | humain (`EXP-…`) | relie protocole ↔ définition ↔ script/fixture ↔ dataset ↔ rapport |
| **`campaign_definition_id`** | déterministe (SHA-256 de la définition) | identité de la définition ; seed de la simulation |
| **`run_id`** | unique (volatil) | identité d'un run |

## Nature d'acquisition vs statut de preuve

- **`acquisition_nature`** : `measured` / `simulated` — déclarée **explicitement** par le driver.
- **`evidence_status`** : `RAW` → `REVIEWED` → `M` (réel) ; `S` (simulé, terminal).

Une acquisition **réelle** sort en `RAW`. Le passage à `M` est une **promotion
contrôlée** (voir [cycle d'une campagne](campaign-workflow.md)). **Simulé = `S`,
jamais `M`.**

## Historique append-only & immuabilité

Les transitions et verdicts sont des **événements immuables** :

```text
evidence-events/
  0001-acquisition.json
  0002-verdict.json
  0003-review.json
  0004-promotion.json
```

Chaque événement (`evidence-event.schema.json`) porte : `event_id`, `event_type`,
`from_status`, `to_status`, `actor`, `timestamp` (ISO 8601), `reason`, `inputs`
(noms + SHA-256), `previous_event_sha256` (**chaînage**), `details` (zone
d'extension typée). L'`acquisition-manifest.json` est **immuable** ;
`evidence-state.json` (statut/verdict/`locked`) et `archive-index.json` (index des
empreintes) sont des **vues dérivées**.

> **Verrou à `M`** : une fois `evidence_status = M`, le run est **verrouillé** —
> plus de verdict, d'analyse, de contexte ni de réécriture. Toute correction
> produit un **nouveau run**.

## Verdict (lié à la règle et à l'analyse)

`verdict` ∈ `{PASS, FAIL, INCONCLUSIVE, INVALID, NOT_RUN}` (ambigu jamais forcé).
L'événement `verdict` relie techniquement la décision : `verdict_rule_ref`,
`analysis_result_sha256` (doit correspondre à `analysis-result.json`),
`analysis_tool`, `analysis_tool_version`, `decided_by`, `decided_at`. Le verdict
peut rester **humain**, mais la décision est **explicite et traçable**.

## Build manifest & build git « sale »

`build_manifest` identifie exactement le build (`git_commit`, `git_dirty`,
`toolchain`, `esp_idf_version`, `target`, `build_configuration_hash`,
`artifact_sha256`, `tooling_version`, `measurement_tooling_commit`). Le rapport
pointe vers `artifact_sha256`. **Promotion** : `git_dirty = false` requis ; si
`true`, le **diff exact** est archivé (`dirty-diff.patch`), **hashé**
(`dirty_diff_sha256`) et **justifié** — sinon le build n'est pas reproductible.

## Baseline archivée

La baseline **réellement appliquée** est copiée (`baseline-record.json`,
`baseline-record.schema.json`) et hashée : `baseline_id`, `protocol_ref`,
`protocol_commit`, `status` (`approved`/`draft`), `approved_by`, `approved_at`,
`resolved_bl_fields` (champs `[BL]` levés). Le statut `approved` **seul** ne
suffit pas : le record doit être présent, valide et intègre pour promouvoir.

## Portée de `[M]` (run qualifié ≠ preuve reproductible)

> **`M` signifie « run réel individuellement qualifié », PAS « preuve
> reproductible suffisante pour arbitrer une DEC ».**

La [reproductibilité](../validation-framework.md) exige plusieurs **campagnes
indépendantes** (`n_dut`, `n_runs`, `n_campaigns` justifiés) et la **comparaison
des alternatives**. Un niveau supérieur **futur** (non implémenté ici) —
`evidence_bundle` / `campaign_set` / `comparison_report` — agrégera plusieurs runs
`M` et vérifiera `n_campaigns`, indépendance, DUT, répétitions, statistiques et
comparaison. **Seul ce bundle reproductible pourra être cité par une ADR.**

## Contrats (JSON Schema) & profil du validateur

Schémas stricts (`additionalProperties: false` + zone `extensions` explicite,
`minLength`, motifs SHA-256 `^[a-f0-9]{64}$`, `DEC-*`, `experiment_id`, ISO 8601,
énumérations). Le **validateur maison** couvre un **profil documenté** (`type`,
`enum`, `required`, `properties`, `additionalProperties`, `items`, `pattern`,
`minLength`, `minItems`) ; la **CI** ajoute un **validateur standard**
(`jsonschema`) pour les schémas, les golden et la **parité**, et signale tout
mot-clé hors profil.

## Archive autosuffisante (pour la preuve)

```text
results/<campaign_definition_id>/<run_id>/
  acquisition-manifest.json   # IMMUABLE (identite + empreintes)
  campaign-definition.json    # IMMUABLE (copie exacte)
  execution-context.json      # IMMUABLE (dont build_manifest)
  baseline-record.json        # IMMUABLE (si reel)
  analysis-result.json        # IMMUABLE (si analyse)
  dirty-diff.patch            # IMMUABLE (si build sale)
  series/*.csv                # IMMUABLE (module csv ; nom sur)
  evidence-events/*.json      # IMMUABLE, append-only, chaine
  archive-index.json          # vue derivee (empreintes de tout l'immuable)
  evidence-state.json         # vue derivee (statut, verdict, locked)
  report.md                   # vue derivee
```

**Artefacts autoritaires** (immuables) : définition, contexte, baseline, séries,
analyse, dirty-diff, **événements append-only**. **Vues dérivées** (régénérables) :
`evidence-state.json`, `archive-index.json`, `report.md`. Ces dernières sont
**reconstruites depuis les artefacts autoritaires** par `rebuild_derived` (CLI
`rebuild-derived`) ; `verify_run` **tolère leur absence** et, si elles sont
présentes, exige qu'elles **correspondent exactement** à leur reconstruction
(l'état dérivé est **rejoué** depuis l'historique, transitions validées).

L'archive est **autosuffisante pour la preuve** : définition, contexte, baseline,
séries, analyse, verdicts et **transitions** sont tous **copiés/hashés** dans le
run. Le Markdown généré (`report.md`) est une **vue**, pas la preuve primaire.
`verify_run` recalcule **toutes** les empreintes, valide **tous** les schémas
(dont les **détails d'événement par type**), vérifie la **chaîne d'événements**,
**rejoue** l'historique (transitions légales) et contrôle la **cohérence des
références** (experiment_id, protocol_ref, baseline, analyse).

## Renvois

- [Architecture](architecture.md) · [Outillage](tooling.md) · [Cycle d'une campagne](campaign-workflow.md)
- [Cadre de validation §1/§3/§4](../validation-framework.md) · [Modèle de rapport](../templates/measurement-report-template.md)

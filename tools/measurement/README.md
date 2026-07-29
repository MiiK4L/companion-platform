<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# `tools/measurement/` — outillage du socle expérimental

Infrastructure **commune** à toutes les campagnes de mesure de la Phase 1 (et
au-delà). Documentation de référence :
[docs/phase-1/experiment-platform/](../../docs/phase-1/experiment-platform/README.md).

> **Aucun pilote d'instrument réel, aucune mesure, aucune logique spécifique à
> une campagne.** Interfaces, stubs et **driver de simulation** uniquement.
> **Garde-fou** : une campagne **simulée** valide l'outillage — elle ne produit
> **aucune donnée `[M]`** et n'alimente aucune ADR.

## Organisation (couches séparées)

```text
measurement/
  common/         serialisation canonique, hachage SHA-256, identifiants
  orchestration/  cycle de vie d'une campagne (id, run, archivage, reproduction)
  acquisition/    interface driver + registre statique + simulation + manual-import
  analysis/       validation de schema
  reporting/      rendu du rapport (extension du template)
  cli.py          point d'entree
schemas/          contrats JSON Schema (source de verite des formats)
golden/           jeu de donnees de reference versionne (tests)
tests/            tests host (bibliotheque standard, sans materiel)
```

## Usage

```bash
cd tools/measurement

# Lister les drivers enregistres
python3 -m measurement.cli drivers

# Executer une campagne (ici simulee) ; --run-id/--generated-at rendent le run
# reproductible (a reserver aux tests / au golden dataset)
python3 -m measurement.cli run \
  --definition golden/campaign-definition.json --out /tmp/run

# Revalider l'integrite complete d'un run (schemas, empreintes, chaine, coherence)
python3 -m measurement.cli verify --run /tmp/run/<def_id>/<run_id>

# Regenerer les vues derivees (etat, index, rapport) depuis l'autoritaire
python3 -m measurement.cli rebuild-derived --run /tmp/run/<def_id>/<run_id>

# Tests (sans dependance externe)
python3 -m unittest discover -s tests -t .

# Lint & format (outillage interne, epingle dans requirements-dev.txt)
pip install -r requirements-dev.txt
ruff check .
ruff format --check .
```

## Conventions

- **Formats** = fichiers `schemas/*.schema.json` (standard ouvert), **indépendants**
  de Python et de tout framework de test.
- **Déterminisme** : mêmes entrées → mêmes artefacts ; `run_id` et horodatages
  sont **volatils** et **isolés** des comparaisons.
- **Plugin** : ajouter un instrument = un adaptateur enregistré via
  `@register_driver`, **sans modifier le cœur ni la CI**.
- **Autoritaire vs dérivé** : artefacts **immuables/autoritaires** (définition,
  contexte, baseline, séries, analyse, dirty-diff, **événements append-only**) vs
  **vues dérivées régénérables** (`evidence-state.json`, `archive-index.json`,
  `report.md`) reconstruites par `rebuild-derived`. Une fois `M` atteint, le run
  est **verrouillé** (correction = nouveau run).

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# ADR-0011 : Documentation as code plutôt que Wiki GitHub

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideur** : Mainteneur du projet (MiiK4L)
- **Expertises consultées** : Expert documentation, expert CI/CD
- **Phase de roadmap** : 0
- **Domaines impactés** : docs, ci
- **Tags** : documentation, mkdocs, github-pages, wiki

## 1. Contexte et problème

La documentation est traitée comme une fonctionnalité de premier plan : aussi
importante que le firmware, obligatoire pour toute fonctionnalité, et
suffisante pour qu'un tiers reprenne le projet. Le cahier des charges évoquait
un Wiki GitHub. Or le Wiki présente un défaut de fond : il **n'est ni versionné
avec le code, ni relu en Pull Request**, ce qui le désynchronise vite du projet.

## 2. Options étudiées

### Option A — Wiki GitHub comme source de la documentation
- **Avantages** : édition rapide en ligne.
- **Inconvénients** : dépôt Git séparé, non relu en PR, se désynchronise du
  code qu'il documente, pas de build/vérification (liens morts, etc.).

### Option B — Docs-as-code : Markdown dans `docs/`, site généré (MkDocs Material), publié sur GitHub Pages
- **Avantages** : versionné **avec** le code, **relu en PR** comme du code,
  build vérifié en CI (`--strict` détecte les liens/nav cassés), navigation et
  recherche pro, repris hors ligne par toute IA/ingénieur.
- **Inconvénients** : nécessite un pipeline de build (déjà en place, Phase 0).

## 3. Décision

On sépare deux choses :

- **Décision acceptée (stable)** : la documentation est **docs-as-code** —
  Markdown dans `docs/`, relue en PR, et **construite en CI avec `mkdocs build
  --strict`** (config, nav, ancres). L'**intégrité des liens** est vérifiée par
  un **contrôle dédié** (`tools/check_links.py`, workflow *Checks*) couvrant tout
  le dépôt — pas par MkDocs, qui ne voit que `docs/`. Le Wiki GitHub n'est jamais
  la source de vérité.
- **Publication (différée)** : la mise en ligne (ex. GitHub Pages) est **reportée**
  jusqu'à une **décision explicite du mainteneur** et jusqu'à ce que l'hébergement
  choisi soit **disponible**. Sur l'offre/visibilité actuelle (dépôt privé, Free),
  GitHub Pages n'est **pas disponible** ; **aucune publication n'est déclenchée
  automatiquement**. Le workflow ne déploie que sur **déclenchement manuel**.

## 4. Raisons du choix

Seul le docs-as-code garantit que la documentation reste synchronisée, relue et
vérifiée comme le reste du projet — condition nécessaire pour « la doc comme
fonctionnalité » et pour la reprise par un tiers.

## 5. Conséquences

### Positives
- Documentation fiable, versionnée, relue et **vérifiée en CI** (build strict).
  La publication reste un acte **volontaire et différé** (pas d'auto-publication).

### Négatives / compromis acceptés
- Édition via PR (moins « instantanée » que le Wiki) — compromis assumé au
  profit de la qualité.

### Impacts futurs
- L'API du firmware pourra être générée (Doxygen) et intégrée au site.
- Toute nouvelle fonctionnalité livre sa doc dans la même PR.

## 6. Liens
- `mkdocs.yml` · [ADR-0000](0000-adopter-les-adr.md)

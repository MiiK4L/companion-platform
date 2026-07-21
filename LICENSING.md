# Stratégie de licences

Companion Platform est un monorepo qui mélange du code, du matériel, de la
documentation et des éléments d'identité visuelle. Chaque catégorie de
contenu est distribuée sous une licence adaptée à sa nature. Ce document
explique quelle licence s'applique à quel dossier, et comment la déclarer
dans vos fichiers.

## Vue d'ensemble par dossier

| Dossier                                   | Contenu                                              | Licence          |
|--------------------------------------------|-------------------------------------------------------|-------------------|
| `firmware/`                                 | Kernel, HAL, drivers, services, Companion SDK, runtime natif + Lua (l'OS est inclus ici) | Apache-2.0 |
| `apps/*/` (apps **officielles**, ex. `tamagotchi`) | Apps maintenues par le projet                    | Apache-2.0        |
| `tools/`                                    | Scripts, générateurs, provisioning, build/flash        | Apache-2.0        |
| `tests/`                                    | Tests unitaires et d'intégration                       | Apache-2.0        |
| `hardware/`                                 | Schémas, PCB, BOM, Gerbers (matériel officiel)         | CERN-OHL-S-2.0    |
| `case/`                                     | Boîtier imprimable 3D (sources CAO + STL)              | CERN-OHL-S-2.0    |
| `modules/*/hardware` et `modules/*/firmware` | Modules CX-Bus officiels                              | CERN-OHL-S-2.0 (matériel) / Apache-2.0 (firmware) |
| `docs/`                                     | Documentation, guides, ADR, architecture               | CC-BY-4.0         |
| `assets/`                                   | Sprites, animations, rendus **génériques**             | CC-BY-4.0 (sauf identité visuelle, voir ci-dessous) |
| Nom, logo, mascotte, illustrations **officielles** de Companion Platform | Identité de marque | **Réservés** (tous droits réservés) |

Le standard **CX-Bus** lui-même (`standards/cx-bus/`), en tant que
spécification documentaire, est publié sous **CC-BY-4.0**, afin que
n'importe qui puisse concevoir un module compatible.

## Le cas particulier de l'identité visuelle

Le nom du projet, son logo, sa mascotte et les illustrations officielles
qui l'accompagnent sont **réservés** : ils ne sont couverts ni par
Apache-2.0, ni par CERN-OHL-S, ni par CC-BY-4.0. Cela signifie qu'en l'état
actuel, tous les droits sur ces éléments sont conservés par le projet.

Cette réserve n'empêche pas la réutilisation du code, du matériel ou de la
documentation : elle protège uniquement la marque et son identité visuelle
distinctive, afin d'éviter toute confusion sur ce qui constitue une
version « officielle » de la plateforme. Une politique de marque plus
détaillée pourra être publiée séparément à mesure que le projet mûrit.

## Apps et modules tiers

Companion Platform est une plateforme, pas un jardin clos. Les **apps et
modules tiers** (non maintenus dans ce dépôt, ou maintenus dans un dossier
clairement identifié comme communautaire) peuvent être publiés sous
**n'importe quelle licence libre au choix de leur auteur**. Le seul
impératif est la clarté :

- Le cœur de la plateforme (firmware, HAL, Companion SDK) reste et restera
  **Apache-2.0**.
- Le matériel **officiel** (mainboard, boîtier) reste et restera
  **CERN-OHL-S-2.0**.
- Une app ou un module tiers doit indiquer explicitement sa propre licence
  (fichier `LICENSE` local et en-têtes SPDX), sans quoi elle ne peut pas
  être considérée comme redistribuable en toute confiance par des tiers.

## Déclarer la licence d'un fichier : en-têtes SPDX

Pour éviter toute ambiguïté fichier par fichier — utile notamment lorsque
plusieurs licences cohabitent dans un même dossier (par exemple un module
CX-Bus tiers avec sa propre licence) — le projet recommande l'usage
d'en-têtes **SPDX** (`SPDX-License-Identifier`) en tête de chaque fichier
source.

### Exemple — fichier C sous Apache-2.0 (firmware, HAL, drivers)

```c
// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Companion Platform contributors
```

### Exemple — script Python sous Apache-2.0 (tools/)

```python
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Companion Platform contributors
```

### Exemple — fichier de documentation sous CC-BY-4.0 (docs/)

```markdown
<!-- SPDX-License-Identifier: CC-BY-4.0 -->
```

### Exemple — app Lua tierce sous une licence différente

```lua
-- SPDX-License-Identifier: MIT
-- Copyright (c) 2026 <auteur de l'app>
```

### Exemple — fichier matériel KiCad sous CERN-OHL-S-2.0

Les formats KiCad ne supportent pas toujours un en-tête de commentaire
en première ligne ; dans ce cas, ajoutez un fichier `LICENSE` ou
`SPDX-License-Identifier` dans le champ de métadonnées du projet KiCad,
ou dans un fichier `NOTICE`/`LICENSE.txt` à la racine du dossier
matériel concerné.

## Textes de licence complets

Les textes intégraux des licences utilisées par le projet sont disponibles
dans le dossier [`LICENSES/`](LICENSES/) :

- [`LICENSES/Apache-2.0.txt`](LICENSES/Apache-2.0.txt)
- [`LICENSES/CERN-OHL-S-2.0.txt`](LICENSES/CERN-OHL-S-2.0.txt)
- [`LICENSES/CC-BY-4.0.txt`](LICENSES/CC-BY-4.0.txt)

Le fichier [`LICENSE`](LICENSE) à la racine du dépôt contient le texte
Apache-2.0, applicable par défaut au code du monorepo (firmware, os,
apps officielles, tools, tests) en l'absence d'indication contraire.

## En cas de doute

En cas de doute sur la licence applicable à un fichier ou un dossier
donné, la règle par défaut est :

1. Le fichier a-t-il un en-tête SPDX explicite ? → Utilisez cette licence.
2. Sinon, le dossier a-t-il un fichier `LICENSE` local ? → Utilisez cette
   licence.
3. Sinon, appliquez la licence par défaut du dossier telle que décrite
   dans le tableau ci-dessus.
4. En dernier recours, ouvrez une Issue pour clarification avant toute
   réutilisation.

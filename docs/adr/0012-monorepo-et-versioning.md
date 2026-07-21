# ADR-0012 : Monorepo et versioning SemVer par domaine

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideurs** : Expert GitHub, chef de projet technique
- **Phase de roadmap** : 0
- **Domaines impactés** : tous, ci
- **Tags** : monorepo, versioning, semver, git

## 1. Contexte et problème

Le dépôt GitHub doit être la **source unique de vérité**, tout en gardant des
composants « le plus découplés possible » et versionnés indépendamment
(hardware, firmware, OS, modules, applications). Deux modèles s'opposent :
multi-dépôts ou monorepo. Le choix impacte la traçabilité, la simplicité de
contribution et la stratégie de versions/tags.

## 2. Options étudiées

### Option A — Multi-dépôts (un dépôt par domaine)
- **Avantages** : versions et droits totalement isolés.
- **Inconvénients** : fragmente la source de vérité, complexifie les
  changements transverses (une évolution CX-Bus touche hardware+firmware+docs),
  onboarding plus lourd, synchronisation manuelle des versions.

### Option B — Monorepo avec versioning par domaine
- **Avantages** : source de vérité unique, changements transverses atomiques
  dans une seule PR, onboarding simple, découplage assuré par la structure de
  dossiers et non par des dépôts séparés.
- **Inconvénients** : tags multiples à gérer dans un même dépôt.

## 3. Décision

**Monorepo** unique. Découplage assuré par l'arborescence (`firmware/`,
`hardware/`, `apps/`, `modules/`, `standards/`, `docs/`, `case/`, `tools/`).
Versioning **SemVer par domaine** via **tags préfixés** :

| Domaine | Préfixe de tag | Exemple |
|---------|----------------|---------|
| Matériel (carte mère) | `hw-` | `hw-v1.0.0` |
| Firmware | `fw-` | `fw-v0.3.1` |
| OS / noyau | `os-` | `os-v0.3.1` |
| Standard CX-Bus | `spec-` | `spec-v1.0.0` |
| Module | `mod-<nom>-` | `mod-gps-v1.0.0` |
| Application | `app-<nom>-` | `app-tamagotchi-v1.0.0` |

Une **matrice de compatibilité** (docs) reliera les versions entre elles.

## 4. Raisons du choix

Le monorepo sert directement l'exigence « source unique de vérité » et facilite
les changements transverses fréquents dans un projet aussi intégré (matériel +
firmware + doc). Le découplage recherché est structurel (dossiers, HAL, SDK,
standard) et n'exige pas des dépôts séparés.

## 5. Conséquences

### Positives
- Reprise complète du projet depuis un seul dépôt ; PR transverses atomiques.

### Négatives / compromis acceptés
- Discipline de nommage des tags ; outillage de release à prévoir (CI).

### Impacts futurs
- Génération de changelog par domaine à partir des Conventional Commits.
- Possibilité d'automatiser les releases par préfixe de tag (Phase CI ultérieure).

## 6. Liens
- [ADR-0011](0011-docs-as-code.md) · [ADR-0000](0000-adopter-les-adr.md)

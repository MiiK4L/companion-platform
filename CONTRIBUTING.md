# Contribuer à Companion Platform

Merci de l'intérêt que vous portez à Companion Platform. Ce document décrit
comment proposer une contribution, quelles conventions respecter, et où
trouver le contexte nécessaire avant de vous lancer.

Companion Platform est un monorepo qui rassemble le firmware, le matériel,
les outils, les apps et la documentation. La priorité du projet est la
qualité, la robustesse, la maintenabilité et l'évolutivité — pas la
rapidité. Une contribution bien documentée et discutée en amont a plus de
valeur qu'une contribution rapide mais isolée.

## Avant de commencer

- Parcourez les [Issues du dépôt](https://github.com/MiiK4L/companion-platform/issues)
  pour vérifier qu'un sujet similaire n'existe pas déjà. *(Les GitHub Discussions
  ne sont pas activées à ce stade ; elles pourront l'être au passage en public.)*
- Pour un changement significatif (nouvelle app, nouveau module, changement d'un
  port de plateforme, nouvelle dépendance), ouvrez d'abord une **Issue** afin de
  valider l'approche avant d'investir du temps dans le code.
- Si votre changement remet en cause une décision d'architecture existante
  (runtime hybride, HAL, Companion SDK, CX-Bus...), consultez le dossier
  `docs/adr/` et envisagez de proposer une nouvelle ADR via le modèle
  d'Issue dédié (« 📐 Proposition de décision d'architecture »).

## Workflow Git

Le projet suit un modèle fork/branche courte/Pull Request :

1. **Forkez** le dépôt (ou créez une branche si vous avez les droits
   d'écriture).
2. Créez une **branche courte et ciblée**, nommée de façon descriptive,
   par exemple :
   - `feat/app-tamagotchi-sommeil`
   - `fix/hal-i2c-timeout`
   - `docs/guide-build-and-flash`
3. Faites des commits atomiques et bien décrits (voir Conventional Commits
   ci-dessous).
4. Ouvrez une **Pull Request documentée** vers `main` en utilisant le
   modèle fourni. Une branche ne doit pas rester ouverte indéfiniment :
   préférez plusieurs PR courtes et successives à une PR volumineuse et
   difficile à relire.
5. `main` est une branche **protégée** : toute intégration passe par une PR
   revue, avec CI verte.

## Conventional Commits

Tous les messages de commit suivent la spécification
[Conventional Commits](https://www.conventionalcommits.org/fr/) :

```
<type>(<portée optionnelle>): <description au présent, concise>

[corps optionnel expliquant le pourquoi]

[pied optionnel : BREAKING CHANGE, référence d'issue, ADR liée...]
```

Types utilisés dans ce projet :

| Type       | Usage                                                              |
|------------|---------------------------------------------------------------------|
| `feat`     | Nouvelle fonctionnalité                                              |
| `fix`      | Correction de bug                                                    |
| `docs`     | Documentation uniquement                                             |
| `refactor` | Changement interne sans impact fonctionnel observable                |
| `test`     | Ajout ou modification de tests                                       |
| `chore`    | Maintenance, outillage, dépendances                                  |
| `ci`       | Intégration/déploiement continu                                      |
| `adr`      | Proposition ou mise en œuvre d'une décision d'architecture           |

Exemples :

```
feat(app-tamagotchi): ajoute le cycle de sommeil nocturne

fix(hal-i2c): corrige le timeout sur bus surchargé

docs(guides): ajoute le squelette du guide de build et flash

adr: formalise ADR-0008 sur la stratégie de mise à jour OTA

chore(tools): met à jour le script de provisioning EEPROM CX-Bus
```

Une PR peut contenir plusieurs commits, mais chacun doit rester cohérent et
compréhensible isolément. Squashez les commits de correction triviale
(« fix typo », « oups ») avant de soumettre la PR.

> **Types de commits vs labels — différence volontaire.** Les *types de commit*
> suivent Conventional Commits (`feat`, `fix`, `docs`, `chore`…). Les *labels*
> GitHub emploient une granularité orientée tri (`type:feature`, `type:bug`,
> `type:adr`, `type:docs`…). C'est **intentionnel** : les commits décrivent la
> *nature technique* du changement, les labels servent la *catégorisation* et la
> recherche. Correspondance usuelle : `feat`→`type:feature`, `fix`→`type:bug`,
> `adr`→`type:adr`, `docs`→`type:docs`.

## SemVer par domaine

Le versionnage sémantique s'applique indépendamment par domaine, via des
tags préfixés :

- `hw-x.y.z` — matériel (mainboard, cartes filles)
- `fw-x.y.z` — firmware (kernel, HAL, drivers, services)
- `os-x.y.z` — système / runtime hybride
- `spec-x.y.z` — spécifications (ex. CX-Bus)
- `mod-x.y.z` — modules CX-Bus
- `app-x.y.z` — apps officielles

Indiquez dans votre PR si votre changement impacte une version publique
d'un de ces domaines, afin que le mainteneur puisse planifier le tag
correspondant.

## Style de Pull Request

- **Une PR, un objectif.** Évitez de mélanger un refactor et une nouvelle
  fonctionnalité dans la même PR.
- Remplissez entièrement le modèle de PR fourni : objectif, type de
  changement, domaine(s) concerné(s), ADR liée(s), checklist.
- Décrivez le **pourquoi**, pas seulement le quoi : le code montre déjà ce
  qui a changé, la description doit expliquer la motivation et les
  alternatives écartées si pertinent.
- Ajoutez des captures d'écran ou GIFs pour tout changement visible sur
  l'écran (LVGL, apps).
- Gardez la discussion de revue publique et argumentée ; un changement
  significatif mérite un consensus, pas une fusion hâtive.

## Documentation obligatoire

**Aucune fonctionnalité importante n'est fusionnée sans documentation
associée.** Selon la nature du changement, cela peut inclure :

- Mise à jour du README du dossier concerné.
- Mise à jour ou création d'un guide dans `docs/guides/`.
- Rédaction d'une ADR si une décision d'architecture est impliquée.
- Mise à jour du manifest d'app ou de module si applicable.

Une PR qui ajoute une fonctionnalité sans documentation correspondante sera
renvoyée pour complément avant revue de fond.

## Guide de code et processus ADR

- Pour les conventions de style (C, Lua, nommage, outillage), consultez
  [`docs/guides/code-style.md`](docs/guides/code-style.md) et
  [`docs/guides/contributing-code.md`](docs/guides/contributing-code.md).
- Pour comprendre comment une décision d'architecture est proposée,
  discutée puis formalisée, consultez `docs/adr/` et le modèle d'Issue
  « 📐 Proposition de décision d'architecture ».

## Certificate of Origin (DCO) — optionnel, non appliqué à ce stade

Le *sign-off* DCO est **entièrement optionnel** aujourd'hui : **aucune
vérification automatique (CI DCO) n'est en place** et une PR n'est pas rejetée
faute de sign-off. Si le projet décide plus tard de le **rendre obligatoire**,
une **politique explicite et une CI dédiée** seront ajoutées et annoncées ici.

En attendant, vous **pouvez** signer vos commits pour certifier que vous êtes
l'auteur de la contribution ou avez le droit de la soumettre sous la licence du
projet, conformément au
[Developer Certificate of Origin](https://developercertificate.org/).

Ajoutez `-s` à vos commits :

```
git commit -s -m "feat(app-tamagotchi): ajoute le cycle de sommeil nocturne"
```

Cela ajoute une ligne `Signed-off-by: Prénom Nom <email>` au message de
commit.

## Licences

En contribuant, vous acceptez que votre contribution soit distribuée sous
la licence applicable au dossier concerné (voir
[`LICENSING.md`](LICENSING.md) pour le détail par dossier). Les apps et
modules tiers peuvent choisir leur propre licence libre, à condition de la
déclarer clairement (en-tête SPDX recommandé).

## Reproductibilité des dépendances

Le projet privilégie des builds **reproductibles et sûrs**. Toutes les
dépendances d'outillage sont épinglées :

- **Dépendances Python de la documentation** : verrou complet (versions figées,
  y compris transitives) dans [`docs/requirements.txt`](docs/requirements.txt).
  La CI et l'environnement local installent exactement les mêmes versions
  (`pip install -r docs/requirements.txt`).
- **GitHub Actions** : épinglées au **SHA de commit** dans les workflows
  (`.github/workflows/`), avec un commentaire indiquant la version lisible
  (ex. `uses: actions/checkout@<sha> # v7.0.1`). L'épinglage au SHA évite
  qu'un tag mobile ne modifie silencieusement le comportement de la CI.

Les montées de version sont proposées automatiquement par **Dependabot**
(voir [`.github/dependabot.yml`](.github/dependabot.yml)) sous forme de Pull
Requests hebdomadaires, **relues et validées** comme tout autre changement —
jamais appliquées en aveugle. Ne modifiez pas une version épinglée à la main
sans raison documentée dans la PR.

## Code de conduite

Toute participation au projet est soumise au
[Code de conduite](CODE_OF_CONDUCT.md). Merci de le lire avant de
contribuer.

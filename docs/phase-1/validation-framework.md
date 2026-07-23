<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Phase 1 — Cadre de validation & modèle de preuves

Ce document de cadrage définit **comment** la Phase 1 produit et conserve des preuves, et
**à quelles conditions** une décision peut changer de statut. Il ne fige aucune décision
technique : il pose les règles de la démarche de validation.

Documents liés :

- [Sourcing & BOM (candidates)](sourcing-and-bom.md)
- [Modèle — Protocole de test](templates/test-protocol-template.md)
- [Modèle — Rapport de mesure](templates/measurement-report-template.md)

## 1. Convention de stockage des données brutes

Les données brutes sont la matière première des preuves. Elles doivent être conservées de
manière prévisible et rejouable.

- **Où** : sous le dossier du lot, dans la PR concernée (les données brutes vivent avec le
  travail qui les a produites, et non dans un espace séparé).
- **Format** : formats ouverts et exploitables — CSV pour les séries de mesures, traces
  d'instrument, captures (écran, oscilloscope). Éviter les formats propriétaires opaques
  quand un équivalent ouvert existe.
- **Nommage** : nommage explicite et ordonnable, reliant le fichier à son protocole et à sa
  date (par exemple `<id-protocole>_<date>_<variante>.csv`). Le nom doit permettre de
  retrouver le protocole et le contexte sans ouvrir le fichier.
- **Versionnement** : les données brutes sont versionnées avec la PR ; toute reprise de
  mesure crée un nouveau fichier daté plutôt que d'écraser l'ancien.
- **Taille raisonnable** : conserver un volume proportionné (échantillonnage/agrégation
  documentés si le brut intégral est trop volumineux). Le critère de conservation est la
  **rejouabilité de l'analyse**, pas l'exhaustivité aveugle.

### Politique de volume des données (le dépôt ne doit pas enfler sans limite)

Seuils **proposés, à confirmer** :

- **Git direct** : fichiers **texte** (CSV, logs) **≤ 1 Mo/fichier**.
- **Git LFS** : fichiers binaires ou traces **> 1 Mo et ≤ 50 Mo** (captures d'oscilloscope,
  images). **Opérationnel** : la **PR du premier lot utilisant LFS** ajoute les règles
  **`.gitattributes`** (par extension) **avant** de committer les données concernées. **Si LFS
  n'est pas disponible**, les fichiers vont en **stockage externe** — **jamais** en blob Git
  classique.
- **Stockage externe** : au-delà de **50 Mo**. Gouvernance obligatoire :
  **responsable du stockage** nommé (par défaut le mainteneur), **pérennité du lien**
  (URL stable, pas d'espace éphémère), **contrôle d'accès** documenté, **hash SHA-256**
  enregistré, **vérification périodique** du lien+hash (**au moins à chaque revue de lot et
  en L9**), et **procédure de migration** si le fournisseur change.
- **Limite par lot / PR** : en plus de la limite par fichier, **≤ 200 Mo** cumulés committés
  (Git + LFS) par PR de lot ; au-delà, agréger/échantillonner ou basculer en stockage externe.
- **Hash SHA-256** : chaque fichier de données brutes est accompagné de son **SHA-256**
  (reporté dans le rapport de mesure), quel que soit le lieu de stockage.
- **Métadonnées & conservation** : chaque jeu porte instrument, date, opérateur, `DEC-*` et
  protocole associés. **Règle de conservation** : les données d'un résultat **qui soutient
  une décision Acceptée** sont **conservées tant que l'ADR correspondante n'est pas
  Remplacée/Rejetée** ; les données purement **exploratoires** peuvent être **élaguées après
  clôture du lot** (en conservant le rapport qui les résume et leur hash).
- **Formats admis** : formats **ouverts** (CSV, PNG, formats d'export documentés) ; éviter les
  formats propriétaires opaques quand un équivalent ouvert existe.

## 2. Identifiant de question de décision (`DEC-*`) & traçabilité

Pour éviter tout **cycle** (« l'ADR référence la preuve / la preuve référence l'ADR » alors
que l'ADR n'existe pas encore), chaque **question de décision** reçoit un **identifiant
indépendant** `DEC-<lot>-NNN` (par exemple `DEC-L2-001`), créé **à l'ouverture du lot,
avant les essais**. Le protocole et le rapport de mesure référencent ce `DEC-*`, jamais un
numéro d'ADR.

**Cycle de vie (modèle unique retenu — ne pas mélanger avec un autre) :**

1. **Ouverture** : la question est enregistrée comme `DEC-*` (énoncé, options envisagées, lot).
2. **Mesure** : le **protocole** puis le **rapport** référencent le `DEC-*`.
3. **Arbitrage** : lorsqu'un arbitrage est possible (options comparées, seuils atteints ou
   non), une **ADR (0013+) est créée *après* les mesures** ; elle référence le `DEC-*` et les
   preuves, et porte le statut correspondant (Accepté si seuils atteints ; Rejeté pour une
   option écartée ; Proposé seulement si la décision est prise mais reste à confirmer).
4. **Bidirectionnel** : une fois l'ADR créée, les liens deviennent bidirectionnels —
   **ADR ↔ `DEC-*` ↔ rapports ↔ données brutes**.

Ainsi aucune preuve n'est orpheline, aucune ADR n'est créée sans preuve, et **aucun numéro
d'ADR n'est exigé avant que le protocole et le rapport n'existent**.

## 3. Critères Proposé → Accepté

**Atteindre les seuils ne suffit pas** : plusieurs options peuvent les atteindre. Le passage
de **Proposé** à **Accepté** exige **les quatre** conditions cumulatives :

1. **Conformité aux exigences** : la solution satisfait les exigences applicables
   ([matrice des exigences](requirements-matrix.md)), pas seulement les seuils du protocole.
2. **Comparaison des alternatives pertinentes** : les alternatives crédibles de la même
   fonction ont été évaluées (mêmes protocole/seuils quand c'est pertinent).
3. **Justification des compromis** : l'ADR explique **pourquoi cette option est retenue** face
   aux autres (coût, conso, disponibilité, complexité, risques), y compris quand plusieurs
   atteignent les seuils.
4. **Traçabilité vers les preuves** : liens `DEC-*` ↔ rapport(s) reproductible(s) ↔ données
   brutes (avec hash).

- Un **rapport reproductible** atteignant les **seuils chiffrés** est **nécessaire** mais non
  suffisant ; à défaut de l'une des conditions, la décision **reste Proposé**.
- Une option peut être **Rejetée**, à condition de laisser une **trace** (rapport et données
  brutes documentant l'échec ou le non-respect des seuils).

Aucun passage en Accepté n'est admis sur la base d'une impression, d'un consensus ou d'un
essai unique.

## 4. Résultat exploratoire vs reproductible

La distinction est structurante pour le modèle de preuves.

**Résultat exploratoire** :

- Essai unique.
- Matériel et procédure **non figés**.
- Sert à **orienter** (défricher, éliminer grossièrement, choisir quoi mesurer sérieusement).
- Ne peut **pas** faire passer une décision en Accepté.

**Résultat reproductible** :

- Protocole **figé** (voir le modèle de protocole de test).
- **Seuils** chiffrés définis avant l'essai.
- **Incertitude** évaluée.
- **Données brutes conservées** (avec SHA-256).
- **Rejouable** par un tiers.

**Combien d'exécutions ?** Pas de `n ≥ 5` universel. Le protocole **distingue et justifie**
quatre grandeurs, selon le **risque**, la **dispersion** attendue et le caractère
**destructif** de l'essai :

- **`n_dut`** : nombre d'exemplaires distincts du *device under test* (plusieurs cartes/
  modules, pour capter la variabilité de fabrication).
- **`n_runs`** : nombre de mesures répétées sur **un même** DUT dans une même campagne.
- **`n_campaigns`** : nombre de **campagnes indépendantes** (voir ci-dessous).
- **`n_cycles`** : nombre de cycles pour les essais d'**endurance/usure** (ex. insertions
  connecteur), distinct des répétitions de mesure.

Chaque nombre est **justifié dans le protocole** (un essai destructif aura un `n_dut` faible
mais documenté ; une grandeur très dispersée exigera un `n_runs`/`n_campaigns` plus élevé).

**Deux campagnes sont *indépendantes*** si elles diffèrent par au moins : un **remontage/
recâblage** du banc, un **moment distinct** (et si possible un **opérateur** ou un **DUT**
distinct), avec **re-vérification de l'étalonnage**. Rejouer une acquisition sans rien
remonter ne compte **pas** comme une campagne indépendante.

Un résultat **reproductible** exige **`n_campaigns ≥ 2`** (indépendantes) et des `n_dut`/
`n_runs` justifiés, avec **résultats consignés par répétition** et **méthode statistique**
déclarée. Une seule exécution reste **exploratoire**.

Seul un résultat **reproductible** peut faire passer une décision de Proposé à Accepté.

## 5. Conformité aux contrôles CI existants

Les documents de planification et les rapports produits en Phase 1 doivent rester conformes
aux contrôles d'intégration continue déjà en place :

- `mkdocs build --strict` — la documentation doit construire sans avertissement.
- **Contrôle de liens** — aucun lien cassé ; liens relatifs uniquement.
- `reuse lint` — chaque fichier porte les en-têtes SPDX de licence et de copyright.
- **commitlint** — les messages de commit respectent la convention en vigueur.

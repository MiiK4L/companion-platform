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
  images). LFS est activé pour les extensions concernées avant dépôt.
- **Stockage externe** : au-delà de **50 Mo**, la donnée brute **n'est pas** committée ; on
  conserve **un lien** vers le stockage externe **+ le hash**.
- **Hash SHA-256** : chaque fichier de données brutes est accompagné de son **SHA-256**
  (reporté dans le rapport de mesure) pour garantir l'intégrité, quel que soit le lieu de
  stockage.
- **Métadonnées & conservation** : chaque jeu de données porte instrument, date, opérateur,
  `DEC-*` et protocole associés ; **durée de conservation** minimale à définir (ex. tant que
  la décision qu'il justifie reste en vigueur).
- **Formats admis** : privilégier des formats **ouverts** (CSV, PNG, formats d'export
  documentés) ; éviter les formats propriétaires opaques quand un équivalent ouvert existe.

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

Le passage d'une décision de **Proposé** à **Accepté** est conditionné :

- Une décision ne passe **Accepté** que si un **rapport reproductible** atteint les
  **seuils chiffrés** définis par son protocole de test.
- À défaut, la décision **reste Proposé** (preuve insuffisante, non reproductible, ou seuils
  non atteints).
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

**Combien d'exécutions pour ne PAS être un essai unique ?** Un résultat n'est
**reproductible** que s'il repose sur **au moins 2 campagnes indépendantes**
(rejouées, idéalement à des moments/opérateurs distincts) **et** un **nombre
d'échantillons `n` défini dans le protocole** (par défaut proposé : `n ≥ 5`, à
ajuster selon la grandeur et sa dispersion), avec **résultats consignés par
répétition** et **méthode statistique** déclarée. Une seule exécution reste
**exploratoire**, quels que soient ses résultats.

Seul un résultat **reproductible** peut faire passer une décision de Proposé à Accepté.

## 5. Conformité aux contrôles CI existants

Les documents de planification et les rapports produits en Phase 1 doivent rester conformes
aux contrôles d'intégration continue déjà en place :

- `mkdocs build --strict` — la documentation doit construire sans avertissement.
- **Contrôle de liens** — aucun lien cassé ; liens relatifs uniquement.
- `reuse lint` — chaque fichier porte les en-têtes SPDX de licence et de copyright.
- **commitlint** — les messages de commit respectent la convention en vigueur.

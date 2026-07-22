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

## 2. Traçabilité ADR ↔ preuves

La traçabilité est **bidirectionnelle** :

- Chaque **ADR de décision (0013 et suivants)** référence explicitement le ou les
  **rapports de mesure** et les **données brutes** qui la justifient.
- Chaque **rapport de mesure** pointe l'**ADR** qu'il alimente.

Ce double lien garantit qu'aucune décision n'est prise sans preuve rattachée, et qu'aucune
preuve n'est orpheline. Un lecteur doit pouvoir naviguer d'une décision vers ses mesures, et
d'une mesure vers la décision qu'elle sert.

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
- **Données brutes conservées**.
- **Rejouable** par un tiers.

Seul un résultat **reproductible** peut faire passer une décision de Proposé à Accepté.

## 5. Conformité aux contrôles CI existants

Les documents de planification et les rapports produits en Phase 1 doivent rester conformes
aux contrôles d'intégration continue déjà en place :

- `mkdocs build --strict` — la documentation doit construire sans avertissement.
- **Contrôle de liens** — aucun lien cassé ; liens relatifs uniquement.
- `reuse lint` — chaque fichier porte les en-têtes SPDX de licence et de copyright.
- **commitlint** — les messages de commit respectent la convention en vigueur.

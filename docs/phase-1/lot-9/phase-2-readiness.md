<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Préparation à la Phase 2 (readiness)

> **Statut : Proposé — état de préparation, sans décision.** Ce document évalue,
> pour chaque `DEC-*`, si les **conditions matérielles et méthodologiques** d'une
> campagne de mesure sont réunies. Il permet de savoir **objectivement** quand la
> Phase 2 peut démarrer — **lot par lot**, sans attendre que tout soit prêt.

## Ce que « Phase 2 » désigne ici

La **Phase 2** est la phase de **mesure et de prototypage** : elle **exécute** les
protocoles rédigés en Phase 1 et, sur preuves reproductibles, crée les **ADR
0013+**. Les [critères de sortie](../execution.md) (budgets mesurés, sûreté
démontrée, ADR acceptées, spec promue `rc.1`) sont les **critères de sortie de la
Phase 2**, pas de la Phase 1.

## Axes de préparation & légende

| Axe | Question |
|-----|----------|
| **Doc** | La comparaison/analyse documentaire est-elle complète ? |
| **Proto** | Le protocole est-il rédigé (et prêt au baselining : seuils `[BL]` à figer) ? |
| **Banc** | Le banc de mesure est-il défini et disponible/monté ? |
| **Compos.** | Les échantillons d'évaluation (éventail ≥ 2/fonction) sont-ils identifiés/acquis ? |
| **Mesurable** | La grandeur est-elle mesurable une fois banc + échantillons réunis ? |

Légende : **✓** prêt · **◑** partiel (spécifié, à instancier/monter/acquérir) · **○** à faire.

## Tableau de préparation par décision

| `DEC-*` | Doc | Proto | Banc | Compos. | Mesurable | Verdict |
|---------|:---:|:---:|:---:|:---:|:---:|---------|
| `DEC-L1-001` | ✓ | ◑ | ◑ | ◑ | ✓ | Protocole de bring-up **à instancier** depuis le [modèle](../templates/test-protocol-template.md) ; sinon prêt |
| `DEC-L1-002` | ✓ | ◑ | ◑ | ◑ | ○ | **Gated** : dépend des résultats L4 (RAM) et L5 (deep-sleep) ; réutilise leurs bancs |
| `DEC-L2-001` | ✓ | ✓ | ◑ | ◑ | ✓ | Protocoles rédigés ; banc d'injection à **monter** ; échantillons sûreté à acquérir |
| `DEC-L2-002` | ✓ | ✓ | ◑ | ◑ | ✓ | Fixtures par famille à **fabriquer** ; DUT = paires accouplées à acquérir |
| `DEC-L2-003` | ✓ | ✓ | ◑ | ◑ | ✓ | Réutilise le banc back-powering 2A (L2A) |
| `DEC-L3-001` | ✓ | ✓ | ◑ | ◑ | ✓ | Prérequis L2 (isolation/rail d'ID) ; banc HW+CODE à monter |
| `DEC-L3-002` | ✓ | ✓ | ✓ | ✓ | ✓ | **Testable sur host** (parsing borné) : jeu de Manifests de test suffit |
| `DEC-L4-001` | ✓ | ✓ | ◑ | ◑ | ✓ | Éventail d'écrans d'éval à acquérir ; banc FPS/conso à monter |
| `DEC-L4-002` | ✓ | ✓ | ◑ | ◑ | ✓ | Moteur(s) candidat(s) à porter ; mesure RAM/FPS/perçu |
| `DEC-L5-001` | ✓ | ✓ | ◑ | ◑ | ✓ | Fixtures de support + cycleur ; cartes cœur d'éval à acquérir |
| `DEC-L6-001` | ✓ | ✓ | ◑ | ◑ | ✓ | Banc conso/charge/thermique ; agrège les entrées L4/L5/L7 |
| `DEC-L6-002` | ✓ | ✓ | ◑ | ◑ | ✓ | Cellules candidates + cycleur de décharge |
| `DEC-L6-003` | ✓ | ✓ | ◑ | ◑ | ✓ | Référence coulomb requise |
| `DEC-L7-001` | ✓ | ✓ | ◑ | ◑ | ✓ | Référence de temps + RTC candidats |
| `DEC-L7-002` | ✓ | ✓ | ◑ | ◑ | ✓ | Banc de coupures brutales à monter |
| `DEC-L8-001` | ✓ | ✓ | ◑ | ◑ | ◑ | **Host déjà mesurable** (CI verte) ; **RAM/flash cible** nécessite toolchain + POC sur cible |
| `DEC-L8-002` | ✓ | ✓ | ◑ | ◑ | ◑ | Faisabilité install dynamique = POC de chargement sur cible |

> **Lecture** : la **documentation** et les **protocoles** sont **prêts** (✓)
> partout ; les protocoles restent au statut **Brouillon** et exigent un
> **baselining** (figer les seuils `[BL]`, passer *Brouillon → Revu → Baseline
> d'essai*). Ce qui manque partout est **physique** : **monter les bancs** et
> **acquérir les échantillons** — activités de **début de Phase 2**, pas de
> Phase 1. Deux volets sont déjà partiellement mesurables **sans matériel** :
> le **parsing du Manifest** (`DEC-L3-002`) et le **squelette host L8** (build +
> tests + graphe de dépendances, déjà verts en CI).

## Critère objectif de démarrage de la Phase 2

La Phase 2 peut démarrer **pour un lot donné** dès que, pour ses `DEC-*` :

1. **Doc complète** — comparaison/analyse documentaire figée *(✓ pour tous les lots)* ;
2. **Protocole baseliné** — seuils `[BL]` fixés, protocole passé au statut
   **Baseline d'essai** (cf. [cadre de validation](../validation-framework.md)) ;
3. **Banc monté & étalonné** — instruments disponibles, étalonnage vérifié ;
4. **Échantillons acquis** — éventail ≥ 2/fonction en main (cycle de vie + stock
   vérifiés, cf. [sourcing & BOM](../sourcing-and-bom.md)) ;
5. **Mesurabilité confirmée** — un essai **exploratoire** (non reproductible)
   valide la chaîne d'acquisition avant la campagne reproductible.

**Conséquence pratique** : la Phase 1 (documentaire) étant close, **le seul
travail restant avant de mesurer est l'acquisition physique** (bancs +
échantillons) et le **baselining** des protocoles. Aucun blocage documentaire ne
subsiste. Le **logiciel host (L8)** et le **parsing Manifest (L3-002)** peuvent
avancer **immédiatement**, sans matériel.

## État des risques (renvoi)

Les risques R1–R9 restent **Ouverts / En analyse** : leur clôture dépend des
mesures de Phase 2 (voir [registre des risques](../risk-register.md)). Aucun
risque n'est **accepté** ou **clos** sur la seule base documentaire.

## Renvois

- [Synthèse Phase 1](synthesis.md) · [Feuille de route des décisions](decision-roadmap.md)
- [Registre des risques](../risk-register.md) · [Sourcing & BOM](../sourcing-and-bom.md)
- [Cadre de validation](../validation-framework.md) · [Ordre d'exécution & critères de sortie](../execution.md)

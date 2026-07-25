<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Préparation des campagnes de mesure (Phase 1)

> **Statut : Proposé — état de préparation, sans décision.** Ce document évalue,
> pour chaque `DEC-*`, le degré de préparation des **campagnes de mesure de la
> Phase 1** (les essais, prototypes et rapports qui restent à conduire). Il permet
> de savoir **objectivement** ce qui est prêt et ce qui reste à faire — **lot par
> lot** — avant d'exécuter un protocole.

## Cadre : où en est la Phase 1

> **Les travaux documentaires initiaux (lots L1 à L9) sont terminés. La Phase 1
> reste ouverte jusqu'à l'exécution des campagnes expérimentales prévues par le
> plan.**

La Phase 1 comporte **deux volets** :

- un **volet documentaire** (lots L1 à L9) — **terminé** : analyses, comparaisons,
  protocoles rédigés, séries représentatives sourcées, et cette consolidation ;
- un **volet expérimental** (campagnes de mesure, prototypes, rapports, création
  des ADR 0013+) — **encore ouvert**.

Ce Lot 9 est un **checkpoint documentaire** : il consolide le volet documentaire
et **prépare** le volet expérimental. Les
[critères de sortie de la Phase 1](../execution.md) (budgets mesurés, sûreté
démontrée, ADR acceptées, SPEC promue `rc.1`) **restent des critères de Phase 1**
et **ne sont pas satisfaits** par ce lot. La **Phase 2** conserve son sens
initial : elle vient **après** la satisfaction des critères de sortie de Phase 1.

## Niveaux de maturité d'un protocole

La préparation d'un essai n'est pas binaire. On distingue **cinq niveaux**, dans
cet ordre :

1. **Analyse documentaire rédigée** — comparaison/analyse écrite (fait, L1–L8).
2. **Protocole générique rédigé** — procédure type, statut *Brouillon*, champs `[BL]` ouverts.
3. **Protocole revu / validé** — relu et corrigé (statut *Revu*) ; **cesse d'évoluer**.
4. **Protocole instancié** — décliné pour une **option / un DUT** précis.
5. **Baseline d'essai approuvée** — seuils `[BL]` fixés, baseline validée (statut *Baseline d'essai*).

> **Aujourd'hui, l'état général est aux niveaux 1–2** (analyse + protocole
> générique **rédigé**, statut *Brouillon*). Les niveaux 3–5 restent à produire :
> **aucun protocole n'est encore revu**, donc **tous restent susceptibles
> d'évoluer**, et **aucun protocole avec un champ `[BL]` ouvert n'est « prêt à
> exécuter ».**

## Axes de préparation & légende

| Axe | Question |
|-----|----------|
| **Doc** | L'analyse/comparaison documentaire est-elle rédigée ? |
| **Rédigé** | Le protocole **générique** est-il rédigé (statut *Brouillon*) ? |
| **Revu** | Le protocole est-il **relu et validé** (statut *Revu*, ne bouge plus) ? |
| **Baseline** | La **baseline d'essai** est-elle approuvée (seuils `[BL]` fixés) ? |
| **Banc** | Le banc de mesure est-il défini et disponible/monté ? |
| **Compos.** | Les échantillons d'évaluation (éventail ≥ 2/fonction) sont-ils identifiés/acquis ? |
| **Mesurable** | La grandeur est-elle mesurable une fois les éléments réunis ? |

Légende : **✓** prêt · **◑** partiel (spécifié, à instancier/monter/acquérir) ·
**○** à faire · **N/A** non applicable (ressource sans objet pour cet essai).

## Tableau de préparation par décision

| `DEC-*` | Doc | Rédigé | Revu | Baseline | Banc | Compos. | Mesurable | Verdict |
|---------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|---------|
| `DEC-L1-001` | ✓ | ◑ | ○ | ○ | ◑ | ◑ | ◑ | Protocole de bring-up **à instancier** depuis le [modèle](../templates/test-protocol-template.md) ; à rédiger, revoir, baseliner |
| `DEC-L1-002` | ✓ | ◑ | ○ | ○ | ◑ | ◑ | ○ | **Gated** : dépend des résultats L4 (RAM) et L5 (deep-sleep) ; réutilise leurs bancs |
| `DEC-L2-001` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ◑ | Protocoles génériques rédigés (*Brouillon*) ; revue + baseline + banc d'injection à produire |
| `DEC-L2-002` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ◑ | Fixtures par famille à fabriquer ; DUT = paires accouplées à acquérir |
| `DEC-L2-003` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ◑ | Réutilise le banc back-powering 2A (L2A) |
| `DEC-L3-001` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ◑ | Prérequis L2 (isolation/rail d'ID) ; banc HW+CODE à monter |
| `DEC-L3-002` | ✓ | ✓ | ○ | ○ | N/A | N/A | ◑ | **Essai logiciel** (parsing borné sur **corpus**) : ni banc ni composant physique ; **corpus/fuzzing à préparer** |
| `DEC-L4-001` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ◑ | Éventail d'écrans d'éval à acquérir ; banc FPS/conso à monter |
| `DEC-L4-002` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ◑ | Moteur(s) candidat(s) **à porter** ; mesure RAM/FPS/perçu |
| `DEC-L5-001` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ◑ | Fixtures de support + cycleur ; cartes cœur d'éval à acquérir |
| `DEC-L6-001` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ◑ | Banc conso/charge/thermique ; agrège les entrées L4/L5/L7 |
| `DEC-L6-002` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ◑ | Cellules candidates + cycleur de décharge |
| `DEC-L6-003` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ◑ | Référence coulomb requise |
| `DEC-L7-001` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ◑ | Référence de temps + RTC candidats |
| `DEC-L7-002` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ◑ | Banc de coupures brutales à monter |
| `DEC-L8-001` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ○ | **Preuve host disponible** (architecture) ; **RAM/flash cible** encore **non mesurable** (toolchain + POC sur cible requis) |
| `DEC-L8-002` | ✓ | ✓ | ○ | ○ | ◑ | ◑ | ○ | **Preuve host disponible** ; **install dynamique + comportement cible** encore **non mesurables** (POC de chargement sur cible requis) |

> **Cas particulier L8 — ne pas confondre deux preuves distinctes** :
>
> - **Preuve host déjà disponible** : le squelette (build + tests + graphe de
>   dépendances, verts en CI) démontre l'**architecture ports/adaptateurs** et
>   l'abstraction du runtime par `IRuntime`.
> - **Preuve nécessaire pour arbitrer `DEC-L8-001/002`, encore absente** :
>   RAM/flash du runtime **embarqué**, faisabilité de l'**installation dynamique**,
>   comportement **sur cible**. Ces mesures exigent une toolchain cible et des POC
>   non encore réalisés.

## Ce qui reste à faire (au-delà du physique)

Le **cadrage documentaire générique** est disponible. **Ne pas** en conclure que
seul le matériel manque : il reste un travail **logiciel et méthodologique**
substantiel. Restent notamment :

- la **sélection des options** réellement mises au banc (toutes ne le seront pas) ;
- la **revue**, l'**instanciation** et le **baselining** des protocoles (niveaux 3–5) ;
- la préparation des **bancs, fixtures et firmwares de test** ;
- le **corpus / fuzzing** du parser de Manifest (`DEC-L3-002`) ;
- le **portage** des moteurs graphiques candidats (`DEC-L4-002`) ;
- le **POC runtime sur cible** et le **POC de distribution** (`DEC-L8-001/002`) ;
- les **scripts de provisioning** (`DEC-L3-001`) ;
- l'**acquisition** des bancs et des échantillons ;
- l'**exploitation** et la **comparaison** des résultats.

> **En résumé** : le cadrage documentaire générique est disponible. Restent la
> **sélection des options** à évaluer, l'**instanciation et le baselining** des
> protocoles, la préparation des **bancs / fixtures / firmwares / corpus**,
> l'**acquisition** des échantillons et l'**exécution** des campagnes.

## Critère objectif de démarrage d'une campagne (Phase 1 expérimentale)

Une campagne de mesure peut démarrer **pour un lot donné** dès que, pour ses
`DEC-*` :

1. **Doc complète** — analyse/comparaison figée *(✓ pour tous les lots)* ;
2. **Options sélectionnées** — les options réellement mises au banc sont arrêtées ;
3. **Protocole revu, instancié + baseliné** — relu et validé (statut *Revu*),
   décliné par option/DUT, seuils `[BL]` fixés, statut **Baseline d'essai**
   (cf. [cadre de validation](../validation-framework.md)) ;
4. **Banc / fixture / firmware prêts** — montés et étalonnés (ou **N/A** pour un
   essai purement logiciel : corpus prêt à la place) ;
5. **Échantillons acquis** — éventail ≥ 2/fonction en main (cycle de vie + stock
   vérifiés, cf. [sourcing & BOM](../sourcing-and-bom.md)), **N/A** si sans objet ;
6. **Mesurabilité confirmée** — un essai **exploratoire** valide la chaîne
   d'acquisition avant la campagne reproductible.

## État des risques (renvoi)

Les risques R1–R9 restent **Ouverts / En analyse** : leur clôture dépend des
**campagnes de mesure de la Phase 1** (voir [registre des risques](../risk-register.md)).
Aucun risque n'est **accepté** ou **clos** sur la seule base documentaire.

## Renvois

- [Synthèse Phase 1](synthesis.md) · [Feuille de route des décisions](decision-roadmap.md)
- [Registre des risques](../risk-register.md) · [Sourcing & BOM](../sourcing-and-bom.md)
- [Cadre de validation](../validation-framework.md) · [Ordre d'exécution & critères de sortie de Phase 1](../execution.md)

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Phase 1 — Ordre d'exécution, dépendances & critères de sortie

> **Statut : planification.** Ce document fixe l'enchaînement des lots, ce qui
> est parallélisable, la stratégie d'intégration (une PR par lot) et les
> **critères de sortie** de la Phase 1.

## Graphe d'exécution

```text
L1 (exigences & budgets)  -- prealable, fige les contraintes
  |
  +--> L2 (surete electrique, HW critique) --> L3 (identification, depend de L2)
  |
  +--> L4 (ecran)   L5 (coeur/socket)   L7 (RTC)      [en parallele apres L1]
  |         \            |               /
  |          +-----> L6 (alimentation, HW ; couple L2/L4/L5/L7)
  |
  +--> L8 (logiciel ports/adaptateurs, CODE host // ; POC HW ensuite)
  |
  v
L9 (synthese, ADR, spec rc.1)  <-- depend de tous les lots
```

- **Chemin critique** : L1 → L2 → L6 → L9.
- **Séquentiel obligé** : L2 **avant** L3 (lecture sûre du Manifest) ; L9 **en dernier**.

## Dépendances (résumé)

| Lot | Dépend de | Alimente |
|-----|-----------|----------|
| L1 | — | tous |
| L2 | L1 ; ↔ L6 | L3, gel connecteur (L9) |
| L3 | L2, L1 | L9 |
| L4 | L1 ; ↔ L2, L6 | L9 |
| L5 | L1 ; ↔ L6 | boîtier (Phase 4), L9 |
| L6 | L1 ; ↔ L2, L4, L5, L7 | L9 |
| L7 | L1 ; ↔ L6, L8 | L9 |
| L8 | L1 (ports) | L9 |
| L9 | tous | — |

## Parallélisation

- **Dès L1 clos** : démarrer **L8** (partie host, sans matériel) en parallèle.
- **Après L1 + banc d'alim de base (L6 partiel)** : **L4, L5, L7** en parallèle.
- **L3** démarre une fois **L2** suffisamment avancé (isolation/rail d'ID).
- **Contrainte** : un seul opérateur matériel → la parallélisation HW réelle
  dépend de la disponibilité des bancs ; le logiciel (L8, host) se parallélise
  sans contrainte matérielle.

## Stratégie d'intégration : une PR par lot

- **PR de planification** (celle-ci) : cadrage + cadre de validation + modèles.
- **Une PR indépendante par lot** (`phase-1/l2-...`, `phase-1/l3-...`, etc.) :
  chacune livre son **rapport de mesure reproductible**, ses **données brutes**,
  la ou les **ADR** associées (statut ajusté par la preuve) et, si utile, un
  petit **PCB de validation** (KiCad) + code de test.
- **PR finale de synthèse** (L9) : ADR consolidées, BOM V1 candidate, matrice de
  compatibilité, promotion SPEC (au plus `rc.1`).
- **Pas de branche parapluie.** Chaque PR de lot doit être **relisible et
  fusionnable indépendamment**, **sans laisser `main` dans un état trompeur** :
  une décision reste **Proposé** tant que sa PR n'apporte pas la preuve
  reproductible qui la fait passer **Accepté**.

## Découpage prévisionnel des commits (exemple, PR d'un lot HW — L6)

1. `docs(phase-1): objectifs et protocole du lot alimentation`
2. `docs(phase-1): banc de mesure et procédures`
3. `docs(phase-1): résultats et budget énergétique mesuré`
4. `adr: ADR-00xx chaîne d'alimentation (Proposé→Accepté ou maintenu Proposé)`
5. `chore(hardware): PCB de validation alim (si applicable)`

## Critères de sortie de la Phase 1 (porte vers la Phase 2)

1. Budgets **GPIO, énergie (mesuré), coût, mécanique** consolidés et cohérents.
2. **Sûreté électrique CX-Bus démontrée** (isolation/commutation, haute-Z, inrush
   borné, court-circuit non destructif, bus-stuck géré, retrait en transaction)
   selon des **seuils chiffrés reproductibles**.
3. **Famille de connecteur** choisie + **endurance** (cycles + résistance de
   contact) testée.
4. **Mécanisme d'identification** décidé **après comparaison d'architectures** +
   preuve de lecture sûre + format Manifest arrêté.
5. **Écran** : RAM LVGL, FPS, conso rétroéclairage **mesurés** et dans le budget.
6. **Alimentation** : chaîne validée, **budget énergétique mesuré**, cible
   d'autonomie **provisoire** fixée.
7. **RTC/Δt** validés à travers deep-sleep ; persistance robuste (seuils atteints).
8. **Logiciel** : squelette ports/adaptateurs **compile + tests host verts en
   CI** ; composition root démontré ; faisabilité Lua/installation dynamique
   **chiffrée**.
9. **≥ 2 alternatives sourcées** par composant critique (cycle de vie + stock
   vérifiés) ; **BOM V1 candidate** produite (pas de BOM de production).
10. **ADR 0013+** : celles **prouvées** passent en *Accepté* ; les autres restent
    *Proposé* (ou une option est *Rejetée* avec trace).
11. **SPEC CX-Bus** promue **au maximum en `spec-v1.0.0-rc.1`** (sinon `0.x`
    stabilisée). **`spec-v1.0.0` n'est PAS un objectif de Phase 1** : il exigera
    une carte Host **intégrée**, **plusieurs types de modules**, une **campagne
    d'interopérabilité** et la **résolution des retours d'implémentation**.

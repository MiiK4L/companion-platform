<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Phase 1 — Registre initial des risques

> Registre **initial et vivant** : il est mis à jour à **chaque lot** du
> [plan de Phase 1](./plan.md). Aucun risque n'est figé, aucune mitigation n'est
> présélectionnée. Les candidats technologiques évoqués restent **Proposés**.
>
> Statuts possibles : **Ouvert** / **En analyse** / **Mitigé** / **Accepté** / **Clos**.
> Propriétaire par défaut : **Mainteneur (MiiK4L)** — sera affiné si des mainteneurs par
> domaine émergent.

## Tableau des risques

Deux vues d'un même registre : **évaluation** puis **suivi**.

### Évaluation

| ID | Risque | Probabilité | Impact | Déclencheur (signal d'occurrence) | Lot(s) | Statut |
| --- | --- | --- | --- | --- | --- | --- |
| R1 | Budget GPIO insuffisant sur le candidat de référence | Moyen | Élevé | Décompte GPIO > broches exposées, même après options | L1–L2 | Ouvert |
| R2 | Contention du bus SPI entre écran et CX-Bus | Moyen | Élevé | Glitch écran ou corruption bus lors d'un accès simultané | L2–L4 | Ouvert |
| R3 | Deep-sleep du candidat de référence > cible — peut rouvrir le choix du cœur | Moyen | Très élevé | Courant deep-sleep mesuré > seuil dérivé de la cible L6 | L5–L6 | Ouvert |
| R4 | Autonomie incompatible avec un écran couleur | Élevé | Élevé | Budget énergétique mesuré < cible d'usage | L4–L6 | Ouvert |
| R5 | Sûreté du hot-plug non démontrable | Moyen | Élevé | Corruption/instabilité reproductible à l'insertion/retrait | L2 | Ouvert |
| R6 | Lecture du Manifest sans alimenter le module | Moyen | Moyen | Impossible d'isoler l'identification du reste du module | L3 | Ouvert |
| R7 | Installation dynamique de scripts infaisable | Moyen | Moyen | RAM/flash par app hors budget ou chargement non démontré | L8 | Ouvert |
| R8 | Disponibilité / obsolescence d'un composant | Moyen | Moyen | Composant NRND/EOL ou stock nul chez ≥ 2 distributeurs | tous | Ouvert |
| R9 | Intégration mécanique hors format « poche » | Moyen | Moyen | Enveloppe dépassant la cible de forme | L1–L5 | Ouvert |

### Suivi

| ID | Propriétaire | Échéance de revue | Mitigation envisagée | Risque résiduel | Décision d'acceptation |
| --- | --- | --- | --- | --- | --- |
| R1 | Mainteneur | fin L1 / L2 | Décompte formel ; **arbitrage** (module alt., custom, expander/partage bus, réduction périmètre, optim. logicielle) | à évaluer | à décider en L9 |
| R2 | Mainteneur | fin L2 | Cartographier accès SPI ; CS dédiés / arbitrage ; SPI module optionnel | à évaluer | à décider en L9 |
| R3 | Mainteneur | fin L5 | Mesurer deep-sleep ; si > cible, **arbitrage** pouvant rouvrir [ADR-0004](../adr/0004-coeur-de-calcul-socket.md) | à évaluer | à décider (structurant) |
| R4 | Mainteneur | fin L6 | Mesures conso ; veille écran agressive ; cible d'autonomie arbitrée en L6 | à évaluer | à décider en L6/L9 |
| R5 | Mainteneur | fin L2 | Isolation/commutation bus ; protections ; banc hot-plug (seuils reproductibles) | à évaluer | à décider en L9 |
| R6 | Mainteneur | fin L3 | Rail d'identification isolé/limité ; comparer ≥ 2 architectures | à évaluer | à décider en L9 |
| R7 | Mainteneur | fin L8 | Chiffrer RAM/flash ; repli sur modèle statique / bundle OTA | à évaluer | à décider en L8/L9 |
| R8 | Mainteneur | continue | ≥ 2 sources par composant critique ; veille cycle de vie | à évaluer | à décider en L9 |
| R9 | Mainteneur | fin L1 / L5 | Enveloppe mécanique tôt ; contraintes de forme au dimensionnement | à évaluer | à décider en L9 |

## Gouvernance des risques

Chaque risque suit un cycle explicite et traçable :

1. **Ouvert** → **En analyse** : dès qu'un lot engage la mesure/prototype qui qualifie le
   risque (déclencheur surveillé).
2. **En analyse** → **Mitigé** : une action est retenue et **vérifiée** (isolation de bus,
   arbitrage cœur, séquencement d'alimentation, repli fonctionnel…).
3. **Accepté** : le risque résiduel est **jugé acceptable** et **explicitement documenté**
   (décision d'acceptation tracée) — utile quand la mitigation complète est hors périmètre
   Phase 1.
4. **Clos** : la mesure démontre que le risque ne peut plus se matérialiser dans le périmètre
   de Phase 1.

Certains risques **remontent une décision structurante** au lieu d'être simplement mitigés :
c'est notamment le cas de **R3**, dont l'atteinte du déclencheur associé lance un arbitrage
pouvant **rouvrir [ADR-0004](../adr/0004-coeur-de-calcul-socket.md)** (voir les
[déclencheurs d'arbitrage du cœur](./requirements-matrix.md)).

**Lien avec les critères de sortie de Phase 1 :** la Phase 1 ne peut être clôturée tant
qu'il subsiste un risque de statut **Ouvert** dont l'impact est jugé bloquant. Chaque
risque doit être ramené à **Mitigé** ou **Clos**, ou explicitement accepté et documenté,
avant de valider la sortie de phase.

## Renvois

- [Matrice initiale des exigences](./requirements-matrix.md)
- [Plan de Phase 1](./plan.md)
- [ADR-0004 — Cœur de calcul remplaçable](../adr/0004-coeur-de-calcul-socket.md)

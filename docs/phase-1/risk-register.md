<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Phase 1 — Registre initial des risques

> Registre **initial et vivant** : il est mis à jour à **chaque lot** du
> [plan de Phase 1](./plan.md). Aucun risque n'est figé, aucune mitigation n'est
> présélectionnée. Les candidats technologiques évoqués restent **Proposés**.
>
> Statuts possibles : **Ouvert** / **Surveillé** / **Mitigé** / **Clos**.

## Tableau des risques

| ID | Risque | Probabilité | Impact | Mitigation | Lot(s) | Statut |
| --- | --- | --- | --- | --- | --- | --- |
| R1 | Budget GPIO insuffisant sur le candidat de référence | Moyen | Élevé | Décompte formel du budget GPIO ; évaluer expander I²C + partage SPI ; sinon remonter vers module alternatif ou carte custom | L1–L2 | Ouvert |
| R2 | Contention du bus SPI entre écran et CX-Bus | Moyen | Élevé | Cartographier les accès SPI ; arbitrage / CS dédiés / horloges ; prototype de charge | L2–L4 | Ouvert |
| R3 | Deep-sleep du XIAO supérieur à la cible — peut rouvrir le choix du cœur | Moyen | Très élevé | Mesurer le deep-sleep système ; confronter à la cible L6 ; critère de rejet pouvant rouvrir [ADR-0004](../adr/0004-coeur-de-calcul-socket.md) | L5–L6 | Ouvert |
| R4 | Autonomie incompatible avec un écran couleur | Élevé | Élevé | Mesurer la conso écran allumé/éteint ; stratégies de veille écran ; arbitrer cible d'autonomie en L6 | L4–L6 | Ouvert |
| R5 | Sûreté du hot-plug non démontrable | Moyen | Élevé | Étudier détection présence + séquencement enable alim ; protection électrique ; banc de test hot-plug | L2 | Ouvert |
| R6 | Lecture du Manifest sans alimenter le module | Moyen | Moyen | Définir le mécanisme d'accès au Manifest hors alimentation module ; prototype de lecture | L3 | Ouvert |
| R7 | Installation dynamique de scripts Lua infaisable | Moyen | Moyen | Vérifier faisabilité runtime scripté (RAM/flash, chargement dynamique) ; repli sur modèle statique | L8 | Ouvert |
| R8 | Disponibilité / obsolescence d'un composant | Moyen | Moyen | Veille approvisionnement ; identifier seconds sources ; critère de pérennité | tous | Ouvert |
| R9 | Intégration mécanique hors format « poche » | Moyen | Moyen | Maquette d'encombrement ; contraintes de forme dès le dimensionnement | L1–L5 | Ouvert |

## Gouvernance des risques

Chaque risque suit un cycle explicite et traçable :

1. **Mesure / prototype** — un lot produit la donnée qui qualifie le risque
   (mesure de consommation, essai de charge SPI, banc hot-plug, etc.). Le risque passe
   d'**Ouvert** à **Surveillé** dès qu'une mesure est engagée.
2. **Mitigation** — la mesure oriente une action (partage de bus, expander, séquencement
   d'alimentation, repli fonctionnel…). Une fois l'action retenue et sa vérification faite,
   le risque passe à **Mitigé**.
3. **Clôture** — le risque devient **Clos** quand la mesure démontre qu'il ne peut plus se
   matérialiser dans le périmètre de Phase 1.

Certains risques **remontent une décision structurante** au lieu d'être simplement mitigés :
c'est notamment le cas de **R3**, dont l'atteinte du critère de rejet associé peut
**rouvrir [ADR-0004](../adr/0004-coeur-de-calcul-socket.md)** (voir les
[critères de rejet](./requirements-matrix.md) (section « Critères de rejet du XIAO »)).

**Lien avec les critères de sortie de Phase 1 :** la Phase 1 ne peut être clôturée tant
qu'il subsiste un risque de statut **Ouvert** dont l'impact est jugé bloquant. Chaque
risque doit être ramené à **Mitigé** ou **Clos**, ou explicitement accepté et documenté,
avant de valider la sortie de phase.

## Renvois

- [Matrice initiale des exigences](./requirements-matrix.md)
- [Plan de Phase 1](./plan.md)
- [ADR-0004 — Cœur de calcul remplaçable](../adr/0004-coeur-de-calcul-socket.md)

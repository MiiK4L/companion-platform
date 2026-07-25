<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 9 — Hors périmètre

> Le Lot 9 est un **checkpoint documentaire** : il clôt le **volet documentaire**
> (L1–L8) et prépare les campagnes de mesure. Ce document énumère explicitement ce
> qu'il **ne fait pas**, afin d'éviter toute lecture erronée de la synthèse ou de
> la spécification consolidée.

## Explicitement exclu

| Exclu | Pourquoi | Où cela se fera |
|-------|----------|-----------------|
| **Clore la Phase 1** | Le volet **expérimental** (mesures, ADR) reste ouvert ; ce lot ne clôt que le volet **documentaire**. | Volet expérimental de la Phase 1. |
| **Arbitrer une `DEC-*`** | Aucun arbitrage sans preuve reproductible ([cadre §3](../validation-framework.md)). | Campagnes de mesure (Phase 1), lot par lot. |
| **Créer ou accepter une ADR 0013+** | L'ADR est créée **après** les mesures ; aucune mesure exécutée. | Campagnes de mesure (Phase 1), dans la PR du lot. |
| **Retenir un composant / figer un MPN** | Les séries restent **candidates** ; les composants **illustrent**, ne décident pas. | Qualification par protocole (Phase 1 expérimentale). |
| **Exécuter une mesure / un prototype** | Le **volet documentaire** est terminé ; les protocoles sont **rédigés, non exécutés**. | Campagnes de mesure (bancs à monter). |
| **Promouvoir la spécification en `rc.1`** | La promotion exige des mesures et une cohérence démontrée. | Sortie de la Phase 1 (après campagnes). |
| **Geler un brochage / une API** | Brochage logique = contrainte **SPEC révisable** ; signatures de ports = **candidates**. | Consolidation ultérieure. |
| **Décider la production** | Thermique, CEM, DFM/DFT, qualification batterie, mécanique finale. | Phases ultérieures. |
| **Implémenter le firmware** | Le squelette host L8 est une **preuve d'architecture**, pas une implémentation. | Adaptateurs cible (ultérieur). |

## Ce que le Lot 9 fait, en revanche

- **Consolide** l'état des 17 `DEC-*` (toutes Ouvertes) — sans les trancher.
- **Ordonne** les arbitrages à venir ([feuille de route](decision-roadmap.md)).
- **Évalue objectivement** la préparation des campagnes de mesure ([readiness](campaign-readiness.md)).
- **Rassemble** l'acquis stable et les contraintes ouvertes ([spécification consolidée](consolidated-specification.md)).

## Renvois

- [Vue d'ensemble du Lot 9](README.md)
- [Cadre de validation & modèle de preuves](../validation-framework.md)

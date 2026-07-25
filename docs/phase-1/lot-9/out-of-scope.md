<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 9 — Hors périmètre

> Le Lot 9 est un lot de **clôture documentaire**. Ce document énumère
> explicitement ce qu'il **ne fait pas**, afin d'éviter toute lecture erronée de
> la synthèse ou de la spécification consolidée.

## Explicitement exclu

| Exclu | Pourquoi | Où cela se fera |
|-------|----------|-----------------|
| **Arbitrer une `DEC-*`** | Aucun arbitrage sans preuve reproductible ([cadre §3](../validation-framework.md)). | Phase 2 (mesure), lot par lot. |
| **Créer ou accepter une ADR 0013+** | L'ADR est créée **après** les mesures ; aucune mesure exécutée. | Phase 2, dans la PR du lot correspondant. |
| **Retenir un composant / figer un MPN** | Les séries restent **candidates** ; les composants **illustrent**, ne décident pas. | Phase 2 (qualification par protocole). |
| **Exécuter une mesure / un prototype** | La Phase 1 est documentaire ; les protocoles sont **rédigés, non exécutés**. | Phase 2 (bancs à monter). |
| **Promouvoir la spécification en `rc.1`** | La promotion exige des mesures et une cohérence démontrée. | Phase 2 (sortie de campagne). |
| **Geler un brochage / une API** | Brochage logique = contrainte **SPEC révisable** ; signatures de ports = **candidates**. | Phase 2+ (consolidation). |
| **Décider la production** | Thermique, CEM, DFM/DFT, qualification batterie, mécanique finale. | Phases ultérieures. |
| **Implémenter le firmware** | Le squelette host L8 est une **preuve d'architecture**, pas une implémentation. | Phase 2+ (adaptateurs cible). |

## Ce que le Lot 9 fait, en revanche

- **Consolide** l'état des 17 `DEC-*` (toutes Ouvertes) — sans les trancher.
- **Ordonne** les arbitrages à venir ([feuille de route](decision-roadmap.md)).
- **Évalue objectivement** la préparation à la Phase 2 ([readiness](phase-2-readiness.md)).
- **Rassemble** l'acquis stable et les contraintes ouvertes ([spécification consolidée](consolidated-specification.md)).

## Renvois

- [Vue d'ensemble du Lot 9](README.md)
- [Cadre de validation & modèle de preuves](../validation-framework.md)

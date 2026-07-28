<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Socle expérimental — Hors périmètre

> Ce document énumère ce que le socle **ne fait pas**, pour éviter toute lecture
> erronée (notamment prendre la simulation pour une preuve).

## Explicitement exclu

| Exclu | Pourquoi | Où cela se fera |
|-------|----------|-----------------|
| **Exécuter une mesure réelle** | Le socle est l'**infrastructure** ; aucune campagne n'est lancée ici. | Campagnes de mesure (Phase 1), après validation de ce socle. |
| **Fournir un pilote d'instrument réel** | Seuls l'**interface**, les **stubs** et le **driver de simulation** sont fournis. | Adaptateurs ajoutés progressivement, au fil des campagnes. |
| **Retenir / figer un instrument** | Les modèles cités sont des **exemples** ; l'inventaire réel est renseigné plus tard. | [reference-bench](reference-bench.md) (inventaire, renseigné à l'usage). |
| **Produire une donnée `[M]` en simulation** | **Une simulation teste l'outil, jamais une décision matérielle.** | Mode réel uniquement (driver d'instrument). |
| **Alimenter une `DEC-*` / une ADR** | Le socle ne **crée** ni ne **tranche** aucune décision. | Campagnes réelles → rapports → ADR (cf. [adr-plan](../adr-plan.md)). |
| **Dupliquer un protocole / un résultat** | Les protocoles et résultats **font autorité** dans leur lot. | Le socle **exécute** les protocoles ; il n'en recopie pas le contenu. |
| **Choisir un composant produit** | Hors périmètre du socle (méthodologie/outillage). | Qualification par protocole (campagnes). |
| **Imposer un framework de test** | `pytest`/linter/formateur sont des **choix d'outillage interne**, pas l'architecture des campagnes. | Le cœur des formats reste **indépendant** du framework de test. |

## Règle explicite — simulation

> **Les campagnes simulées valident uniquement l'infrastructure d'outillage. Elles
> ne produisent aucune donnée `[M]`, n'alimentent aucune ADR et ne peuvent
> satisfaire aucun critère expérimental.**

## Renvois

- [Vue d'ensemble](README.md) · [Architecture](architecture.md)
- [Cadre de validation](../validation-framework.md)

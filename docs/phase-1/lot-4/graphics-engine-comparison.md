<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 4 — Moteur graphique (*Proposé*, `DEC-L4-002`)

> **Statut : Ouvert / Proposé.** **Deux couches nettement séparées** (jamais
> mélangées) : **moteur UI** et **HAL / pilotage**. Le moteur **ne dépend d'une
> dalle que par ses capacités exposées**. Valeurs `[DS]/[H]/[BL]` ; **aucune
> `[M]`**. → **ADR future (n° non réservé)**.

## Quatre catégories logicielles distinctes (à ne jamais confondre)

| Catégorie | Rôle | Exemples | Décision |
|-----------|------|----------|----------|
| **Framework UI** | Widgets, layout, événements (**inclut** un moteur de rendu) | **LVGL** | **`DEC-L4-002`** |
| **Moteur de rendu** | Cœur de rasterisation / composition | intégré à LVGL, **ou moteur maison** | **`DEC-L4-002`** |
| **Bibliothèque graphique** | Primitives de dessin **+ pilotes de contrôleurs** | **LovyanGFX**, **TFT_eSPI** | **adaptateur** (hors `DEC-L4-002`) |
| **Pilote matériel** | Pilote de contrôleur bas niveau | **esp_lcd (ESP-IDF)** | **adaptateur** (hors `DEC-L4-002`) |

> **Distinctions strictes** : un **framework UI** (LVGL) **n'est pas** une
> **bibliothèque graphique** (LovyanGFX). Les catégories **bibliothèque graphique**
> et **pilote matériel** sont des **adaptateurs** derrière l'abstraction
> ([display-abstraction](display-abstraction.md)) : elles peuvent changer **sans
> rouvrir** `DEC-L4-002`.

## 1. Framework UI + moteur de rendu — comparaison

| Critère | LVGL | Moteur maison | (autre) |
|---------|------|---------------|---------|
| Maturité / widgets | riche **[H]** | à développer | selon |
| Empreinte RAM/flash | dépend config **[DS→[BL]]** | maîtrisable | selon |
| Stratégie de rendu | **partiel / direct / double-triple buffer** **[DS]** | à concevoir | selon |
| Portabilité derrière l'abstraction | oui (interface d'affichage) **[H]** | oui | selon |
| Coût d'intégration / maintenance | moyen **[H]** | **élevé** | selon |

### Stratégies mémoire du moteur (exploitent les contraintes matérielles)

| Stratégie | Principe | Mémoire | Remarque |
|-----------|----------|---------|----------|
| **Buffer partiel** | buffer < écran (≥ 1/10 recommandé), redraw des zones invalides (**dirty rectangles**) | faible | + de passes ; adapté RAM limitée **[DS]** |
| **Rendu direct** (full) | buffer = taille écran | élevée (framebuffer plein) | 1 image complète en RAM **[DS]** |
| **Double / triple buffer** | rendu pendant l'envoi DMA de l'autre | 2–3× buffer | parallélise rendu/refresh **[DS]** |
| **Rendu par tuiles** | découpage en tuiles | faible | selon moteur |

> Le **choix de stratégie** relève de `DEC-L4-002` ; il **exploite** les
> **contraintes imposées par le matériel** (`DEC-L4-001`, voir
> [budget mémoire](memory-and-fps-budget.md)).

## 2. Bibliothèque graphique & pilote matériel — comparaison (adaptateurs, hors décision moteur)

| Critère | LovyanGFX *(biblio. graphique)* | TFT_eSPI *(biblio. graphique)* | esp_lcd *(pilote matériel IDF)* |
|---------|--------------------------------|--------------------------------|---------------------------------|
| Contrôleurs supportés | large **[H]** | large **[H]** | via pilotes IDF **[H]** |
| DMA / performance | oui **[H]** | oui **[H]** | oui (natif IDF) **[H]** |
| Intégration ESP-IDF | bonne **[H]** | bonne **[H]** | **native** **[H]** |
| Statut (adaptateur) | révisable | révisable | révisable |

> Ces catégories sont **distinctes** du **framework UI** (§1) et **n'entrent pas**
> dans `DEC-L4-002`.

## Alimente

- **`DEC-L4-002`** — moteur UI + **stratégie mémoire** + pipeline, arbitré **après**
  mesures (RAM/FPS). La **couche HAL/pilotage** reste un **adaptateur** distinct.
  **Aucun moteur retenu.**

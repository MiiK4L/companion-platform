<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 4 — Moteur graphique (*Proposé*, `DEC-L4-002`)

> **Statut : Ouvert / Proposé.** **Deux couches nettement séparées** (jamais
> mélangées) : **moteur UI** et **HAL / pilotage**. Le moteur **ne dépend d'une
> dalle que par ses capacités exposées**. Valeurs `[DS]/[H]/[BL]` ; **aucune
> `[M]`**. → **ADR future (n° non réservé)**.

## Deux couches distinctes

| Couche | Rôle | Exemples | Décision |
|--------|------|----------|----------|
| **Moteur UI** | Widgets, rendu, animations, **stratégie mémoire** | **LVGL**, **moteur maison**, autres | **`DEC-L4-002`** |
| **HAL / pilotage** | Traduit le rendu vers le contrôleur | **LovyanGFX**, **TFT_eSPI**, **esp_lcd (ESP-IDF)** | **adaptateur** (révisable, hors `DEC-L4-002`) |

> La **couche HAL/pilotage** est un **adaptateur** derrière l'abstraction
> ([display-abstraction](display-abstraction.md)) : elle peut changer sans rouvrir
> le choix de moteur UI.

## 1. Couche moteur UI — comparaison

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

## 2. Couche HAL / pilotage — comparaison (adaptateur, hors décision moteur)

| Critère | LovyanGFX | TFT_eSPI | esp_lcd (ESP-IDF) |
|---------|-----------|----------|-------------------|
| Contrôleurs supportés | large **[H]** | large **[H]** | via pilotes IDF **[H]** |
| DMA / performance | oui **[H]** | oui **[H]** | oui (natif IDF) **[H]** |
| Intégration ESP-IDF | bonne **[H]** | bonne **[H]** | **native** **[H]** |
| Statut (adaptateur) | révisable | révisable | révisable |

## Alimente

- **`DEC-L4-002`** — moteur UI + **stratégie mémoire** + pipeline, arbitré **après**
  mesures (RAM/FPS). La **couche HAL/pilotage** reste un **adaptateur** distinct.
  **Aucun moteur retenu.**

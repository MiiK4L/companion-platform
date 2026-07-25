<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 4 — Couches d'affichage (Display HAL → Graphics Engine → UI) — *sans arbitrage*

> **Statut : descriptif — aucun arbitrage.** Ce document **situe** les couches et
> **où portent** `DEC-L4-001` et `DEC-L4-002`, **sans** choisir de techno. Reprend
> l'abstraction déjà actée ([ADR-0003](../../adr/0003-technologie-et-abstraction-ecran.md),
> [ADR-0007](../../adr/0007-hal-et-companion-sdk.md)).

## Empilement (haut = applicatif, bas = silicium)

```
Applications UI            ← apps ; ne connaissent que l'abstraction
        │
Graphics Engine (moteur UI)   ← rendu, widgets, animations (DEC-L4-002)
        │
Display HAL / pilotage        ← pilote/bibliothèque (LovyanGFX, TFT_eSPI,
        │                       esp_lcd…) ; adaptateur derrière l'abstraction
Contrôleur d'affichage        ← ST7789 / ILI9341 / SSD1351 / Memory LCD…
        │
Interface physique            ← SPI / QSPI / RGB parallèle (DEC-L4-001, axe distinct)
        │
Dalle (technologie)           ← TFT IPS / Memory LCD / OLED / transflectif
```

## Rôle et propriété de décision par couche

| Couche | Rôle | Relève de |
|--------|------|-----------|
| **Applications UI** | Utilisent l'API abstraite ; **indépendantes** du matériel | (abstraction actée) |
| **Graphics Engine (moteur UI)** | Rendu, widgets, animations, **stratégie mémoire** | **`DEC-L4-002`** |
| **Display HAL / pilotage** | Traduit le rendu vers le contrôleur (bibliothèque/pilote) | **adaptateur** (révisable, **hors** décision de moteur UI) |
| **Contrôleur d'affichage** | Pilote la dalle (registres, GDDRAM) | **`DEC-L4-001`** |
| **Interface physique** | SPI / QSPI / RGB — **axe distinct** du contrôleur | **`DEC-L4-001`** |
| **Dalle (technologie)** | TFT/OLED/Memory LCD/transflectif | **`DEC-L4-001`** |

## Règles de séparation (à respecter)

- Le **moteur UI** (`DEC-L4-002`) **ne dépend pas** d'une dalle précise — seulement
  des **capacités exposées** (résolution, profondeur, contraintes mémoire).
- La **couche HAL/pilotage** est un **adaptateur** : elle **n'est pas** mélangée au
  choix de moteur UI ; elle peut évoluer (LovyanGFX ↔ esp_lcd…) sans rouvrir
  `DEC-L4-002`.
- Le **matériel** (`DEC-L4-001`) **impose** des contraintes (voir
  [budget mémoire](memory-and-fps-budget.md)) ; **il ne décide pas** de la
  stratégie de rendu.

> Aucun choix ici : ce document **cadre**, les comparaisons et protocoles font le reste.

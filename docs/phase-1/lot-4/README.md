<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 4 — Écran & moteur graphique

> **Statut : en cours (exécution documentaire).** Comparaison **derrière
> l'abstraction d'affichage déjà actée** ([ADR-0003](../../adr/0003-technologie-et-abstraction-ecran.md),
> [ADR-0007](../../adr/0007-hal-et-companion-sdk.md)). **Aucune techno privilégiée**
> (TFT IPS, LVGL, ST7789, une résolution = **hypothèses, non figées**). **Aucune
> ADR**, **rien en Accepté**, **aucune mesure inventée**, **aucun prototype
> exécuté**. Voir le [plan](../plan.md) (Lot 4) et le
> [cadre de validation](../validation-framework.md).

## Deux décisions séparées & indépendantes (*Ouvert / Proposé*)

- **`DEC-L4-001` — matériel d'affichage** : **famille de dalle**, **contrôleur**,
  **interface physique** (distincte du contrôleur), **format/résolution**, et les
  **contraintes mémoire imposées par le matériel**. → **ADR-0017**. **Aucune
  hypothèse de moteur graphique.**
- **`DEC-L4-002` — moteur graphique** : **architecture de rendu**, **stratégie
  mémoire** (buffers, dirty rectangles, rendu par tuiles…), **budget mémoire
  exploité**, **pipeline**. → **ADR future (n° attribué à la création, non
  réservé)**. **Ne dépend d'une dalle que par les capacités qu'elle expose.**

> Indépendance stricte (comme L2/L3) : changer de contrôleur n'oblige pas à
> rechanger de moteur, et inversement. La séparation des couches est décrite dans
> [display-abstraction](display-abstraction.md).

## Distinctions structurantes (ajustements de cadrage)

- **Interface physique ≠ contrôleur** : SPI / QSPI / RGB parallèle / … est un
  **axe distinct** du contrôleur d'affichage (`DEC-L4-001`).
- **Contrainte mémoire matérielle ≠ stratégie mémoire moteur** : le matériel
  **impose** des contraintes (framebuffer, profondeur de couleur, DMA →
  `DEC-L4-001`) ; le moteur **décide comment les exploiter** (buffers, dirty
  rectangles, tuiles → `DEC-L4-002`).
- **Format** (rond / carré / rectangulaire) = **critère de comparaison**, pas une
  technologie (ex. GC9A01 est **un contrôleur TFT SPI**, pas une famille).

## Terminologie & conventions

- **Série représentative documentaire** : définition unique en
  [Lot 2B](../lot-2b/README.md) (série réelle citée pour établir des plages,
  **jamais retenue**, aucun MPN final) — **≥ 2 séries sourcées par famille**.
- **Statut des valeurs** : `[DS]` · `[C]` · `[H]` · `[BL]` · `N/A` · `inconnu` ·
  `[M]` (**aucune `[M]`**).

## Livrables (tous *Proposé*)

| Livrable | Décision | Contenu |
|----------|----------|---------|
| [Couches d'affichage](display-abstraction.md) | — | Display HAL → Graphics Engine → Applications UI (sans arbitrage) |
| [Familles d'affichage](display-families-comparison.md) | L4-001 | TFT SPI couleur · Memory LCD · OLED couleur · **transflectif** ; interface & format en critères |
| [Moteur graphique](graphics-engine-comparison.md) | L4-002 | Moteur UI **et** HAL/pilotage **séparés** ; stratégies de buffers |
| [Budget mémoire & FPS](memory-and-fps-budget.md) | L4-001/002 | Contraintes matérielles (L4-001) vs stratégie moteur (L4-002) ; ≈ 30 fps |
| [Énergie & rétroéclairage](power-and-backlight.md) | L4-001 | Conso par niveau ; extinction auto + reprise (↔ L6) |
| [Partage SPI](spi-sharing.md) | L4-001 | SPI/QSPI partagé écran ↔ CX-Bus (R2, ↔ L2) |
| [Composants candidats](candidate-components.md) | L4-001/002 | Séries représentatives documentaires sourcées (≥ 2/famille) |
| [Protocoles](protocols/README.md) | L4-001/002 | Génériques typés : RAM, FPS, **performances perçues**, conso, contention SPI |

## Ce que ce lot NE fait pas

- Ne **retient** aucune techno/contrôleur/moteur, ne **fige** aucun format, ne
  crée **aucune ADR**.
- **Sélectionne** des options à prototyper **sans les arbitrer** ; prototypes et
  mesures en **PR/campagne ultérieure**. **Aucun `Accepté`** sur documentaire seul.

## Critères de sortie

- Couches d'affichage **décrites** ; interface physique **distinguée** du contrôleur.
- Familles (dont **transflectif**) comparées ; **aucun gagnant**.
- Moteur UI **et** HAL/pilotage **séparés** ; contraintes matérielles vs stratégie
  moteur **distinguées**.
- Protocoles génériques (dont **performances perçues**) en Brouillon ; `DEC-L4-001`
  et `DEC-L4-002` **ouverts** ; options à prototyper **listées, non arbitrées**.

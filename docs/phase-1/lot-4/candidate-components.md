<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 4 — Composants candidats (séries représentatives documentaires, *Proposé*)

> **Statut : Ouvert / Proposé — PRÉ-SHORTLIST DOCUMENTAIRE.** **≥ 2 séries
> sourcées par famille** quand le marché le permet (sinon mono-source documenté) ;
> définition en [Lot 2B](../lot-2b/README.md). **Aucune retenue, aucun MPN
> final.** `[DS]` = fiche (sources ci-dessous) ; `à relever`/`inconnu` sinon ;
> **aucune `[M]`**. Statut de caractérisation par ligne. Caractéristiques
> susceptibles d'évoluer ; essais sur la **révision exacte** testée.

## Types de séries citées (à ne pas confondre)

Chaque série indique **explicitement ce qu'elle représente** :

- **Contrôleur (driver IC)** — puce de pilotage (ex. ST7789, SSD1351) ; le
  **module/dalle** associé est un **choix distinct**.
- **Module écran commercial** — ensemble dalle + contrôleur intégré + connectique
  (ex. modules Sharp Memory LCD).
- **Dalle** — l'élément d'affichage seul (technologie), généralement sourcé via un
  module.

## 1. Contrôleurs TFT couleur (SPI) — *type : contrôleur (driver IC)*

| Contrôleur | Fab. | Résolution typique | Interface | Couleur | Format | Source | Caractérisation |
|-----------|------|--------------------|-----------|---------|--------|--------|-----------------|
| **ST7789(V)** | Sitronix | 240×240 / 240×320 (IPS) **[DS]** | SPI, 3,3 V **[DS]** | 16/18 bit | carré/rect | Sitronix ST7789V | partielle |
| **ILI9341** | Ilitek | 240×320 (QVGA) **[DS]** | SPI 3/4-fils + parallèle + RGB **[DS]** | 262K **[DS]** | rect | Ilitek ILI9341 | partielle |
| **GC9A01** | Galaxycore | 240×240 (**rond**) **[DS]** | SPI ≤ 40 MHz **[DS]** | couleur | **rond** | Galaxycore GC9A01 | partielle |

## 2. Memory LCD (réflectif mono) — *type : module écran commercial* — ≈ mono-fabricant (Sharp)

| Réf. | Fab. | Résolution | Interface | Conso statique | Rétroéclairage | Source | Caractérisation |
|------|------|-----------|-----------|----------------|----------------|--------|-----------------|
| **LS013B7DH03** | Sharp | 128×128 (1,28") **[DS]** | 4-fils SPI, 3 V **[DS]** | **~60 µW** **[DS]** | aucun (réflectif) | Sharp LS013B7DH03 | partielle |
| **LS027B7DH01** | Sharp | 400×240 (2,7") **[DS]** | SPI **[DS]** | **~50 µW** stat. **[DS]** | aucun (réflectif) | Sharp LS027B7DH01 | partielle |

> **Statut des propriétés (ne pas généraliser à « tout Memory LCD »)** :
>
> - **Documenté par datasheet [DS]** : résolution, interface SPI, tension, conso
>   statique ~µW, technologie réflective (sans rétroéclairage).
> - **Spécifique à Sharp** : la technologie **Memory-in-Pixel** et la **rétention
>   d'image sans rafraîchissement continu** sont **propres à ces séries Sharp**,
>   **pas** une propriété générique de toute dalle dite « Memory LCD ».
> - **À valider expérimentalement** : **taux de rafraîchissement réel** et aptitude
>   à l'**animation**, **conso dynamique** en usage, **lisibilité** selon éclairage.
>
> Famille **≈ mono-fabricant (Sharp)** — contrainte de disponibilité (`R8`).

## 3. OLED couleur — *type : contrôleur (driver IC)*

| Contrôleur | Fab. | Résolution | Interface | Couleur | Source | Caractérisation |
|-----------|------|-----------|-----------|---------|--------|-----------------|
| **SSD1351** | Solomon Systech | 128×128 (GDDRAM embarquée) **[DS]** | SPI/parallèle **[DS]** | 262K **[DS]** | SSD1351 (rév. 1.5) | partielle |
| **SSD1331** | Solomon Systech | 96×64 **[DS]** | SPI/parallèle **[DS]** | 65K **[DS]** | SSD1331 (rév. 1.2) | partielle |

## 4. Transflectif (inclus, à sourcer)

| Réf. | Statut |
|------|--------|
| dalles TFT/Memory **transflectives** | **séries représentatives à sourcer** — inclus au titre du compromis lisibilité/conso, **non retenu** |

## 5. Logiciel graphique — quatre catégories distinctes (à ne pas confondre)

| Élément | Catégorie | Rôle | Source |
|---------|-----------|------|--------|
| **LVGL** | **framework UI** (inclut son moteur de rendu) | widgets, layout, événements, rendu ; buffers partiel/direct/double | docs LVGL (9.x) |
| **Moteur maison** | **moteur de rendu / framework** (à concevoir) | alternative maîtrisée | (à concevoir) |
| **LovyanGFX** | **bibliothèque graphique** (+ pilotes de contrôleurs, DMA) | primitives de dessin + drivers | LovyanGFX (dépôt) |
| **TFT_eSPI** | **bibliothèque graphique** (+ pilotes SPI) | primitives + drivers (Bodmer) | TFT_eSPI (dépôt) |
| **esp_lcd (ESP-IDF)** | **pilote matériel** | pilote de contrôleur natif IDF | ESP-IDF esp_lcd |

> Catégories **distinctes** (cf. [moteur graphique](graphics-engine-comparison.md)) :
> **framework UI** ≠ **moteur de rendu** ≠ **bibliothèque graphique** ≠ **pilote
> matériel**. Un **framework UI** (LVGL) n'est pas une **bibliothèque graphique**
> (LovyanGFX).

## 6. Sources datasheet (traçabilité)

> Consultées le **2026-07-23** ; `[DS]` issues de ces documents ; références
> génériques/à sourcer **sans `[DS]`**. Statut de cycle de vie à confirmer au
> sourcing daté (`R8`).

| Réf. | Fabricant | Document | Date | Référence stable |
|------|-----------|----------|------|------------------|
| ST7789V | Sitronix | ST7789V datasheet | 2026-07-23 | `sitronix` (ST7789V) |
| ILI9341 | Ilitek | ILI9341 a-Si TFT driver | 2026-07-23 | `ilitek` (ILI9341) |
| GC9A01 | Galaxycore | GC9A01 datasheet | 2026-07-23 | `galaxycore` (GC9A01) |
| LS013B7DH03 | Sharp | Memory LCD 128×128 | 2026-07-23 | `sharpsde` (LS013B7DH03) |
| LS027B7DH01 | Sharp | Memory LCD 400×240 | 2026-07-23 | `sharpsde` (LS027B7DH01) |
| SSD1351 | Solomon Systech | OLED driver 128RGB×128 (rév. 1.5) | 2026-07-23 | `solomon-systech` (SSD1351) |
| SSD1331 | Solomon Systech | OLED driver 96RGB×64 (rév. 1.2) | 2026-07-23 | `solomon-systech` (SSD1331) |

## Alimente

- **`DEC-L4-001`** (matériel) / **`DEC-L4-002`** (moteur) — ces séries
  **alimentent partiellement** la comparaison ; **aucune retenue**, aucun MPN.
  Familles **mono-source** (Memory LCD Sharp) et **transflectif à sourcer**
  explicitement notées.

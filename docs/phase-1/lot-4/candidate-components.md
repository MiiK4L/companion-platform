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

## 1. Contrôleurs TFT couleur (SPI)

| Contrôleur | Fab. | Résolution typique | Interface | Couleur | Format | Source | Caractérisation |
|-----------|------|--------------------|-----------|---------|--------|--------|-----------------|
| **ST7789(V)** | Sitronix | 240×240 / 240×320 (IPS) **[DS]** | SPI, 3,3 V **[DS]** | 16/18 bit | carré/rect | Sitronix ST7789V | partielle |
| **ILI9341** | Ilitek | 240×320 (QVGA) **[DS]** | SPI 3/4-fils + parallèle + RGB **[DS]** | 262K **[DS]** | rect | Ilitek ILI9341 | partielle |
| **GC9A01** | Galaxycore | 240×240 (**rond**) **[DS]** | SPI ≤ 40 MHz **[DS]** | couleur | **rond** | Galaxycore GC9A01 | partielle |

## 2. Memory LCD (réflectif mono) — ≈ mono-fabricant (Sharp)

| Réf. | Fab. | Résolution | Interface | Conso statique | Rétroéclairage | Source | Caractérisation |
|------|------|-----------|-----------|----------------|----------------|--------|-----------------|
| **LS013B7DH03** | Sharp | 128×128 (1,28") **[DS]** | 4-fils SPI, 3 V **[DS]** | **~60 µW** **[DS]** | aucun (réflectif) | Sharp LS013B7DH03 | partielle |
| **LS027B7DH01** | Sharp | 400×240 (2,7") **[DS]** | SPI **[DS]** | **~50 µW** stat. **[DS]** | aucun (réflectif) | Sharp LS027B7DH01 | partielle |

> Famille **≈ mono-fabricant (Sharp)** — contrainte de disponibilité (`R8`).

## 3. OLED couleur

| Contrôleur | Fab. | Résolution | Interface | Couleur | Source | Caractérisation |
|-----------|------|-----------|-----------|---------|--------|-----------------|
| **SSD1351** | Solomon Systech | 128×128 (GDDRAM embarquée) **[DS]** | SPI/parallèle **[DS]** | 262K **[DS]** | SSD1351 (rév. 1.5) | partielle |
| *(2ᵉ série à sourcer)* | — | — | — | — | à sourcer | à compléter avant shortlist |

## 4. Transflectif (inclus, à sourcer)

| Réf. | Statut |
|------|--------|
| dalles TFT/Memory **transflectives** | **séries représentatives à sourcer** — inclus au titre du compromis lisibilité/conso, **non retenu** |

## 5. Moteur UI & HAL/pilotage (couches distinctes)

| Élément | Couche | Rôle | Source |
|---------|--------|------|--------|
| **LVGL** | moteur UI | widgets/rendu ; buffers partiel/direct/double | docs LVGL (9.x) |
| **Moteur maison** | moteur UI | alternative maîtrisée | (à concevoir) |
| **LovyanGFX** | HAL/pilotage | pilote multi-contrôleurs + DMA | LovyanGFX (dépôt) |
| **TFT_eSPI** | HAL/pilotage | pilote SPI (Bodmer) | TFT_eSPI (dépôt) |
| **esp_lcd (ESP-IDF)** | HAL/pilotage | pilote natif IDF | ESP-IDF esp_lcd |

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

## Alimente

- **`DEC-L4-001`** (matériel) / **`DEC-L4-002`** (moteur) — ces séries
  **alimentent partiellement** la comparaison ; **aucune retenue**, aucun MPN.
  Familles **mono-source** (Memory LCD Sharp) et **transflectif à sourcer**
  explicitement notées.

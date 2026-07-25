<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 7 — Composants candidats (séries représentatives documentaires, *Proposé*)

> **Statut : Ouvert / Proposé — PRÉ-SHORTLIST DOCUMENTAIRE.** Les composants
> **illustrent une solution** de source de temps, **ne définissent pas la
> fonction**. **≥ 2 séries sourcées** ; définition en [Lot 2B](../lot-2b/README.md).
> **Aucune retenue, aucun MPN final.** `[DS]` = fiche ; `à relever` sinon ;
> **aucune `[M]`**. Statut par ligne ; cycle de vie à confirmer au sourcing (`R8`).

## Sources de temps (RTC)

| Réf. | Fab. | Type | Précision | Conso | Sauvegarde | Source | Caractérisation |
|------|------|------|-----------|-------|------------|--------|-----------------|
| **PCF8563** | NXP | RTC **standard, quartz externe** (sans TCXO) | dépend du quartz (à relever) | faible **[DS]** | pile/supercap externe | NXP PCF8563 (Rev. 11.1) | partielle |
| **RV-3028-C7** | Micro Crystal | **module ULP, quartz intégré + calibration usine** (≠ TCXO) | **±3 ppm** **[DS]** | **45 nA** **[DS]** ; switchover + trickle | Micro Crystal RV-3028-C7 | partielle |
| **DS3231** | Analog Devices | **RTC compensé en température (TCXO)** | **±2 ppm** (0–40 °C) **[DS]** | plus élevée (TCXO) **[H]** | pile ; switchover | Analog DS3231 | partielle |
| **RTC interne** | (ESP32-S3) | RTC MCU (repli) | dérive élevée **[H]** | selon domaine | **non tenu** hors alim (sans domaine RTC) | fiche SoC | à compléter |

> Un **RTC interne** seul **ne tient pas l'heure** hors alimentation sans domaine
> RTC alimenté → **resync requise** (cf. [validité](timebase-comparison.md) /
> [resync](resync-strategy.md)).

## Persistance (rappel : décision `DEC-L7-002`, logiciel)

Support **NVS / LittleFS** (ESP-IDF) — pas de « série » matérielle ; comparaison
logicielle (cf. [persistance](persistence-comparison.md)).

## Sources datasheet (traçabilité)

> Consultées le **2026-07-23** ; `[DS]` issues de ces documents.

| Réf. | Fabricant | Titre du document | N° / révision | Date/rév. du doc | Consultée le | Référence stable |
|------|-----------|-------------------|---------------|------------------|--------------|------------------|
| PCF8563 | NXP | *PCF8563 Real-Time Clock/Calendar* | **Rev. 11.1** | **2026-01-19** | 2026-07-23 | `nxp.com/docs/en/data-sheet/PCF8563.pdf` |
| RV-3028-C7 | Micro Crystal | *RV-3028-C7 Extreme Low Power RTC Module* | rév. à relever | à relever | 2026-07-23 | `microcrystal.com` (RTC/Datasheet/RV-3028-C7.pdf) |
| DS3231 | Analog Devices | *DS3231 Extremely Accurate I²C RTC/TCXO/Crystal* | rév. à relever | à relever | 2026-07-23 | `analog.com/en/products/ds3231.html` |

## Alimente

- **`DEC-L7-001`** — ces séries **illustrent** les sources de temps ; **aucune
  retenue**, aucun MPN.

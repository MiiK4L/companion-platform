<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Brochage **candidat** du banc EXP-L1-BRINGUP-001

> **Statut : candidat, additif et révisable. RIEN N'EST FIGÉ.** Ce brochage sert
> à rendre la campagne exécutable ; il ne préempte ni `DEC-L1-001`, ni le
> brochage produit. Les numéros de broches sont **à confirmer sur la fiche
> officielle** de la révision réellement utilisée (cf.
> [budget GPIO/bus](../../lot-1/gpio-bus-budget.md)).
>
> Aucun composant ni MPN n'est retenu : « afficheur » et « module » désignent des
> **rôles de charge**, pas des références.

## Rôles

| Rôle | Carte | Fonction |
|------|-------|----------|
| Hôte | XIAO ESP32-S3 (véhicule de référence) | maître SPI, orchestrateur, lecture des compteurs |
| Module | RP2040 (cible **candidate** de référence) | simulateur de module CX-Bus, esclave SPI, émetteur d'IRQ |
| Afficheur | charge SPI de type afficheur | second périphérique du bus |

## Variante `spi-shared`

Un seul bus SPI, **chip-selects distincts**.

| Signal | Hôte | Module | Afficheur | Remarque |
|--------|------|--------|-----------|----------|
| `SCK` | D8 | ← | ← | ligne commune |
| `MOSI` | D10 | ← | ← | ligne commune |
| `MISO` | D9 | → | → | ligne commune |
| `CS_module` | D2 | ✓ | — | chip-select dédié |
| `CS_screen` | D3 | — | ✓ | chip-select dédié |
| `IRQ_module` | D1 | → | — | **sortie** du module, **entrée** de l'hôte |
| `GND` | GND | ✓ | ✓ | masse commune, point unique (BL-002) |
| `3V3` | 3V3 | ✓ | ✓ | dans les bornes de BL-001 |

## Variante `spi-separated`

Deux bus SPI distincts. Le second jeu de lignes est **à confirmer** selon les
broches réellement disponibles (le budget est tendu — cf.
[budget GPIO/bus §3](../../lot-1/gpio-bus-budget.md)).

| Signal | Hôte | Cible | Remarque |
|--------|------|-------|----------|
| `SCK_A` / `MOSI_A` / `MISO_A` | D8 / D10 / D9 | afficheur | bus A |
| `CS_screen` | D3 | afficheur | — |
| `SCK_B` / `MOSI_B` / `MISO_B` | **à confirmer** | module | bus B — disponibilité à vérifier |
| `CS_module` | D2 | module | — |
| `IRQ_module` | D1 | module → hôte | inchangé |
| `GND` / `3V3` | — | les deux | inchangé |

> **Point ouvert honnête** : la variante `spi-separated` peut ne pas être
> câblable directement sur le véhicule de référence si le budget de broches est
> insuffisant. Le cas échéant, B4 devra documenter le contournement retenu
> (réaffectation UART, expander, autre carte) **comme condition de validité du
> run**, et non le dissimuler.

## Points de mesure

Prévoir l'accès sonde sur : `SCK`, `CS_module`, `CS_screen`, `IRQ_module`, `GND`.
Ces points servent à corréler les horodatages logiciels aux transitions réelles
(invariant BL-005).

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

### Contrainte : `spi-separated` doit tenir sur le **même** DUT

La comparaison n'est **contrôlée** que si les deux variantes s'exécutent sur le
**même véhicule de référence**. La variante `spi-separated` exige donc qu'un
**second contrôleur SPI matériel** de l'ESP32-S3 soit affecté à des **broches
réellement exposées** de la XIAO (BL-012). Cette faisabilité doit être
**prouvée**, pas supposée : la carte n'expose que ~11 E/S et sa documentation
publique ne présente qu'une interface SPI standard.

**Contournements admis** — uniquement ceux qui préservent le DUT :

- **réaffectation de broches** (p. ex. libérer UART D6/D7) vers le second
  contrôleur SPI, si le mapping est possible et sans conflit.

**Contournements refusés** — ils invalideraient la campagne :

| Refusé | Raison |
|--------|--------|
| **GPIO expander** | ne peut pas porter les lignes d'un SPI à plusieurs MHz ; un expander I²C est hors de question pour `SCK`/`MOSI`/`MISO` |
| **Autre carte hôte** | changerait le **DUT** et détruirait le caractère **contrôlé** de la comparaison entre variantes |

> **Si le mapping s'avère impossible sur la même XIAO, la campagne doit être
> redéfinie** (p. ex. réduction du périmètre, autre question expérimentale) —
> **jamais contournée** avec un autre véhicule. Cette vérification est un
> prérequis **bloquant** du [gate matériel](hardware-gate.md).

## Points de mesure

| Signal | Niveau requis | Rôle |
|--------|---------------|------|
| `SCK`, `CS_module`, `CS_screen`, `IRQ_module` | capture temporelle **minimale** | latences, occupation du bus, appariement CS/IRQ |
| `MOSI`, `MISO` | capture **protocolaire** | décodage des trames, numéros de séquence, vérification de contenu |
| `SYNC` (marqueur GPIO) | les deux | **alignement** des traces logicielles et de la trace bus |
| `GND` | — | référence de sonde |

Le signal **`SYNC`** est une broche de l'hôte, réservée au **marqueur de
synchronisation** émis en début de run et périodiquement. Il matérialise la
**base de temps autoritaire** (BL-011) et permet l'alignement exigé par BL-005 ;
il ne participe à aucun protocole. Sa broche exacte est à arrêter avec le mapping
final (contrainte de budget d'E/S).

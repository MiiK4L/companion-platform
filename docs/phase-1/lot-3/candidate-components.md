<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 3 — Composants candidats (séries représentatives documentaires, *Proposé*)

> **Statut : Ouvert / Proposé.** **Séries représentatives documentaires** (voir la
> définition en [Lot 2B](../lot-2b/README.md)) : citées pour **établir des plages**
> et vérifier des solutions crédibles, **jamais retenues**. **Aucun MPN final.**
> Valeurs `[DS]` issues des fiches (sources ci-dessous), `inconnu`/« à relever »
> sinon ; **aucune `[M]`**.
>
> **Obsolescence** — caractéristiques documentaires susceptibles d'évoluer avec
> les révisions de datasheets ; les essais utiliseront la **révision exacte** des
> composants effectivement testés.

## 1. Architectures complètes d'identification

| Série (fab.) | Rôle | Points clés **[DS]** | Couche(s) | Source |
|--------------|------|----------------------|-----------|--------|
| **24AA02E64** (Microchip) | EEPROM I²C + **EUI-64** | 2 Kbit ; EUI-64 pré-programmé usine | Identité **instance native** | DS20002124 |
| **24AA64 / AT24C64** (Microchip) | EEPROM I²C simple | 64 Kbit ; sans EUI (à provisionner) | Description + identité **provisionnée** | DS21189 |
| **DS28E07** (Analog Devices) | EEPROM **1-Wire** | 1024 bit ; **ROM ID 64 bit** unique usine ; write-protect par page | Identité **instance native** + petite desc. | DS28E07 |
| **ATtiny** (Microchip) | **µC d'ID actif** | logique/état/protocole programmables (série générique) | Identité + desc. **générées** (firmware) | famille ATtiny |
| **ATECC608A/B** (Microchip) | **Secure Element** | I²C ≤ 1 Mbps ; ECC, clés/certificats, SHA-256 | **Évolutivité auth** (+ mém. desc.) | DS40001977 / DS40002239 |

## 2. Briques de stockage du Manifest (pas une identification)

| Série (fab.) | Rôle | Points clés **[DS]** | Source |
|--------------|------|----------------------|--------|
| **MB85RC256V** (Fujitsu) | **FRAM I²C** (description) | 256 Kbit ; endurance **≥ 10¹²** ; écriture rapide sans polling | Fujitsu DS501-00017 |
| Flash I²C/SPI dédiée (à sélectionner) | stockage Manifest | endurance ~10⁴–10⁵ **[H]** ; par pages | réf. à sélectionner |

> La FRAM/Flash **ne fournit pas d'identité** : elle est une **brique de
> description** (cf. [architectures](identification-architectures.md) §3).

## 3. Sources datasheet (traçabilité)

> Consultées le **2026-07-23** ; `[DS]` issues de ces documents, `inconnu` sinon.
> Références génériques (ATtiny, Flash à sélectionner) **sans `[DS]`** tant qu'une
> série précise n'est pas choisie.

| Série | Fabricant | Document / n° | Date | Référence stable |
|-------|-----------|---------------|------|------------------|
| 24AA02E64 | Microchip | *2-Kbit I²C EEPROM + EUI-48/64* — DS20002124 | 2026-07-23 | `microchip.com` (24AA02E64) |
| 24AA64 | Microchip | *64K I²C Serial EEPROM* — DS21189 | 2026-07-23 | `microchip.com` (24AA64) |
| DS28E07 | Analog Devices | *DS28E07 1024-Bit 1-Wire EEPROM* | 2026-07-23 | `analog.com` (DS28E07) |
| ATECC608A | Microchip | *ATECC608A CryptoAuthentication* — DS40001977 | 2026-07-23 | `microchip.com` (ATECC608A) |
| MB85RC256V | Fujitsu | *256K I²C FRAM* — DS501-00017 | 2026-07-23 | `fujitsu` (MB85RC256V) |

## Alimente

- **`DEC-L3-001`** (identité) et **`DEC-L3-002`** (Manifest) — ces séries **bornent**
  la comparaison ; **aucune n'est retenue**, aucun MPN final choisi à ce stade.

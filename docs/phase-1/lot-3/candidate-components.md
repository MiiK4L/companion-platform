<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 3 — Composants candidats (séries représentatives documentaires, *Proposé*)

> **Statut : Ouvert / Proposé.** **Séries représentatives documentaires** (voir la
> définition en [Lot 2B](../lot-2b/README.md)) : **≥ 2 par architecture** quand le
> marché le permet, sinon **mono-source documenté**. **Aucune retenue, aucun MPN
> final.** Valeurs `[DS]` (fiches, sources ci-dessous), `à relever`/`inconnu`
> sinon ; **aucune `[M]`**. Caractéristiques susceptibles d'évoluer avec les
> révisions ; les essais utiliseront la **révision exacte** testée.

Champs par série : fabricant · référence/série · tension · interface/adressage ·
capacité · courant (veille/lecture/écriture) · endurance/rétention · protections
d'écriture · température/boîtiers · **statut de cycle de vie** · source.

## 1. Architectures complètes d'identification

### EEPROM I²C + identité (EUI) — ≥ 2 séries
| Réf. | Fab. | Tension | Interface | Capacité | Courant (veille/L/É) | Endurance / rétention | Protection écriture | Temp. / boîtiers | Cycle de vie | Source |
|------|------|---------|-----------|----------|----------------------|-----------------------|---------------------|------------------|--------------|--------|
| **24AA02E64** | Microchip | 1,8–5,5 V (à confirmer) | I²C | 2 Kbit + **EUI-64** usine | à relever | ~10⁶ / longue **[H]** | bloc EUI **verrouillé** | à relever | actif (à confirmer) | DS20002124 |
| **AT24MAC402** | Microchip | **1,7–5,5 V** **[DS]** | I²C | 2 Kbit + **EUI-48/64 + série 128 bits** | à relever | ~10⁶ / longue **[H]** | bloc étendu **verrouillé** | SOIC8/TSSOP8/UDFN8/SOT23-5 **[DS]** | actif (à confirmer) | Atmel-8807 |

### EEPROM I²C simple (identité à provisionner) — catégorie multi-source
| Réf. | Fab. | Tension | Interface | Capacité | Endurance / rétention | Protection écriture | Cycle de vie | Source |
|------|------|---------|-----------|----------|-----------------------|---------------------|--------------|--------|
| **24AA64 / 24LC64** | Microchip | 1,7–5,5 V (variante) | I²C | 64 Kbit | ~10⁶ / longue **[H]** | WP broche | actif | DS21189 |
| **M24C64 / CAT24C64 …** | ST / Onsemi / … | selon réf. | I²C | 64 Kbit | à relever | WP broche | **catégorie richement multi-source** | fiches respectives |

> Le 64 Kbit I²C EEPROM est **fortement multi-fournisseur** (Microchip 24xx64,
> ST M24C64, Onsemi CAT24C64…) ; sans EUI, l'identité est **entièrement
> provisionnée** (cf. [provisioning](provisioning.md)).

### 1-Wire — ≥ 2 séries (fournisseur unique documenté)
| Réf. | Fab. | Interface | Capacité | Identité | Protection écriture | Cycle de vie | Source |
|------|------|-----------|----------|----------|---------------------|--------------|--------|
| **DS28E07** | Analog Devices | 1-Wire | 1024 bit | **ROM ID 64 bit** usine | WP par page / EPROM-emul. | actif | DS28E07 |
| **DS2431** | Analog Devices | 1-Wire | 1024 bit | **ROM ID 64 bit** laser | WP par page / EPROM-emul. | actif | DS2431 |

> Les EEPROM **1-Wire** sont **essentiellement mono-fournisseur (Analog/Maxim)** —
> **contrainte de disponibilité** à peser (`R8`).

### µC d'identification actif — séries concrètes (non retenues)
| Réf. | Fab. | Interface | Rôle | Remarque | Source |
|------|------|-----------|------|----------|--------|
| **ATtiny412 / ATtiny1616** (tinyAVR) | Microchip | I²C/UART (firmware) | ID + desc. **générées** | conso active ; firmware module ; specs à relever | fiches tinyAVR |
| **RP2040** | Raspberry Pi | I²C/… (firmware) | idem, plus de ressources | surdimensionné pour l'ID seul | fiche RP2040 |

> Un µC d'ID est **firmware-défini** ; la catégorie est **multi-source** (tout
> petit MCU). Les caractéristiques dépendent de la série et du firmware (à relever).

### Secure Element (chemin d'évolutivité — voir §identité) — ≥ 2 séries
| Réf. | Fab. | Interface | Éléments | Remarque | Source |
|------|------|-----------|----------|----------|--------|
| **ATECC608A/B** | Microchip | I²C ≤ 1 Mbps **[DS]** | **série 72 bits** unique **[DS]** ; clés ECC ; certificats | voir décomposition d'identité ci-dessous | DS40001977B |
| **SE050 (EdgeLock)** | NXP | I²C | identité + crypto (ECC/RSA) | specs à relever | fiche SE050 |

#### Décomposition de l'« identité » d'un Secure Element (à ne pas confondre)
| Élément | Nature | Exploitable comme identifiant public ? |
|---------|--------|----------------------------------------|
| **Numéro de série matériel** | ex. **72 bits** (ATECC608) **[DS]** | **oui, potentiellement** — **sémantique et mode d'exposition à documenter pour la variante étudiée** |
| **Identité d'instance exploitable par le Host** | dérivée du n° de série lu | oui si lisible et documenté |
| **Clé cryptographique** | privée (stockée protégée) | **NON** — une clé **n'est pas** un identifiant public |
| **Certificat provisionné** | à provisionner | oui (après provisioning) — hors V1 |

## 2. Briques de stockage du Manifest (pas une identification) — ≥ 2 séries
| Réf. | Fab. | Tension | Interface | Capacité | Endurance | Écriture | Source |
|------|------|---------|-----------|----------|-----------|----------|--------|
| **MB85RC256V** | Fujitsu | à relever | I²C | 256 Kbit | **≥ 10¹²** **[DS]** | rapide, sans polling **[DS]** | DS501-00017 |
| **FM24CL64B-G** | Infineon/Cypress | **2,7–3,6 V** **[DS]** | I²C | 64 Kbit | ~10¹²–10¹³ **[DS]** | à vitesse bus **[DS]** | Infineon FM24CL64B |
| Flash I²C/SPI dédiée | (à sélectionner) | — | I²C/SPI | — | ~10⁴–10⁵ **[H]** | par pages | réf. à sélectionner |

> Une brique de stockage **ne fournit pas d'identité** (cf.
> [architectures](identification-architectures.md) §3).

## 3. Sources datasheet (traçabilité)

> Consultées le **2026-07-23** ; `[DS]` issues de ces documents. Références
> génériques (Flash « à sélectionner ») **sans `[DS]`** tant qu'une série précise
> n'est pas choisie. Le **statut de cycle de vie** exact (actif/NRND/EOL) est
> **à confirmer au sourcing daté** (`R8`).

| Réf. | Fabricant | Document / n° | Date | Référence stable |
|-------|-----------|---------------|------|------------------|
| 24AA02E64 | Microchip | 2-Kbit I²C EEPROM + EUI — DS20002124 | 2026-07-23 | `microchip.com` (24AA02E64) |
| AT24MAC402 | Microchip (Atmel) | I²C EEPROM + EUI-48/64 + série 128 bits — Atmel-8807 | 2026-07-23 | `microchip.com` (AT24MAC402) |
| 24AA64 | Microchip | 64K I²C EEPROM — DS21189 | 2026-07-23 | `microchip.com` (24AA64) |
| DS28E07 | Analog Devices | DS28E07 1024-bit 1-Wire EEPROM | 2026-07-23 | `analog.com` (DS28E07) |
| DS2431 | Analog Devices | DS2431 1024-bit 1-Wire EEPROM | 2026-07-23 | `analog.com` (DS2431) |
| ATECC608A | Microchip | ATECC608A CryptoAuthentication — DS40001977B | 2026-07-23 | `microchip.com` (ATECC608A) |
| MB85RC256V | Fujitsu | 256K I²C FRAM — DS501-00017 | 2026-07-23 | `fujitsu` (MB85RC256V) |
| FM24CL64B-G | Infineon (Cypress) | 64-Kbit I²C F-RAM — FM24CL64B | 2026-07-23 | `infineon.com` (FM24CL64B) |

## Alimente

- **`DEC-L3-001`** / **`DEC-L3-002`** — ces séries **bornent** la comparaison ;
  **aucune retenue**, aucun MPN final. Les catégories **mono-source** (1-Wire) et
  **multi-source** (EEPROM 64 Kbit) sont explicitement notées.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# Board `rp2040_reference` — simulateur de module CX-Bus (cible retenue)

Rôle : **esclave SPI déterministe** émulant un module CX-Bus (cible de référence
du banc, cf. [comparaison de cible](../../docs/target-comparison.md)). Réutilise
**tel quel** le cœur portable (`portable/`) ; seule la HAL/BSP RP2040 est
spécifique (`hal/rp2040/`).

> **Statut : squelette d'intégration authored — NON compilé ni flashé ici.** Le
> cœur portable est testé en CI ; cette couche board (Pico SDK, PIO SPI-esclave)
> est **validée au premier build/essai local** (voir [BUILD](../../docs/BUILD.md)).
> Aucune mesure, aucun contenu de campagne L1.

## Intégration
- `main.c` câble le cœur portable (`profiles`, `counters`, `transport`,
  `protocol`, `events`) à la HAL RP2040 (`bench_hal_rp2040()`), exécute le
  protocole de contrôle sur la liaison série et émet les événements
  (`TX_BEGIN`/`TX_END`/`IRQ`/`TIMEOUT`/`CRC_ERROR`) vers le backend GPIO/série.
- Le **SPI esclave** est implémenté via **PIO** (déterministe) dans
  `hal/rp2040/` — partie spécifique à finaliser/valider localement.

## Portabilité
Changer de cible = réimplémenter uniquement `hal/<cible>/` + `boards/<carte>/` ;
le cœur (`portable/`) reste **inchangé**.

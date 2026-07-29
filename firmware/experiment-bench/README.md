<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# `firmware/experiment-bench/` — firmware de banc de référence

Bibliothèque de banc **réutilisable** et **indépendante de toute campagne**. Une
campagne (B3+) n'ajoute qu'un **scénario** ; elle ne modifie ni le cœur, ni les
compteurs, ni le protocole.

```text
portable/   # C standard, ZERO dependance plateforme (~80-90% de la logique) — TESTE en CI
  crc/ scheduler/ profiles/ counters/ protocol/ transport/ events/ tests/
hal/        # interfaces FINES + implementations par cible (non testees en CI)
  common/ rp2040/ esp32/
boards/     # integration par carte
  rp2040_reference/ (esclave = simulateur CX-Bus, cible retenue)  xiao_esp32s3/ (hote, squelette)
scenarios/  # profils de trafic declaratifs  (demo/ ; l1_spi_bringup/ = B3 uniquement)
docs/       # architecture, build local, comparaison de cible
```

## Principes
- **Cœur portable** : protocole, profils **déclaratifs** (`profile_id`/`version`/
  `seed`), CRC, machines d'état, compteurs **bruts** vs statistiques **dérivées**,
  échéances/timeouts **abstraits** (le temps réel est dans la HAL). **Aucune**
  dépendance ESP-IDF / Pico SDK / FreeRTOS / Arduino / registre — vérifié
  mécaniquement (`tools/check_arch_deps.sh`) et **testé en CI** (CMake/ctest).
- **Instrumentation par événements** : le cœur **émet** des événements
  (`TX_BEGIN`, `TX_END`, `IRQ`, `TIMEOUT`, `CRC_ERROR`, `RESET`) ; les backends
  (GPIO/série) les traduisent. Le cœur ne pilote **jamais** GPIO/UART.
- **Transports extensibles** : SPI aujourd'hui ; la structure permet d'en ajouter.
- **Cible esclave retenue : RP2040** (choix de banc **réversible**, justifié, sans
  ADR — cf. [comparaison](docs/target-comparison.md)) ; portabilité préservée.

> Le build embarqué (ESP-IDF / Pico SDK) est **hors CI** pour l'instant (voir
> [BUILD](docs/BUILD.md)). Aucune mesure, aucun flash, aucun contenu de campagne L1.

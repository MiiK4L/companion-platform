<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# Architecture du firmware de banc

Séparation stricte (esprit ports/adaptateurs, ADR-0007) : **cœur portable** ↔
**HAL** ↔ **boards**.

```text
[ boards/<carte> : main + wiring ]
            │  injecte
            ▼
[ hal/<cible> : SPI, GPIO, IRQ, timer, serie ]  (fin, non teste en CI)
            │  interface bench_hal_t
            ▼
[ portable/ : protocole, profils, CRC, machines d'etat, compteurs, evenements ]
   (C standard, ZERO dependance plateforme, TESTE en CI)
```

## Cœur portable (indépendant de la cible)
- **Temps abstrait** : `scheduler/` ne connaît qu'échéances/timeouts ; `now` est
  fourni par la HAL (`bench_hal_now_fn`). Aucun scheduler réel dans le cœur.
- **Profils déclaratifs** : `bench_profile_t` = **données** (`profile_id`,
  `profile_version`, `seed`, paramètres) ; générateur pseudo-aléatoire
  **déterministe** seedé → scénarios **rejouables**.
- **Compteurs** : `bench_counters_t` (**bruts**, vérité mesurée) vs `bench_stats_t`
  (**dérivés**, fonction pure).
- **Instrumentation par événements** : `events/` définit les événements et le
  puits (`bench_event_sink_t`) ; le cœur émet, les backends traduisent.
- **Transport** : `transport/` = interface générique (SPI aujourd'hui) + machine
  d'état de transaction **pure** (progression et temps fournis par l'appelant).

## HAL & boards
`hal/common/hal.h` = interfaces fines injectées (`bench_hal_t`). `hal/rp2040`
(esclave, PIO), `hal/esp32` (hôte) = implémentations locales. Changer de cible ne
touche que HAL + board ; le cœur est inchangé.

## Ce qui est testé où
- **CI (host natif)** : tout `portable/` (déterminisme des profils, CRC, codec,
  machines d'état, compteurs, transitions d'erreur, timeouts simulés).
- **Local / matériel** : SPI/GPIO/IRQ/timing réels, flash (hors CI).

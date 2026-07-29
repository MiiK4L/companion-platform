<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# Build local & versions

## Cœur portable + câblage (host, testé/compilé en CI)

Aucune toolchain embarquée requise :

```bash
cmake -S firmware/experiment-bench/portable -B firmware/experiment-bench/portable/build
cmake --build firmware/experiment-bench/portable/build
ctest --test-dir firmware/experiment-bench/portable/build --output-on-failure
```

Cette commande construit :

- `bench_portable` + `bench_portable_tests` : le cœur et ses tests (exécutés) ;
- `bench_slave_wiring` / `bench_host_wiring` : la **compilation du câblage** des
  board applications contre l'API portable (garde d'anti-dérive). Ces cibles
  sont **compilées, non exécutées** ; les primitives HAL sont des **stubs**. Ce
  n'est **pas** un build matériel.

## Firmware embarqué (local, HORS CI)

> Le build embarqué n'est **pas** en CI (pas d'infrastructure lourde prématurée).
> Les commandes et **versions** ci-dessous sont **figées** et à reporter dans le
> `build_manifest` (commit, dirty, toolchain, versions, config, `artifact_sha256`).

- **RP2040** (`boards/rp2040_reference`, esclave) : **Pico SDK** `[version figée à renseigner]`,
  toolchain `arm-none-eabi-gcc [version]`, CMake ≥ 3.16.
- **ESP32-S3** (`boards/xiao_esp32s3`, hôte) : **ESP-IDF** `[version figée à renseigner]`.

Les valeurs exactes seront figées au premier build local et documentées ici.

> Statut : le cœur portable est **compilé et testé**, et le câblage des boards est
> **compilé** contre l'API. L'implémentation matérielle des adaptateurs (Pico
> SDK / ESP-IDF) est **non réalisée et non validée ici** — voir
> [comparaison de cible](target-comparison.md) pour les prérequis avant mesures.

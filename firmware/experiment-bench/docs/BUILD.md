<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# Build local & versions

## Cœur portable (host, testé en CI)

Aucune toolchain embarquée requise :

```bash
cmake -S firmware/experiment-bench/portable -B firmware/experiment-bench/portable/build
cmake --build firmware/experiment-bench/portable/build
ctest --test-dir firmware/experiment-bench/portable/build --output-on-failure
```

## Firmware embarqué (local, HORS CI)

> Le build embarqué n'est **pas** en CI pour l'instant (pas d'infrastructure
> lourde prématurée). Les commandes et **versions** ci-dessous sont **figées** et
> à reporter dans le `build_manifest` (commit, dirty, toolchain, versions, config,
> `artifact_sha256`).

- **RP2040** (`boards/rp2040_reference`, esclave) : **Pico SDK** `[version figée à renseigner]`,
  toolchain `arm-none-eabi-gcc [version]`, CMake ≥ 3.16.
- **ESP32-S3** (`boards/xiao_esp32s3`, hôte) : **ESP-IDF** `[version figée à renseigner]`.

Les valeurs exactes de versions seront figées au premier build local et
documentées ici (elles alimentent la traçabilité du `build_manifest`).

> Statut : le cœur portable est **compilé et testé** ; les couches board/HAL sont
> **authored, non compilées/flashées ici** — à valider au premier build local.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 8 — Protocoles

> **Statut : mixte.** Contrairement aux lots matériels, une partie de ce lot est
> **exécutée en CI** (host, sans matériel) ; le reste est **spécifié, non
> exécuté** (nécessite un POC embarqué, hors périmètre).

## Exécutés en CI (host, ce lot)

| Protocole | Objectif | État |
|-----------|----------|------|
| [Build & tests host](host-build-and-tests.md) | le squelette compile ; tests host verts | **Exécuté (CI `host-tests`)** |
| [Graphe de dépendances](dependency-graph-check.md) | 0 include plateforme/moteur dans ports/services/SDK | **Exécuté (CI `host-tests`)** |

> La **démonstration de la composition root** et l'**indépendance vis-à-vis du
> moteur** sont couvertes par les **tests host** (via `FakeRuntime`).

## Spécifiés, **non exécutés** (POC embarqué ultérieur)

| Protocole | Objectif | État |
|-----------|----------|------|
| [Faisabilité runtime (RAM/flash, install dynamique)](runtime-feasibility.md) | RAM/flash par app **chiffrées** ; faisabilité install dynamique (R7) | **Brouillon** (seuils `[BL]`) |

> Ces mesures relèvent du **POC ESP-IDF + runtime embarqué** ([hors périmètre](../out-of-scope.md)),
> réalisés dans une **PR/campagne ultérieure**. **Aucune valeur `[M]`** ici.

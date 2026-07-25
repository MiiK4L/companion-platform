<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 8 — Règle de dépendances (vérifiée mécaniquement)

> **Statut : Ouvert / Proposé.** Règle **testée en CI**. Outil :
> `tools/check_arch_deps.sh` ; job : `host-tests.yml`.

## La règle

> **Aucun `#include` de plateforme concrète (ESP-IDF, FreeRTOS, pilote) ni de
> moteur d'exécution concret (Lua/WASM/WAMR) dans `ports/`, `services/`, `models/`
> et `companion-sdk/`.** Les détails de plateforme et de moteur vivent
> **exclusivement** dans les **adaptateurs**.

## Vérification

`tools/check_arch_deps.sh` recherche des includes interdits (`esp_`, `freertos`,
`driver/`, `nvs`, `littlefs`, `lvgl`, `lua`, `wasm`, `wamr`…) dans les zones
« propres ». Il **échoue** (exit ≠ 0) si un seul est trouvé — bloquant la CI.

| Zone | Doit être indépendante de |
|------|---------------------------|
| `ports/` | plateforme **et** moteur |
| `services/` | plateforme **et** moteur (dont Lua/WAMR) |
| `models/` | plateforme **et** moteur |
| `companion-sdk/` | plateforme **et** moteur |

## Pourquoi

- Garantit l'**inversion de dépendances** : les services restent **portables** et
  **testables sur host** (mocks), et un **changement de plateforme ou de moteur**
  n'impacte que les **adaptateurs**.
- Rend l'invariant **exécutable** (pas seulement une convention écrite).

## Alimente

- Critère de sortie du lot (« 0 include interdit ») ; invariant réutilisé par la
  composition **cible** (ultérieure).

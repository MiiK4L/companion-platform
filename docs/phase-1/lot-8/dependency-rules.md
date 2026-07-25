<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 8 — Règle de dépendances (vérifiée mécaniquement)

> **Statut : Ouvert / Proposé.** Règle **testée en CI**. Outil :
> `tools/check_arch_deps.sh` ; job : `host-tests.yml`.

## Deux règles vérifiées

**Règle 1 — pas de plateforme/moteur.**
> **Aucun `#include` de plateforme concrète (ESP-IDF, FreeRTOS, pilote) ni de
> moteur d'exécution concret (Lua/WASM/WAMR) dans `ports/`, `services/`, `models/`
> et `companion-sdk/`.**

**Règle 2 — direction du graphe interne** (`ports → services → composition/adaptateurs`).
> **`ports/` et `models/` n'incluent ni `services/`, ni `adapters/`, ni
> `composition/` ; `services/` n'inclut ni `adapters/` ni `composition/` ; aucune
> couche propre n'inclut directement un fichier `adapters/host/`.** Les adaptateurs
> **peuvent** dépendre des ports ; **jamais l'inverse**.

Les détails de plateforme et de moteur vivent **exclusivement** dans les
**adaptateurs** ; les dépendances **remontent** toujours vers l'abstraction.

## Vérification

`tools/check_arch_deps.sh` applique **les deux règles** : (1) includes de
plateforme/moteur interdits (`esp_`, `esp-idf`, `sdkconfig`, `freertos`,
`driver/`, `nvs`, `littlefs`, `lvgl`, `lua`, `wasm`, `wamr`…) dans les zones
« propres » ; (2) **direction du graphe interne** (ports/models ⊅
services/adapters/composition ; services ⊅ adapters/composition ; couches propres
⊅ `adapters/host/`). Il **échoue** (exit ≠ 0) au premier manquement — bloquant la CI.

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

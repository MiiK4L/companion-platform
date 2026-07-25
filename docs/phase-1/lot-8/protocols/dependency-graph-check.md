<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L8-DEPS — Graphe de dépendances (exécuté en CI)

> **Statut : Exécuté en CI** (`.github/workflows/host-tests.yml`). Vérifie
> l'invariant d'inversion de dépendances.

## Décision alimentée

Preuve d'architecture ; garantit que ports/services/SDK restent **portables**.

## Objectif (mesurable)

Deux invariants : (1) **0 `#include`** de plateforme (ESP-IDF/FreeRTOS/pilote) ou
de moteur (Lua/WASM/WAMR) dans `ports/`/`services/`/`models/`/`companion-sdk/` ;
(2) **dépendances internes orientées vers les ports** (`ports/models ← services ←
composition/adaptateurs`, où `←` se lit « dépend de » — jamais l'inverse) ?

## Procédure (reproductible)

```
bash tools/check_arch_deps.sh
```
Le script **échoue** (exit ≠ 0) au premier include interdit — bloquant la CI.

## Critère de réussite

| Grandeur | Seuil |
|----------|-------|
| Includes interdits dans les zones propres | **0** |

## Portée

Zones vérifiées : `firmware/host-skeleton/{ports,services,models}` et
`firmware/companion-sdk`. Les **adaptateurs** sont **exclus** (ils portent
légitimement les détails de plateforme).

## Traçabilité

Commit exact ; job CI `Host tests` ; motif d'include imprimé en cas d'échec.

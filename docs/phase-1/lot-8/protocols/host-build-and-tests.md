<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L8-HOST — Build & tests host (exécuté en CI)

> **Statut : Exécuté en CI** (`.github/workflows/host-tests.yml`). Host, sans
> matériel, sans moteur réel.

## Décision alimentée

Preuve d'architecture pour l'ensemble du Lot 8 (ports/services/composition
implémentables **et testables**) ; **aucune** DEC arbitrée.

## Objectif (mesurable)

Le squelette **compile** (`-Wall -Wextra -Werror`) et **tous les tests host
passent** (exit 0) ?

## Procédure (reproductible)

```
cmake -S firmware/host-skeleton -B firmware/host-skeleton/build
cmake --build firmware/host-skeleton/build
ctest --test-dir firmware/host-skeleton/build --output-on-failure
```

## Critères de réussite

| Grandeur | Seuil |
|----------|-------|
| Compilation (warnings = erreurs) | **0 warning/erreur** |
| Tests host | **0 échec** (exit 0) |

## Couverture actuelle (tests host)

- Lancement nominal via ports (storage → runtime → log).
- Erreur storage (app absente) : runtime non appelé.
- Échec runtime (load) remonté correctement.
- **Indépendance moteur** : l'`AppManager` fonctionne avec `FakeRuntime` (aucun
  Lua/WAMR).

## Traçabilité

Commit exact ; job CI `Host tests` ; sortie `ctest` en cas d'échec.

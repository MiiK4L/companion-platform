<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 8 — Rôles & squelette (preuve d'architecture)

> **Statut : Ouvert / Proposé.** Matérialise les **rôles** de l'architecture
> actée en code host. Le **nommage des dossiers est une conséquence**, pas un
> préalable ; **`HAL` ne redevient pas le centre**. Code : `firmware/host-skeleton/`.

## Rôles matérialisés → fichiers

| Rôle | Matérialisation | Fichiers |
|------|-----------------|----------|
| **Ports** | interfaces abstraites | `ports/*.h` |
| **Modèles** | types de données | `models/app_descriptor.h` |
| **Services** | orchestration (câblage minimal) | `services/app_manager.{h,c}` |
| **Adaptateurs host** | mocks/bouchons | `adapters/host/*.{h,c}` |
| **Adaptateurs cible ESP-IDF** | *(hors périmètre — spécifiés, non écrits)* | — |
| **Composition root** | câblage host | `composition/composition_root.{h,c}` |
| **Companion SDK (façade)** | *(structure existante `firmware/companion-sdk/`)* | — |

> Les **drivers** sont des **détails internes des adaptateurs cible** : ils
> n'apparaissent **pas** dans les ports/services/SDK (vérifié).

## Démonstration (test host)

Le test host prouve, **sans moteur réel** :

1. `AppManager` lit l'unité d'une app via le **port `IStorage`** (mock) ;
2. la charge et l'exécute via le **port `IRuntime`** (**`FakeRuntime`**, bouchon) ;
3. journalise via le **port `ILog`** (mock) ;
4. l'`AppManager` **ne référence aucune** implémentation Lua/WAMR/NVS.

```
HostComposition (possède mocks) ──▶ AppManager ──▶ IStorage/IRuntime/ILog
                                          (ne connaît que les ports)
```

Un **changement de runtime** (Lua ↔ WAMR ↔ FakeRuntime) **ne touche pas**
`AppManager` : seule la composition root change.

## Vérifications (CI)

- **Build host** (CMake) + **tests host** (`ctest`) verts — `host-tests.yml`.
- **Graphe de dépendances** : `tools/check_arch_deps.sh` (0 include interdit).

## Alimente

- Preuve que l'architecture actée est **implémentable/testable** ; base de la
  composition **cible** (ultérieure). **Aucune signature figée.**

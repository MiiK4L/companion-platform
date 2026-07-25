<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 8 — Composants candidats (logiciels, *Proposé*)

> **Statut : Ouvert / Proposé.** Composants logiciels **illustratifs**, **aucun
> retenu**. Le squelette host **n'en intègre aucun** (harnais de test minimal sans
> dépendance). Valeurs `[H]` ; **aucune `[M]`**.

## Runtimes scriptés (`DEC-L8-001`)

| Composant | Projet | Rôle | Licence (à vérifier) | Référence |
|-----------|--------|------|----------------------|-----------|
| **Lua** | Lua.org (PUC-Rio) | interpréteur léger | MIT | `lua.org` |
| **WAMR** | Bytecode Alliance | runtime WebAssembly embarqué | Apache-2.0 | `github.com/bytecodealliance/wasm-micro-runtime` |

## Frameworks de test host — **candidats (choix NON figé)**

> Le framework de test est une **décision non figée** : le squelette utilise un
> **harnais minimal sans dépendance** (`tests/test_support.h`) pour **ne pas
> préempter** ce choix. Les options ci-dessous sont **documentées, non retenues**.

| Composant | Rôle | Remarque |
|-----------|------|----------|
| **Unity + CMock** | tests unitaires C + mocks générés | léger, orienté embarqué |
| **GoogleTest** | tests C/C++ | riche ; dépendance C++ |
| **Harnais minimal (actuel)** | assertions sans dépendance | évite de figer le choix |

## Ce que le squelette intègre réellement

**Rien** des composants ci-dessus : uniquement du **C standard** + un **harnais de
test minimal**. Aucun moteur (Lua/WAMR), aucun framework tiers — cohérent avec le
[hors périmètre](out-of-scope.md).

## Alimente

- **`DEC-L8-001`** (runtime) — Lua/WAMR **comparés**, aucun retenu. Le **framework
  de test** reste **candidat** (décision ultérieure, hors `DEC-L8-*`).

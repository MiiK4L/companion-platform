<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 8 — Runtime scripté : comparaison (*Proposé*, `DEC-L8-001`)

> **Statut : Ouvert / Proposé.** Comparaison **par critères**, **sans
> présélection** (ne pas figer Lua). Le runtime est **abstrait par le port
> `IRuntime`** : le choix ci-dessous **n'affecte pas** l'`AppManager`. Valeurs
> `[H]/[BL]` ; **aucune `[M]`** (RAM/flash **spécifiés**, mesurés ultérieurement).

## Approches comparées (au même niveau)

| Approche | Principe | Illustré par (non retenu) |
|----------|----------|---------------------------|
| **(RT1) Lua** | interpréteur de bytecode léger | Lua / LuaJIT (selon cible) |
| **(RT2) WASM** | machine virtuelle WebAssembly | **WAMR** (WebAssembly Micro Runtime) |
| **(RT3) Autre** | à documenter si pertinent | — |

## Grille comparative (critères communs)

| Critère | (RT1) Lua | (RT2) WASM/WAMR | Étiquette |
|---------|-----------|-----------------|-----------|
| Empreinte RAM par app | à mesurer | à mesurer | **[BL]** |
| Empreinte flash (moteur) | à mesurer | à mesurer | **[BL]** |
| Isolation / bac à sable | modérée (à cadrer) | **forte** (mémoire linéaire) **[H]** | **[H]** |
| Maturité embarquée | élevée **[H]** | croissante **[H]** | **[H]** |
| Langages sources | Lua | multi (C/Rust→WASM) **[H]** | **[H]** |
| Exposition du Companion SDK | via bindings | via imports WASM | **[H]** |
| Complexité d'intégration | faible-moyenne **[H]** | moyenne **[H]** | **[H]** |
| Faisabilité install dynamique | à démontrer (R7) | à démontrer (R7) | **[BL]** |

> **Seuils RAM/flash `[BL]`** : mesurés au baselining (POC embarqué, hors périmètre
> de ce lot). Aucune cible figée ici.

## Abstraction par port (invariant)

Quel que soit le choix `DEC-L8-001`, l'`AppManager` **ne dépend que de `IRuntime`**
(prouvé par `FakeRuntime`). Un moteur réel sera un **adaptateur** implémentant
`IRuntime` — **sans** toucher aux services.

## Alimente

- **`DEC-L8-001`** — runtime arbitré **après** POC + mesures RAM/flash + faisabilité
  install dynamique. **Aucun runtime retenu.** **Indépendant** de `DEC-L8-002`.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 8 — Hors périmètre (garde-fou anti-dérive)

> **Statut : normatif pour ce lot.** Ce document **borne** le squelette host afin
> d'éviter toute dérive vers une implémentation réelle. Le squelette est une
> **preuve d'architecture uniquement**.

## Autorisé (et rien de plus)

- **Interfaces de ports** (déclarations C candidates).
- **Modèles** (types de données, sans logique).
- **Composition root** (câblage host).
- **Adaptateurs mock / bouchons** (host, pour les tests).
- **Tests host** exécutés en CI.
- **Vérification des dépendances** (graphe/includes).

## Explicitement **hors périmètre** (à ne PAS écrire dans ce lot)

| Exclusion | Raison |
|-----------|--------|
| **Runtime réel** (Lua, WAMR…) | relève de `DEC-L8-001` ; le squelette n'utilise qu'un `FakeRuntime` (bouchon) |
| **Logique métier** (services applicatifs réels) | preuve d'architecture, pas d'application |
| **Scheduler** (ordonnancement réel) | implémentation, hors preuve |
| **Drivers** | détails internes des adaptateurs cible, non écrits ici |
| **HAL réel / adaptateurs cible ESP-IDF** | POC ESP-IDF **spécifié, non exécuté** |
| **Code ESP-IDF / FreeRTOS** | interdit dans ports/services/SDK (vérifié) |
| **Code embarqué** | ce lot est **host uniquement** |
| **Signatures de ports figées** | les signatures sont **candidates**, révisables |
| **Choix d'un framework de test** | non figé ; harnais minimal sans dépendance |
| **Modèle de distribution figé** | `DEC-L8-002`, ouvert |
| **Format Manifest réel** | Lot 3 ; ici modèle de données minimal |

## Reportés (spécifiés, non exécutés)

- **POC ESP-IDF** (boot → composition root → 1 port réel).
- **Runtime scripté embarqué** + **RAM/flash par app chiffrées**.
- **Faisabilité de l'installation dynamique** (R7).

> Objectif : **démontrer que l'architecture est implémentable et testable sans
> figer les futures décisions techniques** (`DEC-L8-*` et suivantes).

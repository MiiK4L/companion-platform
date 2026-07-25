<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 8 — Conception des ports (signatures **candidates**)

> **Statut : Ouvert / Proposé.** Les signatures ci-dessous sont des
> **candidates révisables**, **PAS une API figée** : elles servent la **preuve
> d'architecture**. Le code de référence est sous `firmware/host-skeleton/ports/`.

## Principe (inversion de dépendances en C)

Chaque port est une **interface** = un **contexte opaque `self`** + des
**pointeurs de fonctions**. Les services dépendent de l'interface ; les
adaptateurs la **fournissent**. Aucun port ne connaît de plateforme concrète.

```c
typedef struct IRuntime {
  void *self;                                             // contexte (adaptateur)
  rt_status_t (*load)(void *self, const uint8_t *u, size_t n);
  rt_status_t (*run)(void *self);
} IRuntime;
```

## Ports matérialisés (déclarations candidates)

| Port | Rôle | Fichier |
|------|------|---------|
| `IClock` | base de temps monotone (source/validité = Lot 7) | `ports/iclock.h` |
| `IStorage` | lecture d'un blob nommé (borné) | `ports/istorage.h` |
| `IBus` | accès bus générique (I²C/SPI…) | `ports/ibus.h` |
| `IDisplay` | sortie d'affichage (contrôleur = Lot 4) | `ports/idisplay.h` |
| `IInput` | entrée utilisateur | `ports/iinput.h` |
| `IPower` | énergie (chaîne = Lot 6) | `ports/ipower.h` |
| **`ILog`** | **journalisation abstraite** | `ports/ilog.h` |
| **`IRuntime`** | **moteur scripté abstrait** (choix = `DEC-L8-001`) | `ports/iruntime.h` |

> **`ILog`** est un port à part entière (ajouté à ce lot) : les services
> journalisent **via le port**, jamais via un backend concret.

## Règles de signature (candidates)

- **Contexte non possédé** : `self` appartient à l'adaptateur (voir
  [règles d'injection](dependency-injection-rules.md)).
- **Bornes explicites** : toute lecture est **plafonnée** (`cap`, `out_len`) — le
  code appelant n'alloue jamais selon une taille non contrôlée.
- **Statuts explicites** : retours par `enum` (pas de code magique).
- **Aucune dépendance** : les en-têtes de ports n'incluent **ni** ESP-IDF **ni**
  moteur (vérifié, cf. [règle de dépendances](dependency-rules.md)).

## Évolution & versionnement des interfaces (tant qu'elles sont **candidates**)

Les ports sont **candidats** : ils **évolueront**. Règles pendant cette phase :

- **Pas de garantie de stabilité** : aucune promesse de compatibilité ascendante
  tant que les signatures sont candidates (statut affiché dans chaque en-tête).
- **Changement libre mais tracé** : une modification de signature est un **écart
  documenté** (commit + mise à jour de ce document) ; le squelette et ses tests
  **doivent rester verts** après tout changement (garde-fou CI).
- **Un seul point d'impact** : un changement de port n'impacte que **les
  adaptateurs** et **le point d'appel du service** — jamais la sémantique du bus
  (grâce à l'inversion de dépendances).
- **Versionnement différé** : une politique de **version d'interface** (ex.
  `IRuntime` v1/v2, ou un champ de version) n'est **arrêtée qu'à la consolidation**
  (Phase 2+), **quand** le port passe de *candidat* à *stable* — pas dans ce lot.
- **Critère de passage candidat → stable** : port éprouvé par ≥ 1 adaptateur réel
  (cible) **et** ≥ 1 adaptateur mock, signatures inchangées sur une itération.

## Alimente

- L'architecture **actée** (ports/adaptateurs) ; les signatures restent
  **candidates** jusqu'à consolidation (Phase 2+). **Aucune API figée ici.**

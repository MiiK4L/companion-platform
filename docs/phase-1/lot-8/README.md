<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 8 — Squelette ports/adaptateurs (preuve d'architecture) & runtime scripté

> **Statut : en cours (documentaire + squelette host de preuve).** Matérialise
> l'architecture **déjà actée** ([ADR-0007](../../adr/0007-hal-et-companion-sdk.md),
> [ADR-0002](../../adr/0002-modele-execution-applications.md),
> [inversion des dépendances](../../architecture/dependency-inversion.md)) en un
> **squelette host testable** (**preuve d'architecture uniquement**), et compare
> les runtimes scriptés — **sans figer** les composants. **Aucune ADR**, **rien en
> Accepté**, **aucune mesure inventée**. Voir le [plan](../plan.md) (Lot 8).

## Deux décisions séparées & **indépendantes** (*Ouvert / Proposé*)

- **`DEC-L8-001` — runtime scripté** (Lua, WASM/WAMR…) → **ADR-0022**.
- **`DEC-L8-002` — modèle de distribution des applications** (install dynamique
  vs bundle OTA) → **ADR future non réservée**.

> **Indépendance explicite** : le **modèle de distribution ne dépend pas du
> runtime retenu**, et réciproquement. Le runtime est un **port `IRuntime`** :
> l'`AppManager` ne dépend **jamais** d'une implémentation Lua/WAMR (démontré par
> le squelette + un `FakeRuntime`).

## Volet A — Documentaire

[Rôles & squelette](architecture-skeleton.md) · [Conception des ports](ports-design.md)
(signatures **candidates**) · [Règles d'injection de dépendances](dependency-injection-rules.md) ·
[Règle de dépendances](dependency-rules.md) · [Comparaison des runtimes](scripted-runtime-comparison.md) ·
[Modèle de distribution](app-distribution-model.md) · [Composants candidats](candidate-components.md) ·
[Protocoles](protocols/README.md) · **[Hors périmètre du Lot 8](out-of-scope.md)**.

## Volet B — Squelette host (code réel, preuve d'architecture)

Sous `firmware/host-skeleton/` (Apache-2.0) : **interfaces de ports**, **modèles**,
**composition root**, **adaptateurs mock**, **tests host exécutés en CI**,
**vérification des dépendances** (`tools/check_arch_deps.sh`,
`.github/workflows/host-tests.yml`).

| Élément | Fichiers |
|---------|----------|
| Ports (interfaces C **candidates**) | `ports/{iclock,istorage,ibus,idisplay,iinput,ipower,ilog,iappsource,iruntime}.h` |
| Modèles (référence + vue **opaques**, neutres) | `models/app_reference.h`, `models/app_artifact_view.h` |
| Service (délégation pure) | `services/app_manager.{h,c}` — dépend **des ports uniquement**, aucun modèle de chargement |
| Adaptateurs host (mocks/bouchon) | `adapters/host/{fake_runtime,mock_app_source,mock_log}.{h,c}` |
| Composition root | `composition/composition_root.{h,c}` |
| Tests host (harnais minimal) | `tests/{test_support.h,test_app_manager.c,test_main.c}` |

## Contraintes de périmètre (voir [hors périmètre](out-of-scope.md))

Le squelette **prouve** que l'architecture est **implémentable et testable** ; il
**ne commence pas** l'implémentation du firmware. **Exclus** : aucun **runtime
réel**, aucune **logique métier**, aucun **scheduler**, aucun **driver**, aucun
**HAL réel**, aucun **code ESP-IDF**, aucun **code embarqué**. Le **langage C** est
choisi pour ce squelette host comme **convention d'implémentation provisoire et
réversible** ; **ce choix ne constitue pas encore une décision définitive sur
l'ensemble du firmware** (ESP-IDF prend en charge C **et** C++ ; aucune ADR ne
fige le langage à ce stade). Le **framework de test n'est pas figé** (harnais
minimal sans dépendance) ; les **signatures de ports sont candidates**, pas une
API figée.

## Critères de sortie

- Squelette host **compile** et **tests host verts en CI** ; **graphe de
  dépendances vérifié** (0 include ESP-IDF/FreeRTOS/pilote/moteur dans
  ports/services/SDK) ; **composition root** démontrée ; `AppManager` prouvé
  **indépendant** du moteur (via `FakeRuntime`).
- Runtimes (Lua vs WASM/WAMR) et modèle de distribution **comparés**, indépendants,
  **aucun retenu** ; `DEC-L8-001`/`DEC-L8-002` **ouverts**.

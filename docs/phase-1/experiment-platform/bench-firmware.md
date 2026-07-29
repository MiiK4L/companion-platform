<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Firmware de banc (`firmware/experiment-bench/`)

> **Statut : Proposé.** Décrit l'**organisation**, les **interfaces** et les
> **conventions** du firmware de banc **réutilisable**. Le code de référence est
> sous `firmware/experiment-bench/`. **Aucun contenu de campagne** ici (pas de
> profil L1, pas d'observable, pas de `[BL]`), **aucune mesure**, **aucun flash**.

Le firmware de banc est une **bibliothèque réutilisable**, indépendante de toute
campagne : une campagne (à partir de B3) n'ajoute qu'un **scénario** (profil
déclaratif) et n'altère ni le cœur, ni les compteurs, ni le protocole.

## Organisation (séparation stricte)

```text
firmware/experiment-bench/
  portable/    # C standard, ZERO dependance plateforme (~80-90% de la logique)
    events/     # types d'evenements + puits (le coeur EMET, ne pilote pas le GPIO)
    crc/        # CRC-32/IEEE portable
    scheduler/  # echeances / timeouts ABSTRAITS (aucune horloge reelle)
    profiles/   # profil DECLARATIF (POD) + generateur deterministe seede
    counters/   # compteurs BRUTS vs statistiques DERIVEES (fonction pure)
    protocol/   # codec texte du protocole de controle (aucune E/S)
    transport/  # transport generique (SPI d'abord) + machine d'etat pure
    tests/      # tests host (CMake/ctest) — executes en CI
  hal/         # interfaces FINES injectees + adaptateurs par cible (hors CI)
    common/ rp2040/ esp32/
  boards/      # integration par carte (main + cablage) — hors CI
    rp2040_reference/  # ESCLAVE = simulateur CX-Bus (cible retenue)
    xiao_esp32s3/      # HOTE (squelette)
  scenarios/   # profils declaratifs  (demo/ ; l1_spi_bringup/ = B3 uniquement)
  docs/        # architecture, build local, comparaison de cible
```

## Principes de conception

- **Cœur portable indépendant de la cible.** Protocole, profils, CRC, machines
  d'état, compteurs : C standard, **aucune** dépendance ESP-IDF / Pico SDK /
  FreeRTOS / Arduino / registre matériel. Garanti mécaniquement par
  `tools/check_arch_deps.sh` (interdit les `#include` **et** les symboles de
  plateforme dans `portable/`) et **testé en CI** (`portable/CMakeLists.txt`,
  build natif + `ctest`).
- **Temps abstrait.** Le cœur ne connaît que des échéances et des timeouts en
  *ticks* ; l'horloge réelle (`now`) est fournie par la HAL. Aucun ordonnanceur
  réel dans le cœur.
- **Profils déclaratifs et rejouables.** Un profil est une **donnée** POD
  (`profile_id`, `profile_version`, `seed`, paramètres). Le générateur
  pseudo-aléatoire est **déterministe** (seedé) : un scénario est exactement
  **rejouable**.
- **Compteurs bruts ≠ statistiques dérivées.** Les compteurs bruts sont la
  **vérité mesurée**, jamais recalculée ; les statistiques (moyenne, débit, taux
  de succès) sont recalculées par une **fonction pure**. Cette séparation reflète
  la distinction `raw`/`normalized` du modèle de données de mesure.
- **Instrumentation par événements.** Le cœur **émet** des événements
  (`TX_BEGIN`, `TX_END`, `IRQ`, `TIMEOUT`, `CRC_ERROR`, `RESET`) ; les backends
  (dans les boards) les **traduisent** (toggle GPIO pour analyseur logique, ligne
  série…). Le cœur ne pilote **jamais** directement GPIO/UART.
- **Transports extensibles.** Le SPI est le premier transport ; l'interface
  générique et le champ `kind` permettent d'en ajouter sans réécrire le cœur.

## HAL & boards (hors CI, honnêtement étiquetés)

`hal/common/hal.h` définit des **interfaces fines** (pointeurs de fonctions +
contexte opaque) injectées dans le cœur. Les adaptateurs `hal/rp2040` et
`hal/esp32` et les `boards/` sont **écrits mais NON compilés ni flashés ici** :
ils seront validés au premier build/essai **local**. Changer de cible ne touche
que `hal/<cible>` + `boards/<carte>` ; le cœur reste inchangé.

La cible esclave retenue est le **RP2040** (PIO → esclave SPI déterministe, cœur
dédié distinct du DUT). C'est un **choix de banc réversible**, justifié sur
critères techniques, **sans ADR** — voir `firmware/experiment-bench/docs/target-comparison.md`.

## Ce qui est vérifié, et où

| Couche | Vérification |
|--------|--------------|
| `portable/` | **CI** : build natif `-Wall -Wextra -Werror` + `ctest` ; `check_arch_deps.sh` |
| `hal/`, `boards/` | **Local** : premier build embarqué (Pico SDK / ESP-IDF), essais matériels |

## Périmètre (rappel)

Ce lot livre l'**infrastructure de banc**. Il **ne** contient **aucune** donnée
`[M]`, **aucun** profil de campagne L1, **aucune** ADR. La première campagne
(EXP-L1-BRINGUP-001) et son profil `scenarios/l1_spi_bringup/` relèvent d'un lot
ultérieur, validé et fusionné séparément.

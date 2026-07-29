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
campagne : une campagne (à partir de **B3**) n'ajoute qu'un **scénario** (profil
déclaratif) et n'altère ni le cœur, ni les compteurs, ni le protocole.

## Organisation (séparation stricte)

```text
firmware/experiment-bench/
  portable/    # C standard, ZERO dependance plateforme (~80-90% de la logique)
    ports/      # contrats de ROLE : SPI maitre vs esclave, IRQ in/out, horloge
    util/       # (in)serialisation gros-boutiste, arithmetique saturante
    crc/ frame/ # CRC-32/IEEE + trame versionnee protegee par CRC
    scheduler/  # echeances / timeouts ABSTRAITS et WRAP-SAFE
    profiles/ scenario/  # profil DECLARATIF ; scenario = profil + roles + duree
    protocol/   # protocole de controle BINAIRE (framed/versionne/borne)
    counters/ transport/ # compteurs bruts/derives + machine d'etat (saturants)
    engine/     # moteur Host + Slave (logique partagee)
    sim/        # lien SPI simule (transport de test) pour derouler le flux en CI
    tests/      # tests host (CMake/ctest) — executes en CI
  hal/         # interfaces FINES par ROLE + adaptateurs (stubs, hors CI materiel)
  boards/      # board applications : PUR CABLAGE HAL <-> moteur
  scenarios/   # profils + scenarios  (demo/ ; l1_spi_bringup/ = B3 uniquement)
  docs/        # architecture, build local, comparaison de cible
```

## Principes de conception

- **Contrats de rôle explicites (maître ≠ esclave).** Un simulateur CX-Bus
  esclave n'a pas le cycle de vie d'un maître : deux ports distincts
  (`bench_spi_master_t` = `transfer`/`cancel`/`status` ; `bench_spi_slave_t` =
  `prepare_response`/`on_select`/`on_transaction`/`peek_response`/
  `transaction_status`). L'**IRQ** est directionnelle : l'hôte la **lit**
  (`bench_irq_in_t`), le module l'**émet** (`bench_irq_out_t`).
- **Intégrité CRC réelle dans le flux.** La trame (`frame/`) est versionnée et
  protégée par **CRC-32/IEEE** ; le décodeur est **borné** (rejette tronqué /
  magic / version / longueur / CRC). L'injection de faute corrompt de **vrais
  octets** : la détection est démontrée de bout en bout (moteur Host↔Slave sur le
  lien simulé), pas seulement par un compteur.
- **Protocole de contrôle binaire**, framed/versionné/borné, indépendant de
  `printf` : `SELECT_PROFILE`, `START`, `STOP`, `READ_COUNTERS`,
  `RESET_COUNTERS`, `GET_CAPABILITIES` + codes de statut ; argument trop long
  **rejeté**, jamais tronqué. Une console humaine serait un **adaptateur** texte→
  binaire, jamais le contrat autoritaire.
- **Temps abstrait et wrap-safe** (soustraction modulaire, ticks 64 bits,
  `timeout=0` défini) ; **compteurs et transport saturants** (`bytes_target=0` et
  progression excessive définis ; aucun débordement silencieux).
- **Profils 100% déclaratifs.** Un profil décrit **tout** ce qui définit le
  trafic : fréquence SPI, tailles de trames, délais inter-transaction, politique
  IRQ, motif de payload (seedé / constant / incrémental), injections de fautes et
  seed de reproductibilité. Le **moteur n'exécute que ce qui est décrit** ; il ne
  contient **aucun paramètre de scénario codé en dur**.
- **Profile ≠ Scenario ≠ Board application.** Le profil = paramètres déclaratifs
  rejouables ; le scénario = composition profil + rôles Host/Slave + durée ; la
  board application = câblage des ports aux adaptateurs. Toute la logique vit dans
  le **moteur** ; les `main.c` ne font que câbler.
- **Lien simulé fautif.** Le lien SPI simulé n'est **pas** un bus parfait : il
  injecte, par transaction, latence (→ timeout), CS relâché prématurément,
  réponse tronquée ou perdue, statut timeout/erreur forcé et IRQ concurrente.
  Les tests host couvrent ainsi les **problèmes de synchronisation**, pas
  uniquement le nominal.
- **Instrumentation par événements** ; **transports extensibles**.

## HAL & boards — statut honnête

`hal/common/hal.h` définit deux agrégats **par rôle**. Les adaptateurs
(`hal/rp2040`, `hal/esp32`) et les board applications sont **écrits** ; leur
**câblage est compilé** contre l'API portable en CI (garde d'anti-dérive), mais
l'**implémentation matérielle n'est ni réalisée ni validée ici** : les primitives
HAL sont des **stubs**, sans Pico SDK / ESP-IDF, non flashées.

Le **RP2040** est la **cible candidate de référence** (esclave) — choix de banc
**réversible**, sans ADR. Les prérequis avant toute mesure (compilation Pico SDK,
PIO SPI-esclave, boucle locale/hôte, IRQ sortante vérifiée, trames CRC réellement
échangées) sont dans `firmware/experiment-bench/docs/target-comparison.md`.

## Ce qui est vérifié, et où

| Couche | Vérification |
|--------|--------------|
| `portable/` | **CI** : build natif `-Wall -Wextra -Werror` + `ctest` (trame, protocole, scheduler, transport, compteurs, profils, **moteur de bout en bout**) ; `check_arch_deps.sh` (includes **et** symboles) |
| câblage `boards/` + `hal/` | **CI** : compilation du câblage contre l'API portable (non exécuté) |
| matériel (Pico SDK / ESP-IDF, SPI/PIO/IRQ réels, flash) | **Local**, hors CI |

## Périmètre (rappel)

Ce lot livre l'**infrastructure de banc**. Il **ne** contient **aucune** donnée
`[M]`, **aucun** profil de campagne L1, **aucune** ADR. La première campagne
(EXP-L1-BRINGUP-001) et son profil `scenarios/l1_spi_bringup/` relèvent d'un lot
ultérieur, validé et fusionné séparément.

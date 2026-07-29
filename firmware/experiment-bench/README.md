<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# `firmware/experiment-bench/` — firmware de banc de référence

Bibliothèque de banc **réutilisable** et **indépendante de toute campagne**. Une
campagne (B3+) n'ajoute qu'un **scénario** ; elle ne modifie ni le cœur, ni les
compteurs, ni le protocole.

```text
portable/   # C standard, ZERO dependance plateforme (~80-90% de la logique) — TESTE en CI
  ports/      # contrats : SPI MAITRE vs ESCLAVE, IRQ entree/sortie, horloge
  util/       # (in)serialisation gros-boutiste, arithmetique saturante
  crc/ frame/ # CRC-32/IEEE + format de trame versionne protege par CRC
  scheduler/  # echeances / timeouts ABSTRAITS et WRAP-SAFE
  profiles/   # profil 100% DECLARATIF (freq, tailles, delais, IRQ, motif, fautes, seed)
  protocol/   # protocole de controle BINAIRE (framed/versionne/borne)
  counters/   # compteurs BRUTS vs statistiques DERIVEES (saturants)
  transport/  # machine d'etat de transaction (saturante, wrap-safe)
  scenario/   # composition profil + roles Host/Slave (distinct du profil)
  engine/     # moteur Host + Slave (n'execute QUE ce que le profil decrit)
  sim/        # lien SPI SIMULE + INJECTION DE FAUTES (latence, CS, troncature, IRQ...)
  tests/      # tests host (CMake/ctest) — executes en CI
hal/         # interfaces FINES par ROLE + adaptateurs par cible (stubs, hors CI materiel)
  common/ rp2040/ (esclave) esp32/ (hote)
boards/      # board applications : PUR CABLAGE HAL <-> moteur (aucune logique)
  rp2040_reference/  xiao_esp32s3/
scenarios/   # profils + scenarios declaratifs  (demo/ ; l1_spi_bringup/ = B3)
docs/        # architecture, build local, comparaison de cible
```

## Principes
- **Contrats de rôle explicites.** Le SPI **maître** (Host) et le SPI **esclave**
  (module CX-Bus) ont des cycles de vie distincts, donc des ports distincts
  (`ports/spi.h`) ; l'**IRQ** est directionnelle (`ports/irq.h`) : l'hôte la
  **lit**, le module l'**émet**. Le cœur ne fusionne pas les deux rôles.
- **Trame CRC réelle dans le flux.** `frame/` définit une trame versionnée
  (magic/version/seq/len/payload/**crc32**) ; l'injection de faute corrompt de
  **vrais octets**, et la détection est démontrée de bout en bout par les tests.
- **Protocole de contrôle binaire**, framed/versionné/borné (`protocol/`),
  indépendant de `printf` : `SELECT_PROFILE`, `START`, `STOP`, `READ_COUNTERS`,
  `RESET_COUNTERS`, `GET_CAPABILITIES` + codes de statut ; arguments trop longs
  **rejetés** (jamais tronqués).
- **Temps abstrait et wrap-safe** ; compteurs et transport **saturants** (aucun
  débordement silencieux ; `timeout=0` et progression excessive définis).
- **Profils 100% déclaratifs.** Un profil décrit **tout** : fréquence SPI,
  tailles de trames, délais inter-transaction, politique IRQ, motif de payload,
  injections de fautes et seed. Le **moteur n'exécute que ce qui est décrit** —
  aucun paramètre de scénario codé en dur.
- **Profile ≠ Scenario ≠ Board application.** Le profil décrit un trafic ; le
  scénario compose profil + rôles ; la board application ne fait que **câbler**
  la HAL au **moteur** partagé (`engine/`). Aucune logique dans les `main.c`.
- **Lien simulé fautif.** `sim/` n'est **pas** un bus parfait : il injecte
  latence, CS relâché prématurément, réponse tronquée/perdue, timeout/erreur
  forcés et IRQ concurrente, pour tester la **synchronisation**.
- **Instrumentation par événements** ; **transports extensibles** (SPI d'abord).

## Statut des cibles
- **RP2040** : cible **candidate de référence** (esclave). **Contrat et squelette
  d'intégration fournis** ; **implémentation matérielle NON réalisée et NON
  validée** ici. Voir [comparaison](docs/target-comparison.md) pour les
  prérequis avant toute mesure (B3).
- CI : tout `portable/` est **compilé et testé**, et le **câblage** des board
  applications est **compilé** contre l'API portable (garde d'anti-dérive).
  **Aucun build matériel (Pico SDK / ESP-IDF), aucun flash, aucune mesure.**

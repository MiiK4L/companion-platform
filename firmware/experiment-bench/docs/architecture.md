<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# Architecture du firmware de banc

Séparation stricte (esprit ports/adaptateurs, ADR-0007) : **cœur portable** ↔
**HAL** ↔ **board application**.

```text
[ boards/<carte> : main = PUR CABLAGE HAL <-> moteur, aucune logique ]
            │  injecte (ports)
            ▼
[ hal/<cible> : horloge, SPI, IRQ, serie ]  (adaptateurs ; stubs hors CI materiel)
            │  implemente / cable les PORTS
            ▼
[ portable/ : ports, trame CRC, protocole, profils, scenario, MOTEUR, compteurs ]
   (C standard, ZERO dependance plateforme, TESTE en CI)
```

## Contrats de rôle (ports)
Maître et esclave n'ont pas le même cycle de vie ; ils ne partagent donc pas une
primitive synchrone unique.

- **`bench_spi_master_t`** (Host) : `transfer` / `cancel` / `status`.
- **`bench_spi_slave_t`** (module) : `prepare_response` / `on_select` /
  `on_transaction` / `peek_response` / `transaction_status`. Passif, piloté par CS.
- **IRQ directionnelle** : `bench_irq_in_t` (l'hôte **lit**) vs `bench_irq_out_t`
  (le module **émet** : `raise` / `clear`).
- **Horloge** (`bench_clock_t`) : le cœur n'a aucune horloge propre.

## Cœur portable
- **Trame** (`frame/`) : versionnée, protégée par **CRC-32/IEEE**, décodeur
  **borné** (rejette tronqué / magic / version / longueur / CRC). Injection de
  faute = corruption **réelle** d'octets.
- **Protocole** (`protocol/`) : binaire, borné ; s'appuie sur la trame pour le
  framing/version/CRC. Arguments trop longs **rejetés**.
- **Scheduler** (`scheduler/`) : échéances **wrap-safe** (soustraction modulaire,
  ticks 64 bits) ; `timeout=0` défini comme déjà expiré.
- **Profils** (`profiles/`) : POD **100% déclaratif** — fréquence SPI, tailles,
  délais, politique IRQ, motif de payload, injections de fautes, seed. Le moteur
  n'exécute que ce que le profil décrit (aucun paramètre codé en dur).
- **Compteurs** (`counters/`) : **bruts** (vérité) vs **dérivés** (fonction pure) ;
  toutes les mises à jour **saturent**.
- **Transport** (`transport/`) : machine d'état de transaction, saturante,
  `bytes_target=0` et progression excessive **définis**.
- **Scénario** (`scenario/`) : compose un profil avec rôles/version/durée.
- **Moteur** (`engine/`) : logique Host **et** Slave (encodage de trames,
  injection de fautes réelles, comptage, événements). Les board applications ne
  font que câbler la HAL au moteur.
- **Lien simulé** (`sim/`) : relie un port maître à un port esclave en mémoire,
  avec horloge et IRQ partagées. **Pas un bus parfait** : injecte par transaction
  latence, CS relâché prématurément, réponse tronquée/perdue, timeout/erreur
  forcés et IRQ concurrente — pour tester la **synchronisation**, pas que le
  nominal.

## HAL & boards
`hal/common/hal.h` définit deux agrégats **par rôle** : `bench_hal_host_t`
(implémente le port maître, lit l'IRQ) et `bench_hal_slave_t` (primitives
recv/send bas niveau + IRQ sortante ; le port ISpiSlave est fourni par le
moteur). Les `main.c` câblent puis lancent — rien d'autre.

## Ce qui est vérifié, et où
- **CI (host natif)** : tout `portable/` (trame, protocole, scheduler, transport,
  compteurs, profils, **moteur de bout en bout** sur le lien simulé) ; plus la
  **compilation du câblage** des board applications contre l'API portable.
- **Local / matériel** : build Pico SDK / ESP-IDF, SPI/PIO/IRQ/timing réels,
  flash. **Hors CI.**

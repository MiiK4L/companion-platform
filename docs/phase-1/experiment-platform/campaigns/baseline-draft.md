<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# EXP-L1-BRINGUP-001 — baseline **brouillon**

> **Statut : `draft`. Cette baseline N'AUTORISE AUCUN RUN.**
> Elle n'est ni *Revue*, ni *Baseline d'essai*, ni approuvée. Les valeurs sont
> **proposées**, majoritairement en confiance `hypothesis`, et seront **finalisées
> en B4** après vérification du matériel et des **capacités réelles du banc**.
>
> **Règle d'immuabilité** : toute valeur modifiée crée une **nouvelle
> `baseline_version`**. Une version déjà utilisée par un run n'est **jamais**
> modifiée rétroactivement.

Cycle : **B3 → `draft`** · **B4 → revue → instanciation → approbation → exécution.**

## Classes de confiance

`normative` (imposée par une spécification externe) · `measured` (issue d'une
mesure qualifiée) · `hypothesis` (proposée, non vérifiée).

> À ce stade, **aucune** valeur n'est `measured` : aucune mesure n'existe.

## Catégories de champs `[BL]`

Les `[BL]` ne jouent pas tous le même rôle ; les confondre reviendrait à traiter
un paramètre de charge comme un critère de succès.

| Catégorie | Rôle | Effet |
|-----------|------|-------|
| **A — Invariants de validité du banc** | conditions sans lesquelles le run ne veut rien dire | violation ⇒ run `INVALID` |
| **B — Critères de verdict technique** | ce qui décide `PASS`/`FAIL` | violation ⇒ `FAIL` |
| **C — Paramètres de charge expérimentale** | ce qui est *appliqué*, pas jugé | ne décide d'aucun verdict |

Exemple de la distinction demandée : la **fréquence SPI** et le **nombre de
transactions** sont des **paramètres de charge (C)** ; « aucune corruption CRC »
est un **critère de verdict (B)**.

## A — Invariants de validité du banc

| id | name | value (proposée) | confidence | justification |
|----|------|------------------|------------|---------------|
| BL-001 | `supply_voltage_range` | 3,30 V ± 5 % | hypothesis | plage nominale usuelle du véhicule de référence ; **à vérifier** sur le banc |
| BL-002 | `ground_topology` | masse commune, point unique | hypothesis | évite les boucles de masse faussant l'intégrité SPI |
| BL-003 | `max_wire_length` | ≤ 100 mm par ligne SPI | hypothesis | limite les réflexions ; **à confirmer** à la fréquence retenue |
| BL-004 | `build_reproducible` | arbre git propre, ou diff archivé et haché | normative | exigé par `promote_to_measured` (socle) |
| BL-005 | `clock_sync_verified` | horodatages hôte/module corrélés, dérive bornée | hypothesis | sans cela, les latences croisées et la latence d'IRQ sont ininterprétables |
| BL-006 | `instrumentation_overhead` | ≤ 2 % du temps de transaction | hypothesis | l'instrumentation ne doit pas produire l'effet mesuré ; **à mesurer** en B4 |
| BL-007 | `ambient_temperature` | 20–27 °C, consignée par run | hypothesis | limite la dérive thermique entre runs |

## B — Critères de verdict technique

| id | name | value (proposée) | confidence | justification |
|----|------|------------------|------------|---------------|
| BL-101 | `crc_corruption` | **aucune** trame corrompue acceptée (`crc_errors = 0`) | hypothesis | l'intégrité est un prérequis fonctionnel ; seuil à confirmer sur le comportement réel |
| BL-102 | `transaction_success_ratio` | ≥ 99,9 % | hypothesis | valeur de travail ; **à recalibrer** sur les modes isolés |
| BL-103 | `timeout_count` | 0 timeout en régime nominal | hypothesis | un timeout en nominal signale une contention ou un défaut de bring-up |
| BL-104 | `latency_p99_degradation` | P99 du mode `concurrent` ≤ **2 ×** P99 du mode isolé correspondant | hypothesis | facteur de travail, **explicitement arbitraire** tant qu'aucune référence n'existe ; à fixer après les modes isolés |
| BL-105 | `irq_latency_p95` | seuil **non fixé** en B3 | hypothesis | aucune donnée pour proposer un seuil honnête ; sera dérivé des modes isolés en B4 |
| BL-106 | `no_lockup` | aucun blocage > 100 ms sur une transaction | hypothesis | traduit le symptôme « blocage ponctuel » que la moyenne masquerait |

> BL-104 et BL-105 sont **délibérément non tranchés** : proposer un seuil chiffré
> sans référence mesurée serait un faux plancher de rigueur. Ils seront dérivés
> des modes isolés (`screen-only`, `module-only`) en B4.

## C — Paramètres de charge expérimentale

Ces valeurs sont **appliquées**, jamais jugées. Elles reflètent les profils
déclaratifs de `firmware/experiment-bench/scenarios/l1_spi_bringup/`.

| id | name | value | confidence | justification |
|----|------|-------|------------|---------------|
| BL-201 | `screen_spi_clock_hz` | 20 000 000 | hypothesis | ordre de grandeur d'un afficheur ; **à confirmer** avec le matériel réel |
| BL-202 | `module_spi_clock_hz` | 8 000 000 | hypothesis | trafic CX-Bus plus lent que l'afficheur ; à confirmer |
| BL-203 | `screen_packet_size` | 256 octets | hypothesis | rafale large (plafond de payload de la trame) |
| BL-204 | `module_packet_size` | 32 octets | hypothesis | échange court représentatif |
| BL-205 | `transaction_count` | 512 par source | hypothesis | compromis durée/représentativité ; à ajuster selon la dispersion |
| BL-206 | `screen_inter_delay_ticks` | 2 | hypothesis | rafales rapprochées |
| BL-207 | `module_inter_delay_ticks` | 20 | hypothesis | interrogation périodique |
| BL-208 | `repetitions_per_case` | ≥ 5 | hypothesis | à confirmer selon la dispersion observée |
| BL-209 | `fault_injection` | désactivée | normative | la campagne mesure un comportement **nominal** ; la chaîne de détection est validée séparément (gate matériel) |

## Ce que cette baseline ne fait pas

- N'autorise **aucun** run ni acquisition.
- N'est **pas** approuvée : `status = draft`, sans `approved_by`/`approved_at`.
- Ne contient **aucune** valeur `measured`.
- Ne fige **aucun** composant, MPN ni brochage.

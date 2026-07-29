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

- `baseline_id` : **`BL-EXP-L1-BRINGUP-001`**
- `baseline_draft_version` : **0** (brouillon ; le premier `baseline-record.json`
  **approuvé** produit en B4 portera la version **1** et référencera ce brouillon)
- Référence machine : les six `campaign-definition` portent `baseline_id`,
  `baseline_draft_version` et `baseline_draft_ref` pointant vers cette page.

Cycle : **B3 → `draft`** · **B4 → revue → instanciation → approbation → exécution.**

## Classes de confiance

`normative` (imposée par une spécification externe ou par le socle) · `measured`
(issue d'une mesure qualifiée) · `hypothesis` (proposée, non vérifiée).

> À ce stade, **aucune** valeur n'est `measured` : aucune mesure n'existe.

## Catégories de champs `[BL]`

| Catégorie | Rôle | Effet |
|-----------|------|-------|
| **A — Invariants de validité du banc** | conditions sans lesquelles le run ne veut rien dire | violation ⇒ run `INVALID` |
| **B — Critères de verdict technique** | ce qui décide `PASS`/`FAIL` | violation ⇒ `FAIL` |
| **C — Paramètres de charge expérimentale** | ce qui est *appliqué*, pas jugé | ne décide d'aucun verdict |

La **fréquence SPI** et le **nombre de transactions** sont des **paramètres de
charge (C)** ; « aucune trame corrompue acceptée » est un **critère de verdict (B)**.

> **Applicabilité par cas.** Tous les `[BL]` de catégorie B **ne s'appliquent pas
> à tous les cas** : voir la [matrice d'applicabilité](EXP-L1-BRINGUP-001.md#5-matrice-dapplicabilite-par-cas).
> Les modes isolés ne sont **jamais** jugés sur les critères de dégradation
> concurrente.

## A — Invariants de validité du banc

| id | name | value (proposée) | confidence | justification |
|----|------|------------------|------------|---------------|
| BL-001 | `supply_voltage_range` | 3,30 V ± 5 % | hypothesis | plage nominale usuelle du véhicule de référence ; **à vérifier** sur le banc |
| BL-002 | `ground_topology` | masse commune, point unique | hypothesis | évite les boucles de masse faussant l'intégrité SPI |
| BL-003 | `max_wire_length` | ≤ 100 mm par ligne SPI | hypothesis | limite les réflexions ; **à confirmer** à la fréquence retenue |
| BL-004 | `build_reproducible` | arbre git propre, ou diff archivé et haché | normative | exigé par `promote_to_measured` (socle) |
| BL-005 | `time_correlation_verified` | traces hôte/module alignées sur la base de temps autoritaire via marqueur de synchronisation ; dérive bornée et consignée | hypothesis | sans alignement explicite, aucune latence **inter-cartes** n'est interprétable |
| BL-006 | `instrumentation_overhead` | ≤ 2 % du temps de transaction | hypothesis | l'instrumentation ne doit pas produire l'effet mesuré ; **à mesurer** en B4 |
| BL-007 | `ambient_temperature` | 20–27 °C, consignée par run | hypothesis | limite la dérive thermique entre runs |
| BL-008 | `analyzer_channels_timing` | ≥ **4** voies : `SCK`, `CS_screen`, `CS_module`, `IRQ` | hypothesis | capture **temporelle minimale** : permet latences et occupation de bus, **pas** le décodage des données |
| BL-009 | `analyzer_channels_protocol` | ≥ **6** voies (8 recommandé) : + `MOSI`, `MISO` | hypothesis | capture **protocolaire complète** : seule façon de relier transactions, numéros de séquence et rejets CRC au niveau bus |
| BL-010 | `analyzer_sample_rate_min` | ≥ **4 × f_SPI_max** pour le décodage ; ≥ **10 × f_SPI_max** pour l'analyse temporelle/gigue | hypothesis | critère **exécutable** : à `f_SPI_max` = 20 MHz ⇒ ≥ 80 MS/s (décodage) et ≥ 200 MS/s (temporel). Un appareil à 24 MS/s est **insuffisant** ; Nyquist (2 ×) ne suffit pas à situer les fronts |
| BL-011 | `time_authority` | **analyseur logique** = base de temps autoritaire pour toute latence inter-cartes | normative | deux horloges MCU non corrélées ne permettent pas de soustraire des instants ; cf. [base de temps](EXP-L1-BRINGUP-001.md#6-base-de-temps-et-appariement) |
| BL-012 | `spi_separated_same_dut` | second contrôleur SPI matériel affecté à des broches **exposées** de la **même** XIAO ESP32-S3 | hypothesis | la comparaison n'est contrôlée que si le **DUT est identique** entre variantes ; **à prouver** au gate B4 |

## B — Critères de verdict technique

| id | name | value (proposée) | confidence | justification |
|----|------|------------------|------------|---------------|
| BL-101 | `unexpected_crc_rejects` | **0** rejet CRC pendant le trafic **nominal** | hypothesis | en l'absence de faute injectée (BL-209), tout rejet signale une contention ou un défaut de bring-up |
| BL-107 | `accepted_corrupted_frames` | **0** trame corrompue **acceptée** | hypothesis | vérifié par **numéro de séquence attendu** + **payload recalculé** depuis le profil déclaratif (motifs déterministes) ; c'est un contrôle **de contenu**, distinct d'un compteur de rejets |
| BL-102 | `transaction_success_ratio` | ≥ 99,9 % | hypothesis | valeur de travail ; **à recalibrer** sur les modes isolés |
| BL-103 | `timeout_count` | 0 timeout en régime nominal | hypothesis | un timeout en nominal signale une contention ou un défaut de bring-up |
| BL-104 | `latency_p99_degradation_factor` | **valeur ouverte — à fixer en B4** | hypothesis | seuil **non chiffré** : il sera **dérivé des modes isolés** (P99 de référence) une fois ceux-ci mesurés. Aucun facteur n'est retenu à ce stade |
| BL-105 | `irq_latency_p95` | **valeur ouverte — à fixer en B4** | hypothesis | aucune donnée pour proposer un seuil honnête ; sera dérivé des modes isolés |
| BL-106 | `no_lockup` | aucun blocage > 100 ms sur une transaction | hypothesis | traduit le symptôme « blocage ponctuel » que la moyenne masquerait |

> **BL-104 et BL-105 sont réellement ouverts** : leur `value` est *à fixer en B4*,
> pas un chiffre déguisé en hypothèse. Les instancier exige les P99 de référence
> des modes `screen-only` et `module-only`. Tant qu'ils ne sont pas instanciés,
> **aucun cas `concurrent` ne peut recevoir de verdict `PASS`/`FAIL`** sur la
> dégradation (au mieux `INCONCLUSIVE` sur ce critère).

### Pourquoi deux critères CRC distincts

`unexpected_crc_rejects = 0` (BL-101) et `accepted_corrupted_frames = 0` (BL-107)
**ne sont pas équivalents** :

- BL-101 constate qu'aucune trame **n'a été rejetée** pour CRC. Cela ne démontre
  **pas** qu'aucune trame corrompue **n'a été acceptée** (un CRC peut, en théorie,
  ne pas détecter une corruption ; un défaut d'implémentation peut acquitter à
  tort).
- BL-107 le vérifie **positivement**, par le contenu : séquence attendue et
  payload recalculé depuis le profil déclaratif.
- Que la **chaîne de rejet fonctionne** est prouvé **séparément**, par le
  [gate matériel](hardware-gate.md) (corruption **volontaire** ⇒ rejet observé),
  et non par la campagne, qui reste **nominale** (BL-209).

## C — Paramètres de charge expérimentale

Appliqués, jamais jugés. Reflètent les profils déclaratifs de
`firmware/experiment-bench/scenarios/l1_spi_bringup/`.

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

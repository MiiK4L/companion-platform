<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# EXP-L1-BRINGUP-001 — contention SPI écran / CX-Bus

> **Statut : Proposé — DÉFINITION SEULE (lot B3).** Cette page **décrit** une
> campagne ; elle **n'en exécute aucune**. **Aucune mesure, aucun flash, aucun
> run, aucune donnée `[M]`, aucune ADR.** L'exécution relève de **B4**, soumise
> au [gate matériel](hardware-gate.md).
>
> `DEC-L1-001` reste **Ouvert**. Rien ici ne fige un brochage, un composant ou
> une topologie.

## 1. Question expérimentale

> Sur le véhicule de référence, **le partage du bus SPI entre l'afficheur et le
> CX-Bus dégrade-t-il le service au point de le rendre inacceptable**, comparé à
> deux bus SPI séparés ?

Rattachement : [`DEC-L1-001`](../../decisions-register.md) (budget GPIO/bus,
option de partage) et risque **R2** (contention SPI) du
[registre des risques](../../risk-register.md).

Cette campagne **n'arbitre pas** `DEC-L1-001`. Au mieux, elle produit des runs
`[M]` individuellement qualifiés ; l'arbitrage exige un `evidence_bundle`
reproductible (cf. [cycle d'une campagne](../campaign-workflow.md)).

## 2. Variantes et modes de charge

**Deux variantes** (axe structurant, `variant_id`) — les seules topologies évaluées :

| `variant_id` | Topologie |
|--------------|-----------|
| `spi-shared` | afficheur **et** CX-Bus sur le **même** bus SPI, chip-selects distincts |
| `spi-separated` | **deux** bus SPI distincts |

**Trois modes de charge** (`load_mode`) — ce sont des **contrôles**, *pas* de
nouvelles variantes architecturales. Chaque variante les subit tous les trois :

| `load_mode` | Afficheur | Simulateur CX-Bus | Rôle |
|-------------|-----------|-------------------|------|
| `screen-only` | actif | inactif | référence de l'afficheur seul |
| `module-only` | sans trafic | actif | référence du module seul |
| `concurrent` | actif | actif | situation évaluée |

**Comparaison principale** : `spi-shared/concurrent` **vs**
`spi-separated/concurrent`. Les modes isolés fournissent les **références de
performance** sans lesquelles le mode concurrent n'est pas interprétable : ils
permettent de distinguer un problème **intrinsèque à l'afficheur**, un problème
**intrinsèque au module**, et une dégradation **réellement causée par la
concurrence** sur le bus partagé.

**6 cas** = 2 variantes × 3 modes. Ils sont déclarés dans
`firmware/experiment-bench/scenarios/l1_spi_bringup/` (table `l1_cases`) et
adossés à 6 `campaign-definition` sous
`tools/measurement/campaigns/EXP-L1-BRINGUP-001/`.

Les deux topologies utilisent **exactement les mêmes profils de charge** (vérifié
par test) : seule la topologie change, sinon la comparaison ne serait pas
contrôlée.

## 3. Observables

### 3.1 Bruts — autoritaires, jamais recalculés

Ce sont les données **conservées** ; toute métrique dérivée doit pouvoir être
**recalculée** à partir d'elles.

| Brut | Description |
|------|-------------|
| Horodatages d'événements | `TX_BEGIN`, `TX_END`, `IRQ`, `TIMEOUT`, `CRC_ERROR`, `RESET` (ticks) |
| Séquences de transactions | numéro de séquence de trame, ordre effectif |
| Statuts | état de transaction (`DONE`/`TIMEOUT`), statut de transport |
| Compteurs | `tx_ok`, `tx_failed`, `bytes_transferred`, `latency_samples` |
| Erreurs CRC | `crc_errors` (rejets réels de trames) |
| IRQ | IRQ **émises** (module) et **reçues** (hôte), séparément |
| Transitions de CS | assertions/relâchements par chip-select |

> **Conservation de la distribution (contrainte de conception).** Le banc doit
> conserver assez de données brutes pour reconstruire la **distribution** des
> latences, et pas seulement une somme. Une moyenne seule masque les **blocages
> ponctuels**, qui sont précisément le symptôme attendu d'une contention.
> Minimum requis : **min, max, médiane, P95, P99, nombre d'échantillons**.
>
> **Conséquence pour B4** (à traiter dans une PR d'évolution du socle, séparée
> de la campagne) : les compteurs agrégés livrés en B2 (`latency_sum_ticks`,
> `latency_max_ticks`) **ne suffisent pas** — il faut exporter les latences
> par transaction (ou un histogramme à bornes déclarées). Ce manque est
> **identifié ici**, non contourné.

### 3.2 Dérivés — recalculés, avec formule et provenance

Chaque métrique dérivée déclare sa **formule** et les **bruts** dont elle
provient. Aucune n'est autoritaire.

| Dérivé | Formule | Provenance (bruts) |
|--------|---------|--------------------|
| Débit | `bytes_transferred / durée_écoulée` | compteurs + horodatages |
| Taux de succès | `tx_ok / (tx_ok + tx_failed)` | compteurs |
| Latence (par transaction) | `t(TX_END) − t(TX_BEGIN)` | horodatages |
| Latence min/max/médiane/P95/P99 | quantiles de la série de latences | série de latences |
| Gigue | écart-type (et écart inter-quartile) des latences | série de latences |
| Taux d'erreur CRC | `crc_errors / transactions_émises` | compteurs |
| Latence d'IRQ | `t(IRQ_reçue) − t(IRQ_émise)` | horodatages appariés |

## 4. Protocole d'exécution

### 4.1 Répétitions, ordre et randomisation

- **Répétitions** : `n ≥ 5` runs par cas (6 cas ⇒ ≥ 30 runs), valeur à confirmer
  en B4 selon la dispersion observée.
- **Ordre alterné/randomisé** — l'ordre ne doit **jamais** être systématiquement
  `shared` puis `separated`. L'ordre des variantes **et** des modes est alterné
  ou tiré au sort, et l'ordre effectif est **consigné dans chaque run**.
- **Remise à zéro complète entre runs** : mise hors tension puis sous tension des
  deux cartes, réinitialisation des compteurs, re-flash non requis mais build
  identique vérifié par empreinte.
- Ces règles visent à limiter les effets de **chauffe**, d'**état résiduel**,
  d'**apprentissage opérateur** et de **dérive du banc**.

### 4.2 Conditions d'invalidation d'un run

Un run est marqué `INVALID` (et non `FAIL`) si : le câblage diffère du brochage
consigné · l'alimentation sort des bornes déclarées · le build ne correspond pas
au `build_manifest` · l'arbre git est sale sans diff archivé · un invariant de
validité du banc (§ baseline, catégorie A) n'est pas respecté · l'instrumentation
a perturbé la mesure de façon détectable.

### 4.3 Règle de verdict

Le verdict porte sur **un run** et se rattache aux `[BL]` de la catégorie
« critères de verdict » :

- **PASS** — tous les critères de verdict sont satisfaits sur le run.
- **FAIL** — au moins un critère de verdict est violé.
- **INCONCLUSIVE** — dispersion ou données insuffisantes pour trancher ; **jamais**
  converti en PASS/FAIL.
- **INVALID** — cf. §4.2.
- **NOT_RUN** — non exécuté.

> Un run `PASS` **n'arbitre pas** `DEC-L1-001` : voir §1.

## 5. Actions physiques et matériel

Voir [matériel et actions physiques](materials-and-physical-actions.md) et le
[brochage candidat](pinout-candidate.md) (candidat, **non figé**).

## 6. Baseline

Voir [baseline brouillon](baseline-draft.md) — statut **`draft`**, **n'autorise
aucun run**. Instanciation, revue et approbation en **B4**.

## 7. Ce que cette campagne ne fait pas

- N'exécute rien, ne mesure rien, ne flashe rien : **aucun run**, aucun `RAW`.
- Ne crée **aucune ADR** (ni ADR-0013) ; `DEC-L1-001` reste **Ouvert**.
- Ne retient **aucun composant ni MPN** ; ne fige **aucun brochage**.
- Ne produit **aucune** donnée `[M]` et **aucun** `evidence_bundle`.

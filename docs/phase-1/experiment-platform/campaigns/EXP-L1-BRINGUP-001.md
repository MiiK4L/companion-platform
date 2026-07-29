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

| id | Dérivé | Formule | Provenance (bruts) |
|----|--------|---------|--------------------|
| `M-THR` | Débit | `bytes_transferred / durée_écoulée` | compteurs + horodatages |
| `M-SUC` | Taux de succès | `tx_ok / (tx_ok + tx_failed)` | compteurs |
| `M-LAT` | Latence par transaction | `t(TX_END) − t(TX_BEGIN)` | horodatages |
| `M-DIST` | Distribution : min, max, médiane, P95, P99, n | quantiles de la série `M-LAT` | série de latences |
| `M-JIT` | Gigue | écart-type et écart inter-quartile | série de latences |
| `M-CRCR` | Taux de rejet CRC | `crc_errors / transactions_émises` | compteurs |
| `M-INTG` | Intégrité de contenu | trames acceptées dont séquence **et** payload recalculé correspondent | séquences + payload attendu |
| `M-IRQL` | Latence d'IRQ | `t(IRQ_reçue) − t(IRQ_émise)`, **sur la base de temps autoritaire** | transitions de CS/IRQ (§6) |

Le suffixe `(screen)` / `(module)` désigne la source de charge concernée.

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

Le verdict porte sur **un run** et ne se rattache **qu'aux `[BL]` applicables à
son cas** (§5) :

- **PASS** — tous les critères de verdict **applicables au cas** sont satisfaits.
- **FAIL** — au moins un critère **applicable** est violé.
- **INCONCLUSIVE** — dispersion ou données insuffisantes pour trancher ; **jamais**
  converti en PASS/FAIL. C'est notamment le cas d'un critère applicable dont le
  seuil n'est **pas encore instancié** (BL-104, BL-105).
- **INVALID** — cf. §4.2.
- **NOT_RUN** — non exécuté.

> Un critère **non applicable** (`N/A`) n'est **ni satisfait ni violé** : il est
> exclu du calcul du verdict. Le considérer implicitement comme « à satisfaire »
> rendrait le verdict ambigu.

> Un run `PASS` **n'arbitre pas** `DEC-L1-001` : voir §1.

## 5. Matrice d'applicabilité par cas

Les modes de contrôle ne produisent pas les mêmes observables : `screen-only`
n'a ni trafic module ni IRQ module exploitable, `module-only` n'a pas de trafic
afficheur, et **seul `concurrent` permet d'évaluer la contention**. Chaque
définition de campagne ne référence donc **que ses critères applicables**.

| Cas (les deux topologies) | `[BL]` applicables | Métriques obligatoires | `N/A` |
|---------------------------|--------------------|------------------------|-------|
| `screen-only` | **A** : tous · **B** : BL-101, BL-107, BL-103, BL-106 | `M-LAT(screen)`, `M-DIST(screen)`, `M-THR(screen)`, `M-JIT(screen)`, `M-CRCR(screen)`, `M-INTG(screen)` | BL-102, **BL-104**, **BL-105** · `M-IRQL`, métriques `(module)` |
| `module-only` | **A** : tous · **B** : BL-101, BL-107, BL-102, BL-103, BL-106 | `M-LAT(module)`, `M-DIST(module)`, `M-THR(module)`, `M-JIT(module)`, `M-SUC(module)`, `M-CRCR(module)`, `M-INTG(module)`, `M-IRQL` | **BL-104**, **BL-105** · métriques `(screen)` |
| `concurrent` | **A** : tous · **B** : **tous**, y compris BL-104 et BL-105 | toutes les métriques des deux sources, dont `M-IRQL` et la comparaison de dégradation | — |

**Lecture.** Les modes isolés sont jugés sur leur **intégrité propre** (rejets
CRC, contenu, timeouts, blocages, et succès module là où il existe) ; ils ne sont
**jamais** jugés sur les **critères de dégradation concurrente** (BL-104,
BL-105), qui n'ont pas de sens sans concurrence. Leur rôle premier est de fournir
les **références quantitatives** (P99 isolés) qui permettront d'**instancier**
ces deux critères en B4.

**Conséquence d'ordonnancement** : les modes isolés doivent être exécutés **et
analysés avant** que les cas `concurrent` puissent recevoir un verdict sur la
dégradation.

## 6. Base de temps et appariement

Deux horloges MCU non corrélées ne permettent pas de soustraire des instants :
`t(IRQ_reçue) − t(IRQ_émise)` calculé entre l'hôte et le module serait
ininterprétable. La campagne définit donc une **autorité temporelle** explicite.

| Rôle | Source | Statut |
|------|--------|--------|
| **Autoritaire** — latences **inter-cartes** (IRQ, corrélation CS/transaction) | **analyseur logique** | fait foi (BL-011) |
| Secondaire — ordre et latences **intra-carte**, compteurs | horodatages logiciels | **non autoritaires** pour toute grandeur croisée |

**Alignement des traces.** Un **signal GPIO de synchronisation** (marqueur) est
émis en début de run, et périodiquement, sur une voie capturée par l'analyseur ;
il sert de repère commun aux traces logicielles et à la trace bus. La dérive
résiduelle est bornée et consignée (BL-005).

**Règle d'appariement.** Chaque trame porte un **numéro de séquence** ; la trame
d'acquittement reprend ce numéro. Une IRQ est appariée à la transaction de **même
séquence**. Si l'appariement échoue (séquence manquante, dupliquée ou ambiguë),
l'échantillon est **écarté et compté** — jamais rattaché arbitrairement. Un taux
d'échantillons écartés au-delà de ce que la revue juge acceptable rend le run
`INCONCLUSIVE`.

## 7. Actions physiques et matériel

Voir [matériel et actions physiques](materials-and-physical-actions.md) et le
[brochage candidat](pinout-candidate.md) (candidat, **non figé**).

## 8. Baseline

Voir [baseline brouillon](baseline-draft.md) — statut **`draft`**, **n'autorise
aucun run**. Instanciation, revue et approbation en **B4**.

## 9. Ce que cette campagne ne fait pas

- N'exécute rien, ne mesure rien, ne flashe rien : **aucun run**, aucun `RAW`.
- Ne crée **aucune ADR** (ni ADR-0013) ; `DEC-L1-001` reste **Ouvert**.
- Ne retient **aucun composant ni MPN** ; ne fige **aucun brochage**.
- Ne produit **aucune** donnée `[M]` et **aucun** `evidence_bundle`.

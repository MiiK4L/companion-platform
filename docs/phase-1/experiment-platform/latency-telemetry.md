<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Télémétrie de latence (lot B4.1)

> **Statut : Proposé.** Évolution du **socle**, indépendante de toute campagne :
> aucun matériel, aucun flash, aucune mesure, aucun run. B4.1 **n'autorise pas**
> l'ouverture de la campagne EXP-L1-BRINGUP-001, qui reste soumise au
> [gate matériel](campaigns/hardware-gate.md).

Les compteurs agrégés livrés en B2 (`latency_sum_ticks`, `latency_max_ticks`) ne
permettent pas de reconstruire une distribution : une moyenne seule masque les
**blocages ponctuels**, précisément le symptôme attendu d'une contention. B4.1
ajoute une **série brute par transaction**, seule source autoritaire des
quantiles.

## Hiérarchie des artefacts

| Grandeur | Autoritaire | Rôle des autres |
|---|---|---|
| Latence **de transaction** | **série brute** exportée (`raw/samples/`) | — |
| Latence **inter-cartes** (IRQ, CS↔transaction) | **capture analyseur** (BL-011) | série logicielle = corroboration |
| Quantiles, moyenne, dispersion | **recalculés côté outillage** depuis la série | — |
| Histogramme | **recalcul outillage** | histogramme MCU = **secondaire**, comparé |

La vue CSV (`sequence_id`, `t_start_ticks`, `t_end_ticks`, `latency_ticks`,
`status`, `variant`, `mode`) est une **vue normalisée** avec provenance, jamais
la source.

## Mémoire bornée : flux, pas stockage

```text
moteur ──1 enregistrement/transaction──▶ ring BORNÉ (K) ──▶ trame CRC ──▶ puits abstrait
                                              │
                                     saturation ⇒ producer_drop + MARQUEUR DE LACUNE
```

- Le MCU ne conserve **jamais** `n` échantillons : empreinte **O(K)**,
  indépendante du nombre de transactions.
- **Non bloquant par contrat** : si le tampon est plein, l'échantillon est perdu
  et le moteur poursuit. Il n'attend **jamais** le puits.
- Le puits est **abstrait** (`bench_telemetry_sink_t`) : **aucune dépendance** à
  un port série, à un OS ou à un transport. Les adaptateurs matériels viendront
  plus tard sans toucher au cœur.
- Écriture **atomique par trame** : la trame est acceptée entière ou refusée
  entière — une trame partielle corromprait le flux.

### La latence n'est pas transportée

Le fil porte `t_start` et `t_end` en **ticks bruts** ; `latency` est **recalculée**
avec l'arithmétique **wrap-safe** du cœur. Transporter une valeur redondante
autoriserait une contradiction entre elle et ses entrées. L'en-tête de flux
déclare **largeur du compteur**, **fréquence de tick**, **identifiant d'horloge**
et **politique de wrap**.

## Deux pertes, jamais fusionnées

| Perte | Origine | Détection |
|---|---|---|
| `producer_drop` | échantillon perdu **avant** sérialisation (ring saturé) | compteur saturant + **marqueur de lacune** situé dans le flux |
| `transport_gap` | trame sérialisée mais **absente ou rejetée** côté capture | **discontinuité** des numéros de séquence de trame |

Les confondre masquerait la cause : saturation du producteur ou défaut de
transport. Elles restent donc **deux compteurs distincts**.

## Réconciliation — un seul statut terminal par échantillon

```text
issued = ok + timeout + rejected + unpaired + duplicate + out_of_order + producer_drop
```

Les `transport_gap` **n'en font pas partie** : ce ne sont pas des transactions
exécutées par le moteur, mais des artefacts perdus **après** émission ; ils sont
réconciliés séparément par les numéros de séquence. Si l'identité ne se ferme
pas, le run n'est **pas** rapporté comme un résultat.

## Quantiles — méthode figée et archivée

`quantile_method = nearest-rank-inclusive-v1`

```text
P(p) = x[ceil(p × n)]     sur les valeurs triées, indexées à partir de 1
```

- Rang calculé en **entiers** (`ceil(num·n/den)`), jamais en flottants.
- `n = 0` ⇒ quantile **indisponible** (`null`), jamais une valeur inventée.
- `p` borné à `0 < p ≤ 1`.
- P50/P95/P99 sur la **même population filtrée**.
- Changer de méthode impose une **nouvelle version de nom** : aucune modification
  silencieuse.

**Les timeouts sont exclus** de la distribution principale — les inclure
écraserait le P99 vers le budget de timeout. Ils sont publiés à part
(`timeout_count`, `timeout_ratio`, `timeout_budget_ticks`), **à côté** des
quantiles, pour qu'un P99 flatteur ne masque jamais un taux de timeout élevé.

### Dispersion : pas de « gigue »

Le terme générique **n'est pas employé** : il recouvre au moins trois grandeurs
distinctes (écart-type des latences, variation inter-échantillons, écart à une
période attendue). B4.1 implémente **une seule**, nommée explicitement :
`latency_stddev_population` (écart-type de **population**, variance calculée en
rationnel exact).

## Complétude de la série

```text
series_completeness = complete   si producer_drop = 0 ET transport_gap = 0
                    = incomplete sinon
```

> **Les pertes ne sont pas aléatoires.** Un puits saturé perd pendant les
> **rafales**, c'est-à-dire pendant la contention même que l'on mesure. Une
> perte, même faible, biaise donc la **queue** de distribution dans le sens
> favorable.

Conséquence **mécanique** (et non déclarative) : le bloc d'analyse porte
`quantiles_verdict_eligible`, **faux** dès qu'une perte existe. Une série
`incomplete` ne fonde **aucun** verdict sur P95/P99 ; par défaut, le run est
`INVALID`. Un test dédié vérifie qu'une perte corrélée aux latences hautes est
bien **signalée** plutôt que publiée comme un bon résultat.

## Histogramme — optionnel, désactivé par défaut

Vue dérivée et bornée, utile au diagnostic direct et aux contraintes mémoire.
Bornes **déclaratives et versionnées** dans le profil ; classes `[edges[i],
edges[i+1])` (borne basse incluse, haute exclue) ; accumulation **saturante** ;
`underflow`, `overflow`, `sample_count` obligatoires.

- **Jamais** de P95/P99 précis depuis l'histogramme : au mieux un **intervalle**
  de classe, insuffisant pour un critère de verdict.
- Lorsqu'il est activé, il est **comparé** au recalcul outillage ; toute
  divergence est un **défaut** à investiguer, pas un arbitrage.

## Vérification

| Niveau | Contenu |
|---|---|
| C (`ctest`) | codec, ring borné (pertes comptées et **localisées**), histogramme (convention, saturation, configurations invalides), flux (trames CRC, séquence monotone, refus du puits) |
| Python (`unittest`) | quantiles + **cas limites** (`n=0,1,2`, valeurs égales, rang exactement sur une frontière), wrap-safe, réconciliation, complétude, histogramme, **biais de perte** |
| **Golden inter-langage** | `golden/telemetry/stream.hex` est produit par l'encodeur **C** et décodé par **Python** ; le test C le reproduit **octet à octet**. Source unique : toute dérive entre les deux implémentations échoue |

## Périmètre

B4.1 **ne** contient : aucune implémentation matérielle, aucun flux concurrent
(B4.2), aucun run, aucune donnée `[M]`, aucune ADR. Les seuils de perte
acceptable et de résolution d'horloge sont proposés comme **candidats** pour la
**baseline v1** (B4.7a) — le brouillon v0 déjà fusionné n'est pas modifié.

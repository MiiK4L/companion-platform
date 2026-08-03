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
- **Ordre chronologique garanti** : une perte survient forcément **après** les
  échantillons déjà présents dans le tampon. Le marqueur n'est donc émis
  qu'une fois ceux-ci drainés — jamais avant.

```text
tampon : seq 0,1,2,3   puis pertes 4,5
flux   : SAMPLE(0..3) PUIS GAP(2, après seq 3)      ← et non l'inverse
```

- **Consommation transactionnelle du marqueur** (`peek` puis `commit`) : un refus
  du puits ne détruit pas l'information de perte, qui est **réémise** au drainage
  suivant.

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
| `transport_gap` | trame sérialisée mais **absente ou rejetée** côté capture | **discontinuité** des numéros de séquence, bornée par la **clôture** |

Les confondre masquerait la cause : saturation du producteur ou défaut de
transport. Elles restent donc **deux compteurs distincts**.

## Clôture du flux — sans elle, rien ne prouve la fin

Une perte des **dernières** trames serait indétectable : aucune trame ultérieure
ne révélerait leur absence. Le flux se termine donc par une **clôture** portant
`last_stream_seq`, `frames_attempted`, `frames_accepted`, `frames_refused` et
`samples_attempted`.

```text
stream_completeness = complete   si clôture présente, cohérente et aucune trame manquante
                    = incomplete sinon (y compris si la clôture elle-même est perdue)
```

Une absence silencieuse de clôture **ne permet jamais** de déclarer la série
complète : `quantiles_verdict_eligible` passe à faux.

## Numéros de séquence — analyse en ordre de réception

Les numéros sont analysés **dans l'ordre de réception**, en comparaison
**modulaire sur 32 bits**, avec des états distincts : `expected`, `gap`,
`duplicate`, `out_of_order`, `wrap`. Le nombre de trames attendues vient de la
**clôture** ; sans elle, il ne peut pas être établi.

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

### Ce qui est autoritaire, et ce qui ne l'est pas

Les **quantiles** (et min/max) sont autoritaires : entiers exacts en ticks. La
**moyenne** et la **variance** sont archivées sous forme **rationnelle exacte**
(`mean_numerator`/`mean_denominator`, `variance_numerator`/`variance_denominator`)
— seule forme recalculable à l'identique sur toute plateforme. Les valeurs
flottantes correspondantes sont des **vues non autoritaires**, explicitement
listées comme telles dans le bloc d'analyse.

### Largeur de compteur et fréquence

`elapsed_wrap_safe` applique la largeur **déclarée** par le flux (8/16/32/64
bits) : appliquer un masque 64 bits à un compteur 32 bits fausserait toute
latence après rebouclage. Une largeur non déclarée est **refusée**. Si
`tick_hz = 0`, aucune conversion physique n'est possible : aucun verdict exprimé
en secondes ou microsecondes.

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
- **Saturation** : dès qu'une accumulation atteint la capacité d'un compteur, le
  drapeau `saturated` est levé. L'identité
  `sample_count = Σ bin_counts + underflow + overflow` n'est alors plus
  vérifiable : l'histogramme reste utile au diagnostic mais **n'est plus
  réconciliable** et **retire l'éligibilité au verdict**.

## Vérification

| Niveau | Contenu |
|---|---|
| C (`ctest`) | codec ; **ordre** de la lacune (tampon plein, drain partiel, transactions après la lacune, plages multiples, exactitude d'`after_sequence_id`) ; **refus du marqueur puis réémission** ; histogramme (convention, **saturation signalée**, configurations invalides) ; **rejet** des chaînes d'en-tête trop longues ou non imprimables ; **clôture** ; refus du puits |
| Python (`unittest`) | quantiles + **cas limites** ; wrap **par largeur déclarée** (8/16/32/64) ; `tick_hz = 0` ; séquence (lacune, doublon, hors ordre, **wrap uint32**) ; **clôture absente/incohérente/perdue** ; tests **négatifs** du parseur (en-tête tronqué, dupliqué, non premier, chaîne invalide, sample trop court/long, summary dupliqué, histogramme tronqué ou `bin_count` excessif, type inconnu) ; **biais de perte** ; valeurs **rationnelles exactes** |
| **Golden inter-langage** | `golden/telemetry/stream.hex` est produit par l'encodeur **C** et décodé par **Python** ; le test C le reproduit **octet à octet**. Source unique : toute dérive entre les deux implémentations échoue |

## Périmètre

B4.1 **ne** contient : aucune implémentation matérielle, aucun flux concurrent
(B4.2), aucun run, aucune donnée `[M]`, aucune ADR. Les seuils de perte
acceptable et de résolution d'horloge sont proposés comme **candidats** pour la
**baseline v1** (B4.7a) — le brouillon v0 déjà fusionné n'est pas modifié.

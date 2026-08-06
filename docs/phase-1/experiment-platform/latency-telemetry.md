<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Télémétrie de latence (lots B4.1 et B4.2)

> **Statut : Proposé.** Évolution du **socle**, indépendante de toute campagne :
> aucun matériel, aucun flash, aucune mesure, aucun run. Ces lots **n'autorisent
> pas** l'ouverture de la campagne EXP-L1-BRINGUP-001, qui reste soumise au
> [gate matériel](campaigns/hardware-gate.md).
>
> **B4.2 valide UNIQUEMENT le moteur, l'arbitre et l'instrumentation.** La
> simulation ne produit **aucune donnée `[M]`**, **aucune preuve expérimentale**,
> et **aucun élément permettant d'arbitrer `DEC-L1-001`**. Un entrelacement
> simulé n'est pas une contention mesurée.

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

### Réconciliation de la clôture (convention archivée)

Les compteurs du footer sont **capturés avant son émission** ; le `+ 1` des
relations ci-dessous correspond donc **au footer lui-même** :

```text
first_stream_seq                       == 0
footer_frame_seq                       == footer.last_stream_seq
frames_attempted                       == last_stream_seq + 1
frames_accepted + frames_refused + 1   == frames_attempted
decoded_unique_frames + transport_gap  == frames_attempted
samples_attempted                      == summary.issued
```

Chaque relation est vérifiée séparément et exposée dans `footer_checks` : une
seule qui échoue rend la clôture incohérente.

## Machine d'état du flux

L'ordre des messages est **unique**, documenté et testé :

```text
HEADER → (SAMPLE | GAP)* → SUMMARY → HISTOGRAM? → FOOTER
```

Toute transition non prévue est une **erreur de structure** : `SAMPLE` ou `GAP`
après le bilan, `FOOTER` avant le bilan, histogramme avant l'en-tête, message
**après** le footer, en-tête ou bilan dupliqué. En version 3, un **type inconnu
est rejeté** : il n'existe pas de zone d'extension, et le tolérer laisserait un
flux inconnu passer pour éligible.

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

## Éligibilité au verdict — conjonction explicite

Une série n'est **pas** qualifiée complète au seul motif qu'aucune trame ne
semble manquer : sans **bilan obligatoire et réconcilié**, on ignore si toutes
les transactions attendues sont représentées.

```text
quantiles_verdict_eligible =
      summary_present
  AND reconciliation.balanced
  AND summary_footer_consistent
  AND series_complete
  AND stream_complete
  AND gap_localization_complete
  AND histogram_usable_if_required
```

Le bloc d'analyse expose chaque condition **et la liste des conditions
bloquantes** (`eligibility.blocking`) : le lecteur sait *pourquoi* un verdict
est refusé, il n'a pas à le déduire.

- **`gap_localization_complete`** est faux si des plages de pertes ont dû être
  **fusionnées** faute de place, ou si des pertes sont survenues sans capacité de
  localisation déclarée. Le **total** des pertes reste exact, mais leur
  **position** ne l'est plus : le flux reste utile au diagnostic sans prétendre
  savoir où les pertes ont eu lieu.
- **`histogram_usable_if_required`** est faux si un histogramme **annoncé** est
  absent, ou s'il est **saturé**.

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
| C (`ctest`) | codec ; **fusion de plages** signalée et pertes sans capacité de localisation ; **ordre** de la lacune (tampon plein, drain partiel, transactions après la lacune, plages multiples, exactitude d'`after_sequence_id`) ; **refus du marqueur puis réémission** ; histogramme (convention, **saturation signalée**, configurations invalides) ; **rejet** des chaînes d'en-tête trop longues ou non imprimables ; **clôture** ; refus du puits |
| Python (`unittest`) | quantiles + **cas limites** ; wrap **par largeur déclarée** (8/16/32/64) ; `tick_hz = 0` ; séquence (lacune, doublon, hors ordre, **wrap uint32**) ; **clôture absente/incohérente/perdue** ; **machine d'état** (sample/gap après bilan, footer avant bilan, message après footer, histogramme avant en-tête, type inconnu **rejeté**) ; **réconciliation de clôture** (séquence de la trame footer, compteurs, `issued` vs `samples_attempted`, première séquence non nulle) ; **bilan absent ou déséquilibré** ; **pertes non localisables** ; **histogramme annoncé mais absent** ; **biais de perte** ; valeurs **rationnelles exactes** |
| **Golden inter-langage** | `golden/telemetry/stream.hex` est produit par l'encodeur **C** et décodé par **Python** ; le test C le reproduit **octet à octet**. Source unique : toute dérive entre les deux implémentations échoue |

## Périmètre

B4.1 **ne** contient : aucune implémentation matérielle, aucun flux concurrent
(B4.2), aucun run, aucune donnée `[M]`, aucune ADR. Les seuils de perte
acceptable et de résolution d'horloge sont proposés comme **candidats** pour la
**baseline v1** (B4.7a) — le brouillon v0 déjà fusionné n'est pas modifié.


---

# Deux flux concurrents (lot B4.2)

## Séparation génération / arbitrage

Les **producteurs** restent seuls responsables de leur **cadence**, de leurs
**seeds**, de leurs **payloads** et de leurs **IRQ**. L'**arbitre** n'accorde que
le bus : il ne modifie **jamais** le comportement interne d'un producteur.

## Atomicité — un invariant, pas une hypothèse

Une transaction SPI est **atomique** : le bus est acquis **avant** l'assertion du
CS, reste détenu pendant toute la transaction, et n'est libéré qu'**après** sa
fin complète. **Aucune préemption au milieu** d'une transaction ; elle ne peut
intervenir qu'**entre** deux transactions. L'arbitre refuse toute attribution
tant que le bus est détenu, et un test le vérifie mécaniquement.

## Topologie — la seule variable

```text
spi-shared     : 1 arbitre pour les deux producteurs  → contention possible
spi-separated  : 1 arbitre PAR producteur             → bus_wait_ticks == 0
```

Profils, seeds, politique, durée d'occupation et cadence sont **identiques**
entre les deux montages. Les **modes isolés** réutilisent les **mêmes**
producteurs, le second étant simplement **désactivé** — jamais un profil
simplifié. Conséquence testée : `shared/screen-only` et `separated/screen-only`
donnent des résultats **strictement identiques**, puisque sans concurrence la
topologie ne doit rien changer.

## Politiques d'arbitrage

| Politique | Rôle |
|---|---|
| **FIFO** | référence de la campagne ; ordre d'arrivée stable |
| **Round-robin** | référence de la campagne ; alternance, sans famine |
| Priorité fixe | **mode de stress uniquement**, pour valider la détection de famine et ses compteurs. **Pas** la politique de référence |

Les demandes **simultanées** sont départagées par un **rang d'arrivée global**,
ce qui rend le tie-break déterministe et documenté.

## Files et famine

Par producteur : profondeur, **débordements imputés au bon producteur**, âge de
la plus ancienne requête, `max_bus_wait_ticks`,
`requests_over_starvation_threshold`, `queue_overflow_count`.

## Causes de timeout — jamais ambiguës

`none` · `bus_wait` · `peripheral_response` · `transport` · `scheduler`.
Un timeout dû à l'**attente du bus** n'est **jamais** assimilé à un périphérique
silencieux : ce sont deux diagnostics opposés.

## Format de flux v4

**v4 remplace v3 partout** — le moteur mono-flux émet lui aussi du v4
(producteur 0, instants de bus dégénérés). Le parseur **rejette explicitement**
toute version antérieure : il n'existe **aucun fallback silencieux**.

| Champ | Rôle |
|---|---|
| `global_event_seq` | ordre **global**, strictement monotone — capture l'entrelacement |
| `producer_sequence_id` | séquence **locale**, continue par producteur |
| `t_request`, `t_grant`, `t_release`, `t_end` | quatre instants, en ticks bruts |
| `timeout_cause` | cause explicite sur chaque enregistrement terminal |

**Aucune des deux numérotations n'est reconstruite depuis l'autre** : avec deux
producteurs ou une lacune, la déduction serait fausse. Les durées sont
**dérivées**, jamais transportées :

```text
bus_wait_ticks = t_grant   - t_request
bus_hold_ticks = t_release - t_grant
```

Une **lacune conserve l'identité** des producteurs touchés
(`lost_by_producer`) : une perte dans le flux global n'efface jamais **qui** a
perdu. Le **bilan est par producteur**, et la réconciliation doit se fermer
**globalement ET pour chaque producteur** pris isolément.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2A-REMOVE — Retrait du module pendant une transaction

> **Statut : Brouillon (en attente de baselining).** Seuils `[P]/[H]` ; champs
> ouverts `[BL]` à geler avant essai ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md) ·
> [définitions des événements](event-definitions.md).

## Lot & décision visée

Lot 2A · alimente **`DEC-L2-001`** (robustesse au retrait ; **R5**).

## Objectif (une question mesurable)

Si le module est **retiré pendant un échange** I²C **ou** SPI, le Host évite-t-il
le crash (0/200) et **revient-il** à un état connu ?

## Conditions initiales

- Banc d'injection ([hot-plug](../hot-plug.md) §1) ; transaction I²C **ou** SPI
  déclenchée puis **interrompue** par ouverture des lignes/`VMOD` à un instant
  contrôlé (au milieu d'un octet, entre octets, pendant l'ACK…).
- Firmware de test avec timeouts de transaction et détection de présence.

## Instrumentation requise

- Analyseur logique (I²C/SPI) + oscilloscope ; séquenceur d'ouverture commandé.

## Montage / fixture

Ouverture des contacts simulée par le banc d'injection à des **phases variées**
de la transaction ; horodatage de l'instant d'ouverture.

## Conditions d'alimentation

Voir la [matrice partagée](../electrical-risk-analysis.md) (§2). **Rail observé :
lignes de bus + `VMOD`.** Le bus est sur `3V3` **régulé** ; les coins batterie ne
s'appliquent qu'en variante **VB** (`VMOD` = `VBAT`). Variantes : **VA** (défaut,
coins régulé ±5 %) ; **VB** (batterie 3,0 / 4,2 V). USB présent et absent.

## Fréquence et charge des bus

I²C 100/400 kHz ; SPI à la fréquence écran **[H]** (à documenter) ; retrait
déclenché à ≥ 4 phases distinctes de la trame.

## Seuils de réussite / échec chiffrés

Verdicts instrumentés : voir [définitions des événements](event-definitions.md)
(crash, blocage bus, état connu).

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Crash Host | 0 sur le total de retraits (def. instrumentée) | **[P]** |
| Blocage pilote (bus non libéré) | 0 | **[P]** |
| Retour à un état connu | 100 % ; délai de reprise ≤ `[BL]` ms | **[P]/[BL]** |

### Champs à finaliser au baselining (`[BL]`)

- **Délai de reprise max** (ms) vers l'« état connu » (source : timeout pilote + marge).
- **Liste précise** des états HW/SW de l'« état connu » (voir
  [définitions des événements](event-definitions.md)).

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 · **`n_runs`** = **200 retraits par (DUT × campagne)** ·
  **`n_campaigns`** ≥ 2 · **`n_cycles`** = n/a.
- **Clarification** : les 200 retraits s'entendent **par DUT et par campagne**
  (pas 200 au total) ; **total** = `n_dut` × 200 × `n_campaigns` × `n_bus` (I²C/SPI),
  figé au baselining.
- **Répartition** : 200 répartis sur **≥ 4 phases** de trame (milieu d'octet,
  inter-octets, pendant l'ACK, en fin de trame), pour I²C **et** SPI.
- **Ordre / randomisation** : phases **randomisées** (graine journalisée).
- **Repos** : non requis (non destructif) ; pause si anomalie.
- **Reprise après échec** : crash/blocage → arrêt, journalisation, réinitialisation,
  reprise non rétroactive.
- **Méthode** : min/max/percentiles du délai de reprise.

## Critères d'arrêt immédiat

- Blocage durable du pilote/bus non récupérable.
- Reset Host non prévu répété ; corruption des autres périphériques.

## Remise en état entre campagnes

Réinitialiser firmware et contrôleurs de bus, vérifier libération des lignes,
recharger l'état de référence, revérifier la synchro d'horodatage.

## Éléments susceptibles d'être détruits

Faible risque électrique ; usure de la fixture d'ouverture.

## Données brutes attendues

Traces I²C/SPI avec marqueur d'ouverture, journal état Host avant/après, délais
de reprise, par phase/essai ; SHA-256 par fichier au rapport.

## Sécurité opérateur

Faibles tensions ; précautions ESD standard.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2A-REMOVE — Retrait du module pendant une transaction

> **Statut : Proposé (figé avant essai).** Seuils **[P]/[H]** ; **aucune [M]**.
> Modèle : [protocole de test](../../templates/test-protocol-template.md).

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

## Plage de tension

`3V3` ±5 % ; batterie ≈ 3,0 V / ≈ 4,2 V **[DS]** (coins).

## Fréquence et charge des bus

I²C 100/400 kHz ; SPI à la fréquence écran **[H]** (à documenter) ; retrait
déclenché à ≥ 4 phases distinctes de la trame.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Crash Host | 0 sur ≥ 200 retraits | **[P]** |
| Blocage pilote (bus non libéré) | 0 | **[P]** |
| Retour à un état connu | 100 % ; délai ≤ valeur à fixer | **[P]** |

## Reproductibilité

`n_dut` ≥ 2 · `n_runs` = 200 retraits (répartis sur les phases) · `n_campaigns`
≥ 2 ; méthode min/max/percentiles du délai de reprise.

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

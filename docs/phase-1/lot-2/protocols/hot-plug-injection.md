<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2A-HOTPLUG — Hot-plug par banc d'injection (indépendant du connecteur)

> **Statut : Proposé (figé avant essai).** Étudie le hot-plug **électrique** via
> un **banc d'injection**, **sans connecteur final** (connecteur → Lot 2B).
> Seuils **[P]/[H]** ; **aucune [M]**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 2A · alimente **`DEC-L2-001`** ; couvre **R5** (hot-plug), **R2** (SPI
partagé), **back-powering**.

## Objectif (une question mesurable)

Sous insertion/retrait simulés (banc d'injection), aux **ordres de contacts**
nominaux **et défavorables**, le Host reste-t-il **sans reset ni corruption** et
sans **réveil parasite** du module non alimenté ?

## Conditions initiales

- Banc d'injection ([hot-plug](../hot-plug.md) §1) : commutation indépendante de
  `GND`, `VMOD`, lignes de bus, avec **timing réglable**.
- Écran actif (trafic **SPI**) **et** trafic **I²C** en cours (cas R2).

## Instrumentation requise

- Oscilloscope multi-voies + sonde de courant ; analyseur logique (bus).
- Séquenceur de commutation (relais/MOSFET) à timing contrôlé ; observation écran.

## Montage / fixture

Fixture d'injection reproduisant les séquences de contact ; **aucun** connecteur
réel (ni mesure d'endurance/résistance de contact — Lot 2B).

## Plage de tension

`3V3` ±5 % ; batterie ≈ 3,0 V / ≈ 4,2 V **[DS]** (coins).

## Scénarios (dont défavorables)

| Scénario | Ordre injecté |
|----------|---------------|
| Nominal | `GND` → présence → `VMOD` (rampe) → bus |
| Défavorable 1 | bus avant `VMOD` |
| Défavorable 2 | `GND` en dernier |
| Rebond | commutations rapides répétées |
| Module non alimenté, bus actif | bus activé, `VMOD` coupé (back-powering) |

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Cycles hot-plug (écran + trafic I²C) | ≥ 500 | **[P]** |
| Reset Host | 0 | **[P]** |
| Corruption / glitch écran hors spec | 0 | **[P]** |
| Réveil parasite module (`VMOD` coupé) | 0 ; fuite ≤ seuil | **[P]** |
| Latch-up (ordre défavorable) | 0 | **[P]** |

## Reproductibilité

`n_dut` ≥ 2 · `n_runs` = 500 cycles (par scénario) · `n_campaigns` ≥ 2 ;
méthode min/max/percentiles ; règle d'aberrants pré-définie.

## Critères d'arrêt immédiat

- Latch-up ou courant anormal soutenu.
- Reset Host répété / corruption écran persistante.
- Échauffement anormal du commutateur.

## Remise en état entre campagnes

Couper `VMOD`, décharger, vérifier commutateurs/fixture, réinitialiser le
firmware, revérifier l'étalonnage sonde/analyseur et la synchro d'horodatage.

## Éléments susceptibles d'être détruits

Commutateurs d'injection, tampons/level-shifters de bus, module de test ; risque
faible pour le Host (objet du test).

## Données brutes attendues

Captures courant/tension `VMOD` + lignes de bus, traces logiques, indicateurs de
corruption écran, journal par scénario/cycle ; SHA-256 par fichier au rapport.

## Sécurité opérateur

Limitation de courant ; lunettes ; commande à distance des commutations ;
surface ininflammable.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2A-HOTPLUG — Hot-plug par banc d'injection (indépendant du connecteur)

> **Statut : Brouillon (en attente de baselining).** Étudie le hot-plug
> **électrique** via un **banc d'injection**, **sans connecteur final**
> (connecteur → Lot 2B). Seuils `[P]/[H]` ; champs ouverts `[BL]` à geler avant
> essai ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md) ·
> [définitions des événements](event-definitions.md).

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

## Intégrité des masses (indispensable au scénario « `GND` en dernier »)

Le scénario où `GND` arrive **en dernier** (ou est absent) n'est **valide** que si
**aucun instrument ne recrée un chemin de masse parasite**. Sinon l'essai ne teste
pas réellement l'absence de référence — et peut **endommager l'instrumentation**.
Le protocole **impose**, avant essai :

- **Inventaire des masses reliées à la terre** : quelles masses d'instrument
  (oscilloscope, analyseur logique, alim de labo, PC de programmation) sont à la
  terre ; lesquelles sont flottantes.
- **Sondes différentielles** (ou isolation galvanique / alim isolée / USB isolé)
  pour toute mesure sur un nœud dont la masse est commutée.
- **Aucun retour de masse par `USB`/`UART`/`JTAG`** : programmation débranchée ou
  **isolée** pendant les scénarios `GND` commuté ; sinon la masse « revient » par
  le câble.
- **Schéma complet des masses de la fixture** (document joint, référencé au rapport).
- **Ordre de connexion des instruments** défini et respecté.
- **Vérification de continuité préalable** (avant mise sous tension) confirmant
  qu'aucune masse parasite ne subsiste quand `GND` du DUT est ouvert.
- **Limites de tension en mode commun** des sondes respectées (valeur `[BL]`
  selon les sondes retenues).

## Conditions d'alimentation

Voir la [matrice partagée](../electrical-risk-analysis.md) (§2). **Rail observé :
`VMOD` + lignes de bus.** Variantes **VA** (régulé) **et** **VB** (`VBAT` exposé) —
le comportement de hot-plug diffère. Coins : VA → régulé ±5 % + near-dropout ;
VB → **batterie 3,0 / 4,2 V**. USB **présent et absent** (le hot-plug en session
filaire est un cas distinct). Source documentée par run.

## Scénarios (dont défavorables)

| Scénario | Ordre injecté |
|----------|---------------|
| Nominal | `GND` → présence → `VMOD` (rampe) → bus |
| Défavorable 1 | bus avant `VMOD` |
| Défavorable 2 | `GND` en dernier |
| Rebond | commutations rapides répétées |
| Module non alimenté, bus actif | bus activé, `VMOD` coupé (back-powering) |

## Seuils de réussite / échec chiffrés

Verdicts **instrumentés** : voir [définitions des événements](event-definitions.md)
(reset, corruption écran, latch-up, réveil parasite, état connu).

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| `n_cycles` hot-plug (écran + trafic I²C) par scénario | ≥ 500 | **[P]** |
| Reset Host | 0 (def. instrumentée) | **[P]** |
| Corruption **logique** de l'affichage | 0 (CRC framebuffer + erreurs SPI) | **[P]** |
| **Anomalie de sortie affichée** | 0 (**≥ 1 méthode de vérification de sortie**, cf. [définitions](event-definitions.md)) | **[P]** |
| Réveil parasite module (`VMOD` coupé) | pas de réveil ; fuite ≤ `[BL]` µA | **[P]/[BL]** |
| Surcourant persistant (ordre défavorable) | 0 : I > `[BL]` pendant > `[BL]` µs | **[P]/[BL]** |
| Latch-up **suspecté** | 0 : conduction persistant stimulus coupé, levée **uniquement** par power-cycle | **[P]** |

### Champs à finaliser au baselining (`[BL]`)

- **Seuil de fuite** « réveil parasite » (µA) et seuil de tension résiduelle `VMOD`.
- **Seuils de surcourant persistant** (courant limite + durée) et **procédure
  d'attribution du latch-up** (stimulus off → power-cycle).
- **Spec « anomalie de sortie »** : méthode de vérification retenue (caméra /
  registre contrôleur / mire), image de référence et tolérance.
- **Limite de mode commun** des sondes différentielles retenues.

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 · **`n_cycles`** = **500 par scénario** · **5 scénarios**
  (nominal + 4) · **`n_runs`** = 1 série de `n_cycles` par (scénario, coin, DUT) ·
  **`n_campaigns`** ≥ 2 (indépendantes).
- **Total cycles** = `n_dut` × 5 scénarios × `n_coins` × 500 × `n_campaigns`.
  **Intention explicite** : à `n_dut`=2, 1 coin, 2 campagnes → **≥ 10 000
  commutations** ; le décompte exact (avec coins) est **figé au baselining**.
- **Répartition** : 500 cycles par scénario, pour chaque coin applicable, chaque
  DUT, chaque campagne.
- **Ordre / randomisation** : ordre des scénarios **randomisé** par campagne
  (graine journalisée) pour éviter tout biais d'usure/échauffement.
- **Repos** : refroidissement entre séries (durée `[BL]`) ; l'échauffement des
  commutateurs ne doit pas polluer les cycles suivants.
- **Reprise après échec** : un arrêt immédiat **invalide** la série en cours
  (non rétroactif) ; remise en état, journalisation, puis reprise.
- **Méthode** : min/max/percentiles ; règle d'aberrants pré-définie.

## Critères d'arrêt immédiat

- **Surcourant persistant / conduction anormale** (I > `[BL]` pendant > `[BL]` µs)
  — **déclencheur direct** de l'arrêt.
- Après arrêt : si la conduction **persiste stimulus coupé** et n'est levée
  **que** par power-cycle → **latch-up suspecté** (consigné, à confirmer).
- Reset Host répété **ou** **anomalie de sortie affichée** persistante.
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

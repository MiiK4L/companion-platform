<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2A — Analyse des risques électriques (*Proposé*, `DEC-L2-001`)

> **Statut : Ouvert / Proposé.** Analyse **documentaire** reliée au
> [registre des risques](../risk-register.md). Aucun statut de risque n'est
> modifié par cette PR (aucune mesure engagée) ; les mitigations restent
> **candidates**. Valeurs **[DS]/[C]/[H]/[P]** ; **aucune [M]**.

## 1. Risques électriques dans le périmètre 2A

| ID | Risque | Déclencheur (signal) | Mitigations candidates (non présélectionnées) | Protocole(s) |
|----|--------|----------------------|-----------------------------------------------|--------------|
| **R2** | Contention SPI écran ↔ CX-Bus | Glitch écran / corruption bus lors d'accès simultané | CS dédiés, arbitrage, SPI module optionnel, isolation lignes | [hot-plug/injection](protocols/hot-plug-injection.md) (trafic SPI actif) |
| **R5** | Sûreté du hot-plug non démontrable | Corruption/instabilité reproductible à l'insertion/retrait | Isolation/commutation, séquencement, banc d'injection | [inrush](protocols/inrush-et-rampe.md), [hot-plug](protocols/hot-plug-injection.md), [retrait](protocols/retrait-en-transaction.md) |
| **back-powering** | Alimentation parasite du module | Réveil/fuite `VMOD` coupé | Haute-Z lignes, séquencement, non-exposition `VBAT` (2B) | [hot-plug/injection](protocols/hot-plug-injection.md) |
| **court-circuit** | Défaut franc côté slot | Reset/dégât Host au court-circuit | Load switch protégé / eFuse, limitation de courant | [court-circuit](protocols/court-circuit.md) |
| **collisions I²C / bus-stuck** | Bus bloqué (esclave/module) | `SDA` maintenu bas, timeout | Récupération SCL, power-cycle, bus switch | [bus-stuck](protocols/bus-stuck.md) |

## 2. Coins de tension (à appliquer aux essais)

Tous les protocoles s'exécutent aux **coins** suivants (bornes d'analyse) :

| Condition | Valeur | Étiquette |
|-----------|--------|-----------|
| Rails nominaux ±5 % | `3V3` ±5 % | **[H]** |
| Batterie basse | ≈ 3,0 V | **[DS]** (seuil LiPo typique) |
| Batterie haute | ≈ 4,2 V | **[DS]** |

## 3. Critère global d'échec (proposé)

Une **corruption reproductible** ou un **risque avéré pour le Host** (reset,
latch-up, dégât) impose de **revoir la topologie** (isolation/commutation,
séquencement) — et **peut** remonter une contrainte de **brochage/connecteur**
au **Lot 2B**. **Échec ≠ rejet automatique** d'une option : il **déclenche un
arbitrage documenté**.

## 4. Gouvernance

Les statuts R2/R5 (registre) passeront de **Ouvert** à **En analyse** lorsque les
**campagnes de mesure** correspondantes **débuteront** (hors périmètre de cette
PR documentaire). Cette PR **prépare** cette transition (protocoles figés).

## Renvois

- [Registre des risques](../risk-register.md) · [Matrice des exigences](../requirements-matrix.md)
- [Protocoles](protocols/README.md) · [Cadre de validation](../validation-framework.md)

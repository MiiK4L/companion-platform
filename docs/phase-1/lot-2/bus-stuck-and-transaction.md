<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2A — Bus-stuck & retrait en transaction (*Proposé*, `DEC-L2-001`)

> **Statut : Ouvert / Proposé.** Analyse **documentaire** de la détection/
> récupération d'un bus bloqué et du retrait pendant un échange. Valeurs
> **[DS]/[C]/[H]/[P]** ; **aucune [M]**.

## 1. Bus-stuck (bus tenu bas)

Sur I²C, un esclave (ou le module retiré) peut **maintenir `SDA` bas**, bloquant
le bus pour tous. Le Host doit **détecter** et **récupérer** sans intervention.

### Mécanismes de récupération (au même niveau)

| Mécanisme | Principe | Vigilance |
|-----------|----------|-----------|
| **Clocking de récupération** | Le Host génère ≤ 9 impulsions `SCL` pour libérer l'esclave | Ne résout pas tous les cas |
| **Coupure `VMOD`** | Power-cycle du module fautif (power-gating) | Impacte le module entier |
| **Bus switch / isolation** | Déconnexion physique de la branche fautive | Coût (option I3) |
| **Timeout logiciel + reset contrôleur I²C** | Détection par watchdog de bus | Latence de détection |

### Critères chiffrés proposés

| Critère | Seuil | Étiquette | Alimente |
|---------|-------|-----------|----------|
| Détection bus-stuck | ≤ 50 ms | **[P]** | `DEC-L2-001` |
| Récupération automatique | 0 blocage sur ≥ 100 essais | **[P]** | `DEC-L2-001` |

## 2. Retrait en transaction

Le module peut être **retiré pendant un échange** (I²C ou SPI en cours). Le Host
doit rester **stable** (pas de crash, pas d'état incohérent) et **récupérer**.

### Comportements attendus

- Détection de l'absence (ligne présence + timeout de transaction).
- **Abandon propre** de la transaction en cours (pas de blocage du pilote).
- Retour à un **état connu** (bus haute-Z, `VMOD` coupé).

### Critères chiffrés proposés

| Critère | Seuil | Étiquette | Alimente |
|---------|-------|-----------|----------|
| Retrait en transaction | 0 crash Host sur ≥ 200 essais | **[P]** | `DEC-L2-001` |
| Reprise après retrait | retour état connu ≤ `[BL]` ms | **[P]/[BL]** | `DEC-L2-001` |

## 3. Alimente

- **`DEC-L2-001`** — exigences de détection/récupération (bus-stuck, retrait).

## Renvois

- [Hot-plug (électrique)](hot-plug.md) · [Isolation & commutation](isolation-and-switching.md)
- [Protocole bus-stuck](protocols/bus-stuck.md) · [Protocole retrait en transaction](protocols/retrait-en-transaction.md)

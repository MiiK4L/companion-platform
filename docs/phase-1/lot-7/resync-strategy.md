<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 7 — Stratégie de resynchronisation du temps (*Proposé*, `DEC-L7-001`)

> **Statut : Ouvert / Proposé.** Choix d'architecture **indépendant du RTC**
> lui-même : **quand** et **comment** corriger l'heure. Valeurs `[H]/[BL]` ;
> **aucune `[M]`**.

## Approches comparées

| Approche | Principe | Vigilance |
|----------|----------|-----------|
| **(S0) Jamais** | on fait confiance à la source (RTC précis + sauvegarde) | dérive cumulée non corrigée ; validité perdue si sauvegarde échoue |
| **(S1) Au branchement USB** | resync à chaque connexion filaire (hôte/temps réseau) | dépend de la fréquence des branchements |
| **(S2) Périodique** | resync à intervalle défini si une source est disponible | coût énergie/complexité |
| **(S3) Source externe** | via une source de temps (réseau, module, GNSS…) | dépend de la disponibilité de la source |

## Lien avec la validité de l'heure

La resynchronisation est le **mécanisme de transition** vers l'état **Valide**
(voir [validité](timebase-comparison.md)) : après une heure **Inconnue**, seule une
resync la rend **Valide** ; la **précision requise** de la resync dépend du besoin
produit (`[BL]`).

## Critères de comparaison

| Critère | Nature |
|---------|--------|
| Disponibilité de la source de resync | selon approche |
| Fréquence de correction | selon approche |
| Coût énergie / complexité firmware | **[H]** |
| Compense la dérive d'un RTC interne ? | oui (selon fréquence) |
| Robustesse (source absente) | repli défini |

> Une resync **fréquente** peut **relâcher** l'exigence de précision de la source
> (un RTC interne + resync USB peut suffire) — **compromis à mesurer**, sans
> présupposer un RTC externe.

## Alimente

- **`DEC-L7-001`** — la stratégie de resync est **arbitrée avec** la source de temps
  (compromis précision/conso/complexité). **Aucune approche retenue.**

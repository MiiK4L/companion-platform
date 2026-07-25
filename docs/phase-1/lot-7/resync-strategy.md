<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 7 — Stratégie de resynchronisation du temps (*Proposé*, `DEC-L7-001`)

> **Statut : Ouvert / Proposé.** Choix d'architecture **indépendant du RTC**
> lui-même : **quand** et **comment** corriger l'heure. Valeurs `[H]/[BL]` ;
> **aucune `[M]`**.

## Deux dimensions distinctes : déclencheurs vs politique

La resynchronisation combine **deux dimensions indépendantes** :

### Dimension A — **Déclencheurs** (quel événement peut lancer une resync)

| Déclencheur | Principe |
|-------------|----------|
| **USB branché** | connexion filaire (hôte / temps réseau) disponible |
| **Temps écoulé** | seuil de temps depuis la dernière resync atteint |
| **Utilisateur** | réglage manuel |
| **Source disponible** | une source externe (réseau, module, GNSS…) devient accessible |

### Dimension B — **Politique** (règle décidant quand appliquer une resync)

| Politique | Principe | Vigilance |
|-----------|----------|-----------|
| **Jamais** | on fait confiance à la source (RTC + sauvegarde) | dérive non corrigée ; validité perdue si sauvegarde échoue |
| **À chaque alimentation** | resync au démarrage/branchement | dépend de la présence d'un déclencheur |
| **Périodique** | resync à intervalle défini | coût énergie/complexité |
| **Opportuniste** | resync **si** un déclencheur est disponible, sans forcer | robuste, non déterministe |

> **Déclencheur ≠ politique** : un même déclencheur (USB branché) peut être exploité
> par des politiques différentes (à chaque alimentation, ou opportuniste). La
> comparaison croise **A × B**.

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

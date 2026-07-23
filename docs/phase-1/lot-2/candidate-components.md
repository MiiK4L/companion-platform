<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2A — Composants candidats par fonction (*Proposé*, `DEC-L2-001`)

> **Statut : Ouvert / Proposé.** Familles **au même niveau**, **sans
> présélection**. Les MPN cités sont des **exemples représentatifs** pour cadrer
> le sourcing (MPN, pas SKU ; ≥ 2 sources visées — voir
> [sourcing & BOM](../sourcing-and-bom.md)). Caractéristiques **[DS]** issues des
> fiches techniques, **à confirmer** ; **aucune [M]**.

## 1. Commutation d'alimentation `VMOD` (load switch)

| Fonction | MPN exemples | Points [DS] à confronter | Vigilance |
|----------|--------------|--------------------------|-----------|
| Load switch à rampe | TPS22918, AP22913 | Rampe réglable, OCP/thermique, R_on | Disponibilité, courant max |
| P-MOSFET + rampe RC | (P-MOSFET générique) | V_GS(th), R_DS(on), Q_g | Protection à ajouter |
| eFuse / switch protégé | (famille eFuse) | Limitation active, clamp | Coût, complexité |

## 2. Isolation / level-shift de bus

| Fonction | MPN exemples | Points [DS] à confronter | Vigilance |
|----------|--------------|--------------------------|-----------|
| Répéteur/tampon I²C | TCA9517 | Isolation capacitive, offset, level-shift | Alim propre, un actif de plus |
| Level-shifter passif | PCA9306 | Bidirectionnel, capacité de bus | Pas de limitation active |
| Bus switch | (famille bus switch) | Déconnexion, R_on, latence | Coût |

## 3. Protection ESD (préliminaire — voir [ESD](esd-paths.md))

| Fonction | MPN exemples | Points [DS] à confronter | Vigilance |
|----------|--------------|--------------------------|-----------|
| TVS/ESD série ligne rapide | PESD (série) | **Capacité faible**, V_stand-off, I_leak | Intégrité SPI |
| Réseau ESD multi-lignes | (réseau ESD) | Capacité, nb de lignes | Diaphonie |

> **Dimensionnement ESD non figé** ici (dépend connecteur 2B + routage + mécanique).

## 4. Grille de comparaison **par fonction** (à instruire)

Chaque fonction sera comparée sur des critères **homogènes** (comparaison par
fonction, pas par produit) :

| Critère | Nature |
|---------|--------|
| Conformité au besoin électrique (courant, tension, rampe) | [DS] → **[M]** en L2 |
| Capacité ajoutée au bus (I²C/SPI) | [DS] → **[M]** en L2/L4 |
| Courant de fuite (impact deep-sleep) | [DS] → **[M]** en L5 |
| Protection intégrée (OCP, thermique, clamp) | [DS] |
| Disponibilité / pérennité (≥ 2 sources) | sourcing (continu) |
| Coût unitaire (multi-distributeurs, daté) | sourcing (au moment du lot) |
| Surface / boîtier | [DS] |

## 5. Alimente

- **`DEC-L2-001`** — le **choix par fonction** se fait **après** mesures
  (protocoles) et comparaison ; aucune famille n'est retenue à ce stade.

## Renvois

- [Isolation & commutation](isolation-and-switching.md) · [ESD](esd-paths.md)
- [Sourcing & BOM](../sourcing-and-bom.md) · [Protocoles](protocols/README.md)

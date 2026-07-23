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

> **Une référence par ligne.** Les caractéristiques sont **[DS]** (fiche
> officielle, source citée) ; les cellules non encore transcrites sont marquées
> **« à relever »** (jamais inventées). **Ne pas mutualiser** des fonctions entre
> références distinctes.

## 1. Commutation d'alimentation `VMOD` (load switch)

| Référence | Plage d'entrée | I continu | Limitation de courant | Prot. thermique | Court-circuit | Rampe | Décharge de sortie | Hors alimentation | I repos / fuite | Boîtier | Source (fiche) |
|-----------|----------------|-----------|-----------------------|-----------------|---------------|-------|--------------------|-------------------|-----------------|---------|----------------|
| **TPS22918** | ≈ 0,8–5,5 V (à confirmer) | à relever | **Non** (pas de limitation active) | à relever | pas de limitation active | **configurable** (broche `CT`) | **configurable** (QOD) | sortie déchargée (QOD) | faible (à relever) | à relever | TI « TPS22918 » |
| **AP22913** | à relever | à relever | à relever (ne pas présumer) | à relever | à relever | **slew-rate contrôlé** | à relever | à relever | à relever | à relever | Diodes Inc. « AP22913 » |
| **P-MOSFET discret** (générique) | selon MOSFET | selon R_DS(on) | **Non** (sauf circuit ajouté) | Non | Non (à ajouter) | via réseau `RC` de grille | via R de décharge | dépend du montage | négligeable | selon réf. | fiche du MOSFET retenu |
| **eFuse / switch protégé** (à sélectionner) | selon réf. | selon réf. | **Oui** (active) | Oui (typ.) | Oui (clamp/limite) | selon réf. | selon réf. | selon réf. | selon réf. | selon réf. | fiche de l'eFuse retenu |

> ⚠️ Le **TPS22918** n'offre **pas** de limitation active de courant : il ne doit
> **pas** être présenté comme un load switch à protection de courant. Ses atouts
> sont le **temps de montée configurable** et la **décharge rapide de sortie
> (QOD)**. L'**AP22913** est un load switch à **slew-rate contrôlé** : relever ses
> protections et variantes **sur sa propre fiche**, sans les mutualiser.

## 2. Tampon / traduction de niveau de bus (I²C) — *pas de la simple « isolation »*

| Référence | Type exact | Plage tension (A / B) | Translation de niveau | Actif / passif | Broche `EN` | Effet sur la capacité de bus | Protection | Boîtier | Source (fiche) |
|-----------|-----------|-----------------------|-----------------------|----------------|-------------|------------------------------|------------|---------|----------------|
| **TCA9517** | **Buffer/répéteur I²C actif** avec translation | à relever (VCCA / VCCB) | **Oui** (offset de tension statique côté B) | **Actif** (alim requise) | à relever | **Découple** les capacités A/B (répéteur) | selon fiche | à relever | TI « TCA9517 » |
| **PCA9306** | **Traducteur bidirectionnel à pass-FET** | à relever | par **pass-FET** (suiveur) | **Passif** (pass-FET) | **Oui** (entrée `EN`) | **Ne découple pas** (pass-through) | selon fiche | à relever | TI « PCA9306 » |
| **Bus switch** (à sélectionner) | Commutateur analogique de lignes | selon réf. | selon réf. | Actif | selon réf. | Déconnexion physique (haute-Z) | selon réf. | selon réf. | fiche du bus switch retenu |

> Le **TCA9517** et le **PCA9306** ne sont **pas** équivalents : le premier est un
> **répéteur actif** (découple les capacités, translation à offset), le second un
> **traducteur passif à pass-FET** avec entrée `EN` (ne découple pas). Ne pas les
> résumer sous une notion vague d'« isolation ».

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

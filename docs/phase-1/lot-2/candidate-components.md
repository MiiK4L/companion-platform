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
| **TPS22918** | 1–5,5 V | 2 A (max) | **Non** (pas de limitation active) | à relever | pas de limitation active | **configurable** (broche `CT`) | **configurable** (QOD) | sortie déchargée (QOD) | faible (à relever) | SOT-23-6 (DBV) | TI · SLVSD76 → §Sources |
| **AP22913** | à relever | à relever | à relever (ne pas présumer) | à relever | à relever | **slew-rate contrôlé** | à relever | **blocage de courant inverse** (true reverse current blocking) | à relever | à relever | Diodes · DS41203 → §Sources |
| **P-MOSFET discret** (générique) | selon MOSFET | selon R_DS(on) | Non (par nature ; sauf circuit ajouté) | selon montage | selon montage | via réseau `RC` de grille | via R de décharge | dépend du montage | négligeable | selon réf. | réf. à sélectionner |
| **eFuse / switch protégé** (à sélectionner) | selon réf. | selon réf. | selon réf. | selon réf. | selon réf. | selon réf. | selon réf. | selon réf. | selon réf. | selon réf. | réf. à sélectionner |

> ⚠️ Le **TPS22918** n'offre **pas** de limitation active de courant : il ne doit
> **pas** être présenté comme un load switch à protection de courant. Ses atouts
> sont le **temps de montée configurable** et la **décharge rapide de sortie
> (QOD)**. L'**AP22913** est un load switch à **slew-rate contrôlé** (avec
> **blocage de courant inverse**) : relever ses protections et variantes **sur sa
> propre fiche**, sans les mutualiser. Les lignes **génériques** (P-MOSFET, eFuse)
> **ne portent aucune caractéristique `[DS]`** tant qu'une référence n'est pas
> sélectionnée (la mention « Non par nature » du MOSFET est définitoire, pas une
> donnée de fiche).

## 2. Tampon / traduction de niveau de bus (I²C) — *pas de la simple « isolation »*

| Référence | Type exact | Plage tension (A / B) | Translation de niveau | Actif / passif | Broche `EN` | Effet sur la capacité de bus | Protection | Boîtier | Source (fiche) |
|-----------|-----------|-----------------------|-----------------------|----------------|-------------|------------------------------|------------|---------|----------------|
| **TCA9517** | **Buffer/répéteur I²C actif** avec translation | VCCA 2,3–3,6 V / VCCB 4,5–5,5 V | **Oui** (offset de tension statique côté B) | **Actif** (alim requise) | à relever | **Découple** les capacités A/B (répéteur) | selon fiche | à relever | TI · SCPS242 → §Sources |
| **PCA9306** | **Traducteur bidirectionnel à pass-FET** | à relever | par **pass-FET** (suiveur) | **Passif** (pass-FET) | **Oui** (entrée `EN`) | **Ne découple pas** (pass-through) | selon fiche | DCU | TI · SCPS113 → §Sources |
| **Bus switch** (à sélectionner) | Commutateur analogique de lignes | selon réf. | selon réf. | Actif | selon réf. | Déconnexion physique (haute-Z) | selon réf. | selon réf. | réf. à sélectionner |

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

## Sources datasheet (traçabilité)

> Sources **consultées le 2026-07-23**. Seules les **références réellement citées**
> figurent ici ; les lignes **génériques** (P-MOSFET, eFuse, bus switch, familles
> ESD) **ne portent aucune caractéristique `[DS]`** tant qu'une référence n'est pas
> sélectionnée — elles seront renseignées lors du **sourcing daté**
> (voir [sourcing & BOM](../sourcing-and-bom.md)).

| Référence | Fabricant | Titre du document | N° / révision | Date consultée | Variante / boîtier | Référence stable |
|-----------|-----------|-------------------|---------------|----------------|--------------------|------------------|
| TPS22918 | Texas Instruments | *5.5-V, 2-A, 52-mΩ On-Resistance Load Switch* | **SLVSD76C** | 2026-07-23 | DBV (SOT-23-6) | `ti.com/lit/ds/symlink/tps22918.pdf` (lit. SLVSD76) |
| AP22913 | Diodes Incorporated | *Single Slew Rate Controlled Load Switch With True Reverse Current Blocking* | **DS41203 Rev. 6** (mai 2021) | 2026-07-23 | voir fiche | `diodes.com/datasheet/download/AP22913.pdf` (DS41203) |
| TCA9517 | Texas Instruments | *Active I²C Bus Buffer/Repeater with Level Shifting* | **SCPS242D** | 2026-07-23 | voir fiche | `ti.com/lit/ds/symlink/tca9517.pdf` (lit. SCPS242) |
| PCA9306 | Texas Instruments | *Dual Bidirectional I²C Bus and SMBus Voltage-Level Translator* | **SCPS113** | 2026-07-23 | DCU | `ti.com/lit/ds/symlink/pca9306.pdf` (lit. SCPS113) |

> La **révision exacte** (lettre) est celle relevée à la date ci-dessus ; toute
> caractéristique `[DS]` du tableau §1/§2 renvoie à ces documents. Les cellules
> « à relever » restent à transcrire depuis ces fiches au baselining/sourcing.

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

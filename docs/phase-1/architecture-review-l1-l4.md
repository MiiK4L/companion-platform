<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Revue d'architecture intermédiaire — lots L1 à L4

> **Statut : Proposé — revue d'architecture intermédiaire, sans décision ni ADR.**
> Point de contrôle **documentaire** après fusion des lots L1, L2A, L2B, L3, L4.
> **Aucune nouvelle DEC, aucune ADR, aucun changement de statut, aucun achat
> validé, aucun composant retenu.** Les décisions restent celles du
> [registre des décisions](decisions-register.md) et du [plan des ADR](adr-plan.md).

## 1. État des 9 `DEC-*` ouvertes

Toutes **Ouvertes / Proposé**, aucune arbitrée, aucune ADR créée ; validation sur
banc L2+ selon le [cadre de validation](validation-framework.md).

| `DEC-*` | Objet | Validation | ADR (prév.) |
|---------|-------|-----------|-------------|
| **DEC-L1-001** | Allocation GPIO/bus de référence (+ expander / partage de bus éventuel) | L2 (bring-up) | 0013 |
| **DEC-L1-002** | Architecture de cœur de calcul (A XIAO / B autre module / C custom) | L4 + L5 | 0018 / dédiée |
| **DEC-L2-001** | Sûreté électrique CX-Bus (isolation/commutation, protections, séquencement) | L2 (banc) | 0014 |
| **DEC-L2-002** | Connecteur : famille, mécanique, endurance, **mapping physique candidat** | L2 (banc méca) | 0015 |
| **DEC-L2-003** | Rails exposés (dont `VBAT`) + stratégie de puissance des modules | L2 | future (n° à la création) |
| **DEC-L3-001** | Architecture de découverte & d'identification **logique** | L3 (banc HW+CODE) | 0016 |
| **DEC-L3-002** | Format et contenu du **Manifest** | L3 (parsing borné prouvé) | future (n° à la création) |
| **DEC-L4-001** | Matériel d'affichage (dalle, contrôleur, **interface physique**, format) | L4 (banc) | 0017 |
| **DEC-L4-002** | Moteur graphique (rendu, **stratégie mémoire/buffers**, pipeline) | L4 (banc) | future (n° à la création) |

## 2. Trois couplages transverses identifiés

Ce sont des **dépendances à surveiller**, **pas des contradictions** :

1. **Topologie SPI** — décidée **conjointement** par `DEC-L1-001` (budget), la
   contention `DEC-L2-001`, et l'interface `DEC-L4-001` (option « partage SPI
   écran/CX-Bus »). Voir la **matrice R2** au §4.
2. **Brochage ↔ interface écran** — le **mapping physique** (`DEC-L2-002`) doit
   accueillir les lignes SPI de l'écran **et** du module ; un passage écran en
   **QSPI** (`DEC-L4-001`) remonte vers le brochage et le budget GPIO (`DEC-L1-001`).
3. **Domaine d'alimentation de l'identification** — la lecture d'ID « sans
   alimenter le module » (`DEC-L3-001`) est **conditionnée** par les **rails
   exposés** (`DEC-L2-003`), sans qu'aucun rail ne soit créé au Lot 3.

## 3. Interfaces entre lots (L2 ↔ L3 ↔ L4)

- **L2 ↔ L3** — le mieux tissé : L3 réutilise le **banc d'injection hot-plug**
  (L2A), l'**analyse back-powering** (lire l'ID sans alimenter le module) et les
  **contacts/brochage** (L2B) ; le « domaine d'alimentation » (L3) **renvoie** à
  `DEC-L2-003` sans créer de rail.
- **L2 ↔ L4** — via **R2 (SPI partagé)** et l'**énergie** : `spi-sharing` (L4)
  réutilise les entrées 2A/2B ; la conso écran renvoie à **L6**. Un passage écran
  en **QSPI** remonte vers L1/L2.
- **L3 ↔ L4** — couplage **indirect** : ressources Host communes (budget GPIO/bus
  L1, firmware, buses I²C/SPI). Vigilance : ne pas sur-souscrire le SPI (écran +
  module + éventuel stockage Manifest).
- **Amont commun** : **L1** (budget GPIO/bus, RAM cœur) conditionne L2/L3/L4 ;
  **R2** (SPI) et **R8** (disponibilité) sont les risques transverses.

**Verdict : interfaces cohérentes et explicitement renvoyées** ; aucune interface
orpheline. Seule zone à discipliner : le **partage du SPI**.

## 4. Risque transverse principal — le SPI partagé (R2)

Le bus **SPI partagé écran ↔ CX-Bus** est la dépendance la plus transverse. Il est
tracé dans **plusieurs DEC** et **n'a pas d'autorité unique** : il sera **arbitré
au banc L2/L4** (voir le risque **R2** du [registre des risques](risk-register.md)).

### Matrice de traçabilité du SPI partagé

```
Risque R2 (contention SPI écran ↔ CX-Bus)
├── DEC-L1-001 : budget GPIO/bus
├── DEC-L2-001 : sûreté et contention électrique
├── DEC-L2-002 : mapping physique / brochage
├── DEC-L4-001 : interface d'affichage
└── DEC-L4-002 : trafic, buffers et pipeline
```

| Maillon | Contribution au SPI partagé | Où c'est traité |
|---------|-----------------------------|-----------------|
| `DEC-L1-001` | Budget GPIO/bus ; option de **partage** vs SPI séparé | [budget GPIO](lot-1/gpio-bus-budget.md) |
| `DEC-L2-001` | **Contention électrique**, glitch, isolation des lignes | [risques électriques L2A](lot-2/electrical-risk-analysis.md) |
| `DEC-L2-002` | **Mapping physique** des lignes SPI (écran + module) | [brochage L2B](lot-2b/pinout.md) |
| `DEC-L4-001` | **Interface d'affichage** (SPI/QSPI), débit | [partage SPI L4](lot-4/spi-sharing.md) |
| `DEC-L4-002` | **Trafic**, stratégie de **buffers** et **pipeline** (charge du bus) | [moteur graphique](lot-4/graphics-engine-comparison.md) |

> Aucune de ces DEC ne **tranche seule** le SPI : l'arbitrage se fera **au banc**
> (contention L2A + FPS sous trafic L4), en cohérence entre les cinq maillons.

## 5. Composants : véhicules de prototypage vs trop ouverts

> ⚠️ **Aucune DEC n'est Accepté** : **aucun composant n'est retenu**. La liste
> ci-dessous **ne fige rien**.

### Suffisamment stables pour servir de **véhicules de prototypage**

- **Plateforme ESP32-S3** (silicium acté, [ADR-0001](../adr/0001-framework-firmware-esp-idf.md) /
  [ADR-0004](../adr/0004-coeur-de-calcul-socket.md)) → **cartes XIAO ESP32-S3** comme
  véhicule de prototypage de référence (`DEC-L1-002` option A, **non figée**).
- **Instrumentation de banc** (transverse) : oscilloscope + sonde de courant,
  alimentation de labo à limitation, analyseur logique, cycleur mécanique (L2B).
  **Indépendant** de toute décision.

### Encore **trop ouverts** — à ne pas engager

- **Connecteur final** (`DEC-L2-002`), **contrôleur/dalle final** (`DEC-L4-001`),
  **exposition `VBAT`** (`DEC-L2-003`), **socketage** (L5), **batterie/jauge**
  (L6), **RTC** (L7). Tout engagement ici serait **prématuré**.

## 6. Règle sur les achats — échantillons d'évaluation, pas une BOM de production

> **Les composants d'évaluation listés ci-dessous sont des ÉCHANTILLONS
> D'ÉVALUATION destinés aux bancs de comparaison — ni des composants retenus, ni
> une BOM de production.** Conforme à la politique **BOM prototype ≠ BOM V1
> candidate ≠ production** ([sourcing & BOM](sourcing-and-bom.md)). Ils sont
> **achetés en éventail** (plusieurs candidats par fonction) précisément pour
> **instruire** les décisions, **sans en présélectionner aucune**.

| Fonction | Échantillons d'évaluation (éventail) | DEC alimentée |
|----------|--------------------------------------|---------------|
| Sûreté / commutation | TPS22918 / AP22913 ; TCA9517 / PCA9306 ; PESD | DEC-L2-001 |
| Connecteur | échantillons des 5 familles | DEC-L2-002 |
| Identification | 24AA02E64 / AT24MAC402 ; DS28E07 / DS2431 ; ATECC608 / SE050 ; MB85RC / FM24 | DEC-L3-001 |
| Affichage | ST7789 ; GC9A01 ; ILI9341 ; SSD1351 / SSD1331 ; Sharp LS013B7DH03 / LS027B7DH01 | DEC-L4-001 |

## Synthèse

- **Cohérence des DEC : bonne** ; unique vigilance = **arbitrage du SPI partagé
  (R2)**, transverse à L1/L2/L4.
- **Interfaces L2 ↔ L3 ↔ L4 : propres et tracées.**
- **Prototypage** : véhicule **XIAO ESP32-S3** + **instrumentation** + **échantillons
  d'évaluation en éventail** ; **rien de figé**, aucune BOM de production.

## Renvois

- [Registre des décisions](decisions-register.md) · [Plan des ADR](adr-plan.md)
- [Registre des risques](risk-register.md) · [Cadre de validation](validation-framework.md)
- [Sourcing & BOM](sourcing-and-bom.md)

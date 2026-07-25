<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Phase 1 — Sourcing & BOM (candidates)

Ce document est un document de **cadrage**. Il ne fige aucune décision et ne présélectionne
aucun composant : tout candidat listé ci-dessous reste au statut **Proposé** tant qu'un
rapport de mesure reproductible ne l'a pas fait passer en **Accepté** (voir
[Cadre de validation & modèle de preuves](validation-framework.md)).

## 1. Stratégie de sourcing

La stratégie de sourcing de la Phase 1 vise à réduire le risque d'approvisionnement dès
l'étude de conception, sans figer de choix prématuré.

Principes :

- **Raisonner en référence fabricant (MPN)**, jamais en SKU de boutique. Un SKU de
  distributeur peut disparaître ou changer sans que le composant lui-même soit affecté ;
  seul le MPN identifie le composant de manière stable.
- **≥ 2 fabricants ou 2 distributeurs vérifiés** par composant critique. Aucun composant
  critique ne doit dépendre d'une source unique.
- Noter le **cycle de vie** de chaque composant candidat : **Active**, **NRND**
  (Not Recommended for New Designs) ou **EOL** (End Of Life). Un composant NRND ou EOL ne
  peut pas être retenu comme candidat principal sans justification explicite.
- Noter le **stock daté** : la disponibilité doit toujours être accompagnée de la date de
  relevé, car elle est volatile.
- **Croiser ≥ 2 distributeurs** parmi LCSC, Mouser, Digi-Key, Farnell/Element14, TME pour
  chaque composant critique, afin de vérifier la cohérence des MPN, du cycle de vie et de la
  disponibilité.
- **Préférer les composants multi-sources et les boîtiers courants** (boîtiers standards,
  seconde source disponible), qui réduisent le risque d'appro et facilitent la fabrication.
- Le **sourcing en temps réel est effectué au moment de l'exécution du lot**, pas figé dans
  ce plan. Les prix et les stocks sont volatils ; les valeurs relevées à la rédaction
  seraient périmées à l'exécution. Ce document définit la méthode, pas les chiffres.

## 2. Alternatives par composant critique (≥ 2 candidats)

Le tableau ci-dessous liste, pour chaque fonction critique, des **candidats à comparer**.
Ce ne sont **pas des choix** : la colonne « Candidat A » n'indique aucune préférence, seulement
un point de départ pour l'étude comparative. Chaque candidat devra être qualifié via un
protocole de test avant toute décision.

Chaque **ligne regroupe des alternatives d'une même fonction** (une alternative doit
remplir exactement la même fonction que les autres de sa ligne). Les fonctions autrefois
mélangées (technologie d'écran, contrôleur, moteur graphique, pilote, chargeur simple vs
power-path, régulateur, load switch, buffer I²C, traducteur de niveau, multiplexeur,
connecteur) sont désormais **séparées**. Les références sont **candidates** et seront
vérifiées (MPN, cycle de vie, stock) **au moment du lot**.

| Fonction | Candidat A | Candidat B | Candidat C / repli |
|---|---|---|---|
| Panneau / technologie d'affichage | TFT IPS couleur | OLED couleur | Sharp Memory LCD (mono) |
| Contrôleur d'affichage | ST7789 | GC9A01 | ILI9341 |
| Moteur graphique | LVGL | µGFX | moteur minimal maison |
| Couche pilote (affichage) | LovyanGFX | TFT_eSPI | pilote custom |
| RTC | PCF8563 | RV-3028-C7 | DS3231 |
| Accéléromètre | BMA400 | LIS2DH12 | LIS2DW12 |
| Jauge (fuel gauge) | MAX17048 | LC709203F | BQ27441 |
| Chargeur simple | MCP73831 | TP4056 | LTC4054 |
| Chargeur avec power-path | BQ24074 | BQ25185 | BQ24232 |
| Régulateur | LDO TLV757P | Buck TPS62840 | LDO AP2112 |
| Load switch | TPS22918 | AP22913 | P-MOSFET + commande |
| Buffer / hot-swap I²C | TCA9517 | LTC4302 | PCA9508 |
| Traducteur de niveau | PCA9306 | TXB0104 | TXS0108E |
| Multiplexeur I²C | TCA9548A | PCA9546A | — |
| Expander GPIO | TCA9535 | PCA9555 | MCP23017 |
| Support d'identification | 24AA02E64 (EUI) | AT24C64 | DS28E07 (1-Wire) / µC actif |
| Famille de connecteur CX-Bus | Card-edge (bord de PCB) | Board-to-board (mezzanine) | FPC / pogo |
| Haptique | ERM + transistor | LRA + DRV2605L | — |

**Connecteur** : on reste ici sur des **catégories neutres**. Aucune série précise (ex. de
famille mezzanine) n'est désignée à ce stade ; la **série exacte** et ses **caractéristiques
comparables** (nombre de contacts, pas/pitch, mode de rétention, aptitude au hot-plug, nombre
d'insertions spécifié) seront **sourcées et comparées en L2**.

Rappel : tous ces éléments sont **Proposés**. La comparaison objective (mesures, seuils,
reproductibilité) tranche ; ce tableau ne fait qu'ouvrir l'espace des options. *(Une échelle
de boutons sur ADC — sans puce — reste une option de réduction d'entrées, traitée côté
budget GPIO/L1, distincte de l'expander.)*

## 3. Trois niveaux de BOM (aucune « BOM de production »)

La Phase 1 produit **trois niveaux de nomenclature**, aucun n'étant une BOM de production.
Ce point est structurant : parler de « BOM de production » à ce stade serait trompeur.

### Niveau 1 — BOM de prototype

Matériel d'évaluation sur breakouts et cartes de développement, destiné à l'exploration et à
la mise au point des protocoles de test :

- Carte de développement / µC : XIAO (ou équivalent d'éval).
- Modules TFT d'évaluation (pour comparer contrôleurs écran candidats).
- Breakouts RTC, jauge, accéléromètre, expander GPIO.
- Breakout EEPROM (support d'identification).
- Load switch (module ou composant sur carte de test).
- Batterie LiPo de test.
- Alimentation de laboratoire.
- Instruments : oscilloscope, multimètre.

### Niveau 2 — BOM V1 candidate (conception détaillée & préproduction)

Nomenclature candidate en vue de la conception détaillée et de la préproduction :

- ICs dans les **boîtiers pressentis** (et non plus en breakout).
- Connecteur CX-Bus candidat.
- Batterie candidate.

Cette BOM V1 est **explicitement PAS une BOM de production**. Elle sert de base à l'étude de
conception détaillée ; elle n'intègre ni le PCB final, ni la qualification complète.

### Niveau 3 — Shortlist qualifiée + contraintes de seconde source

- Liste restreinte de composants **qualifiés** par les rapports de mesure reproductibles.
- **Contraintes de seconde source** documentées pour chaque composant critique (MPN
  alternatif, distributeur alternatif, compatibilité de boîtier/brochage).

## 4. Encadré — Ce que la Phase 1 NE couvre PAS

> Une véritable **BOM de production** exige des travaux qui **ne font pas partie** de la
> Phase 1 :
>
> - PCB intégré (routage final, empilage, contraintes de fabrication).
> - Analyse thermique.
> - Compatibilité électromagnétique (CEM).
> - Conception mécanique finale (boîtier, assemblage, tenue).
> - DFM / DFT (Design For Manufacturing / Design For Test).
> - Qualification batterie (sécurité, cyclage, certification).
> - Approvisionnement réel (commandes fermes, contrats, incoterms).
>
> Tant que ces travaux ne sont pas menés, aucune nomenclature ne peut être qualifiée de
> « BOM de production ».

## 5. BOM candidate agrégée par fonction d'architecture (consolidation L9)

> **Statut : Proposé — index de consolidation, aucune retenue.** Cette agrégation
> **indexe par fonction d'architecture** les **séries représentatives candidates**
> déjà sourcées lot par lot ; elle **renvoie** vers le détail (cycle de vie, stock,
> statut par ligne) plutôt que de le recopier (anti-duplication). Les composants
> **illustrent** une fonction — ils ne la définissent pas et **aucun n'est retenu**.
> Le raisonnement reste **par fonction** ; les MPN sont **candidats** et vérifiés
> **au moment du lot**.

> **Maturité documentaire** (colonne dédiée) : **●** étudié (comparaison complète
> + ≥ 2 séries sourcées) · **◐** partiel (élément explicitement **à
> compléter/sourcer**) · **○** à compléter.

| Fonction d'architecture | Séries représentatives (éventail candidat) | Maturité doc. | `DEC-*` | Détail sourcé |
|-------------------------|---------------------------------------------|:---:|---------|---------------|
| Cœur de calcul | XIAO ESP32-S3 (réf. prototypage) · autre module ESP32-S3 · carte custom ESP32-S3 nu | ● | `DEC-L1-002` | [comparaison des cœurs](lot-1/core-architecture-comparison.md) |
| Sûreté / commutation CX-Bus | Load switch (TPS22918 / AP22913 / P-MOSFET) · buffer I²C (TCA9517 / LTC4302 / PCA9508) · traducteur (PCA9306 / TXB0104) · PESD | ● | `DEC-L2-001` | [composants L2A](lot-2/candidate-components.md) |
| Connecteur CX-Bus | 5 familles : mezzanine (DF40 / SlimStack) · card-edge · FPC/FFC · pogo (Mill-Max) · filaire verrouillable (témoin) | ● | `DEC-L2-002` | [familles L2B](lot-2b/families-comparison.md) |
| Identification module | EEPROM+EUI (24AA02E64 / AT24MAC402) · EEPROM (24AA64 / M24C64) · 1-Wire (DS28E07 / DS2431) · µC (ATtiny / RP2040) · Secure Element (ATECC608 / SE050) · FRAM (MB85RC / FM24) | ● | `DEC-L3-001` | [composants L3](lot-3/candidate-components.md) |
| Dalle / contrôleur d'affichage | TFT (ST7789 / ILI9341 / GC9A01) · OLED (SSD1351 / SSD1331) · Sharp Memory LCD (LS013B7DH03 / LS027B7DH01) · transflectif (à sourcer) | ◐ | `DEC-L4-001` | [composants L4](lot-4/candidate-components.md) |
| Moteur / pilote graphique | Moteur (LVGL / µGFX / maison) · pilote (LovyanGFX / TFT_eSPI / esp_lcd) | ● | `DEC-L4-002` | [moteur graphique](lot-4/graphics-engine-comparison.md) |
| Support / socketage du cœur | Barrettes 2,54 mm (Harwin / Samtec) · tulipe (Mill-Max / Preci-Dip) · mezzanine (DF40 / SlimStack) · ZIF/LIF (Aries / Würth) · baseline soudée (témoin) | ● | `DEC-L5-001` | [composants L5](lot-5/candidate-components.md) |
| Charge / power-path / régulation | Charge simple (MCP73831 / TP4056 / LTC4054) · power-path (BQ24074 / BQ25185 / BQ24232) · régulateur (TLV757P / TPS62840 / AP2112) | ● | `DEC-L6-001` | [composants L6](lot-6/candidate-components.md) |
| Stockage d'énergie | Architectures mono/multi-cellule, protection intégrée ; chimies **propriété** (LiPo / Li-Ion / LiFePO4) | ● | `DEC-L6-002` | [stockage d'énergie](lot-6/energy-storage-comparison.md) |
| Estimation d'état batterie | Jauge (MAX17048 / LC709203F / BQ27441) · ADC seul · hybride | ● | `DEC-L6-003` | [estimation d'état](lot-6/battery-state-estimation.md) |
| Base de temps (RTC) | Quartz externe (PCF8563) · module ULP quartz intégré (RV-3028-C7) · TCXO (DS3231) | ● | `DEC-L7-001` | [base de temps](lot-7/timebase-comparison.md) |
| Persistance (support) | NVS · LittleFS · (autres supports comparés) | ● | `DEC-L7-002` | [persistance](lot-7/persistence-comparison.md) |
| Runtime scripté | Lua · WASM/WAMR · autre | ● | `DEC-L8-001` | [runtimes](lot-8/scripted-runtime-comparison.md) |
| Distribution des applications | Install dynamique « sans reflash » · bundle OTA · hybride | ◐ | `DEC-L8-002` | [modèle de distribution](lot-8/app-distribution-model.md) |

> **Rappel de niveau** : cette agrégation reste au **niveau BOM candidate**
> (§3, niveaux 1–2) — **jamais** une BOM de production (§4). La **shortlist
> qualifiée** (niveau 3) ne sera produite qu'après les **rapports de mesure
> reproductibles** des **campagnes de la Phase 1** (volet expérimental). Voir la
> [feuille de route des décisions](lot-9/decision-roadmap.md).

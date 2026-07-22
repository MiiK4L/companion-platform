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

| Fonction | Candidat A | Candidat B | Candidat C / repli |
|---|---|---|---|
| Contrôleur écran | ST7789 | GC9A01 | ILI9341 |
| Moteur graphique | LVGL | Couche pilote LovyanGFX / TFT_eSPI | Sharp Memory LCD monochrome (repli) |
| RTC | PCF8563 | RV-3028-C7 | DS3231 ou RTC interne |
| Accéléromètre | BMA400 | LIS2DH12 | LIS2DW12 |
| Jauge (fuel gauge) | MAX17048 | LC709203F | BQ27441 ou ADC |
| Charge + power-path | BQ24074 | BQ25185 | MCP73831 ou TP4056 |
| Load switch | TPS22918 | AP22913 | P-MOSFET |
| Isolation de bus | TCA9517 | PCA9306 | Expander + multiplexeur |
| Expander GPIO | TCA9535 | PCA9555 | MCP23017 ou échelle ADC |
| Support d'identification | 24AA02E64-EUI | AT24C64 | 1-Wire DS28E07 ou µC actif |
| Connecteur CX-Bus | Card-edge | Board-to-board Hirose DF | FPC ou pogo |
| Haptique | ERM + transistor | LRA + DRV2605L | — |

Rappel : tous ces éléments sont **Proposés**. La comparaison objective (mesures, seuils,
reproductibilité) tranche ; ce tableau ne fait qu'ouvrir l'espace des options.

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

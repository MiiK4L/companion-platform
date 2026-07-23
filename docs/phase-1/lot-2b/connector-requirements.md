<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2B — Exigences fonctionnelles du connecteur (*Proposé*, `DEC-L2-002`)

> **Statut : Ouvert / Proposé.** Besoins **indépendants de la famille** de
> connecteur. Valeurs **[DS]/[C]/[H]/[P]/[BL]** ; **aucune [M]**. Ces exigences
> **cadrent** la [comparaison des familles](families-comparison.md) sans en
> présélectionner aucune.

## 1. Besoins fonctionnels (indépendants de la famille)

| Exigence | Besoin (origine) | Cible provisoire | Étiquette |
|----------|------------------|------------------|-----------|
| Nombre de contacts | Bus (I²C + SPI partagés + IRQ) + alim + présence + `GND` (cf. [budget GPIO L1](../lot-1/gpio-bus-budget.md)) | à consolider (`[BL]`) | **[BL]** |
| Courant par contact | Alim module `VMOD` + marge | ≥ courant `VMOD` max (`[BL]`, cf. [2A](../lot-2/power-architecture.md)) | **[BL]** |
| Pas (pitch) | Densité vs robustesse d'insertion manuelle | à comparer par famille | **[H]** |
| Hauteur / encombrement | **Format poche** (cf. [budgets v0 L1](../lot-1/budgets-v0.md)) | minimal ; contrainte, non figée | **[H]** |
| Rétention / verrouillage | Tenue en usage nomade, anti-arrachement | à comparer par famille | **[H]** |
| Détrompage | Éviter l'inversion / mauvaise insertion | requis (moyen à comparer) | **[P]** |
| Robustesse d'insertion | Insertions/retraits répétés par l'utilisateur | ≥ 1000 cycles (cf. [protocoles](protocols/README.md)) | **[P]** |
| Possibilité de séquencement des contacts | Séquencement de masse/alim **candidat** (cf. [pinout](pinout.md)) | souhaitable, **non garanti** selon famille | **[H]** |
| Coût | BOM prototype / V1 candidate | estimatif, par famille | **[H]** |
| Disponibilité multi-fournisseur | Pérennité (≥ 2 sources) | requis | **[P]** |

## 2. Contraintes transverses

- **Compatibilité électrique** avec les exigences du 2A (inrush, court-circuit,
  hot-plug) — le connecteur **ne doit pas** dégrader ces marges.
- **Topologie V1 = 1 slot** (cf. architecture) : pas de contrainte de chaînage.
- **Évolutivité** : le brochage **logique** doit pouvoir évoluer sans changer la
  famille (voir la distinction dans [pinout](pinout.md)).

## 3. Ce que ces exigences ne font pas

- Ne **présélectionnent** aucune famille ni aucun MPN.
- Ne **figent** ni le nombre exact de contacts, ni le pas, ni la hauteur (`[BL]`).

## Alimente

- **`DEC-L2-002`** (famille + mécanique + endurance) — via les exigences ci-dessus.
- Entrée de **`DEC-L2-003`** pour le **courant par contact** des rails exposés.

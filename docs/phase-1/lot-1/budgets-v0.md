<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 1 — Budgets v0 : énergie, mécanique, coût (*Proposé*, à affiner/mesurer)

> **Statut : Proposé / v0.** Estimations **documentaires** destinées au cadrage.
> **Aucun chiffre n'est décrété** : l'énergie sera **mesurée** (L6/L5), la
> mécanique affinée (L2/L4/L5 puis Phase 4), le coût consolidé en BOM V1
> candidate (L9). Aucune cible d'autonomie n'est fixée ici.

## 1. Budget énergétique v0

### Scénario d'usage de référence (mesurable, à valider)

Proposé pour servir de base **mesurable** (à confirmer) : sur 24 h — écran actif
par courtes sessions (ex. quelques minutes cumulées), **deep-sleep** le reste du
temps, réveils périodiques (RTC + bouton + mouvement), radio par **salves**
courtes, module CX-Bus **hors tension** sauf usage ponctuel.

### Postes de consommation (tous **à mesurer**)

| Poste | Ordre de grandeur (datasheet, indicatif) | À mesurer en |
|-------|------------------------------------------|--------------|
| Deep-sleep système (MCU + périphériques + fuites carte) | dizaines de µA ? (très dépendant du module) | **L5/L6** |
| MCU actif | dizaines à centaines de mA | L6 |
| Écran + rétroéclairage | poste **dominant** attendu ; dépend du panneau/luminosité | **L4/L6** |
| Radio Wi-Fi/BT (salves) | pics importants, moyennés sur salves | L6 |
| Module CX-Bus actif | plafond à définir (power-gating hors usage) | L2/L6 |

> ⚠️ Les ordres de grandeur ci-dessus sont **indicatifs (documentation)** et ne
> constituent pas des mesures. Le **budget mesuré (v1)** et une **cible
> d'autonomie provisoire** sont produits en **L6**.

## 2. Enveloppe mécanique v0

Objet **de poche** ; l'enveloppe est **dictée** par : batterie (format/capacité,
L6), écran (taille/format, L4), connecteur CX-Bus + slot (L2), cœur
**remplaçable** (socket ou alternative, épaisseur, L5). **Aucune dimension
figée** : contrainte de forme uniquement à ce stade ; croquis d'encombrement
affiné une fois les composants candidats mesurés (entrée pour la Phase 4).

## 3. Budget coût v0

Estimation **grossière** par familles de composants candidats (cf.
[sourcing & BOM](../sourcing-and-bom.md)) : cœur (module), écran, alimentation
(charge + régulation + éventuelle jauge + load switch), base de temps,
accéléromètre, connecteur, passifs, batterie, boîtier. Les **prix réels
(multi-distributeurs, datés)** sont relevés **au moment des lots** ; la **BOM V1
candidate chiffrée** est consolidée en **L9**. Aucun coût unitaire n'est arrêté
ici.

## Sortie du lot

Budgets **v0 établis comme référence de cadrage**, tous **Proposé** ; l'énergie
mesurée, la mécanique affinée et le coût consolidé relèvent des lots ultérieurs.
Aucune ADR créée.

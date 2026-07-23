<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 1 — Comparaison des architectures de cœur (*Proposé*, `DEC-L1-002`)

> **Statut : Ouvert / Proposé.** Comparaison **documentaire** ; les cellules
> « mesure » relèvent de lots matériels (L4/L5) et **ne sont pas** renseignées
> ici. Le principe accepté est le **cœur remplaçable** ([ADR-0004](../../adr/0004-coeur-de-calcul-socket.md)) —
> **pas** un MCU définitif. Aucune ADR n'est créée à ce stade.

## Options comparées (au même niveau)

- **(A) XIAO ESP32-S3** — candidat de **référence** de prototypage.
- **(B) Autre module ESP32-S3** exposant davantage de GPIO (module tiers socketable).
- **(C) Carte custom** autour de l'ESP32-S3 nu (Option C d'[ADR-0004](../../adr/0004-coeur-de-calcul-socket.md)).

## Grille de comparaison

`doc` = évaluable sur documentation en L1 · `mesure (Lx)` = à produire par un lot matériel.

| Critère | (A) XIAO ESP32-S3 | (B) Autre module ESP32-S3 | (C) Carte custom | Source |
|---------|-------------------|---------------------------|------------------|--------|
| GPIO exposés vs budget | 11 (serré, cf. [budget GPIO](gpio-bus-budget.md)) | potentiellement > 11 | libre (par conception) | doc (L1) |
| Bus (I²C/SPI/UART) | oui (défaut) | oui | libre | doc (L1) |
| RAM/flash/PSRAM | 8 Mo flash + 8 Mo PSRAM (à confirmer) | selon module | libre | doc (L1) → mesure L4 |
| Deep-sleep réel | à mesurer | à mesurer | à mesurer/optimiser | **mesure (L5)** |
| USB-C natif | oui (intégré) | selon module | à concevoir | doc (L1) |
| Radio Wi-Fi/BT | oui | oui | oui (ESP32-S3) | doc (L1) |
| Charge batterie | embarquée | selon module | à concevoir (L6) | doc (L1) |
| Dimensions / intégration poche | très compact | variable | optimisable | doc (L1) → L5 |
| Remplaçabilité (socket) | à valider | à valider | n/a (soudé) | **mesure (L5)** |
| Disponibilité / pérennité | à sourcer | à sourcer | dépend BOM | sourcing (continu) |
| Effort d'intégration / maintenance | faible (module prêt) | faible-moyen | **élevé** (RF/USB/alim) | doc (L1) |

*(Aucune conclusion : la grille s'enrichit des mesures des lots suivants.)*

## Déclencheurs d'arbitrage (rappel)

L'atteinte d'un [déclencheur d'arbitrage](../requirements-matrix.md) (GPIO
non couvrable, deep-sleep > cible L6, RAM/flash insuffisantes, indisponibilité)
**lance une comparaison documentée** entre A/B/C, réduction de périmètre,
expander/partage de bus et optimisation logicielle — **sans rejet automatique**
du candidat de référence ni sélection implicite d'une option. Une bascule peut
**rouvrir [ADR-0004](../../adr/0004-coeur-de-calcul-socket.md)**.

## Sortie du lot

- `DEC-L1-002` reste **Ouvert** ; (A) confirmé comme **candidat de référence**
  de prototypage (pas une décision de cœur définitif).
- Les mesures conditionnant l'arbitrage sont **planifiées en L4 (RAM/écran)** et
  **L5 (deep-sleep, socket)**.

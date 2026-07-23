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

**État de chaque critère** (à faire évoluer au fil des lots) :

- **Documenté** — évalué sur documentation ; ne vaut **pas** mesure.
- **À mesurer (Lx)** — à produire sur banc par le lot indiqué.
- **Validé (Lx)** — mesuré **et** conforme au [cadre de validation](../validation-framework.md) ; **aucun critère n'est Validé à ce stade**.

| Critère | (A) XIAO ESP32-S3 | (B) Autre module ESP32-S3 | (C) Carte custom | État |
|---------|-------------------|---------------------------|------------------|------|
| GPIO exposés vs budget | ~11 (serré, cf. [budget GPIO](gpio-bus-budget.md)) | potentiellement > 11 | libre (par conception) | Documenté |
| Bus (I²C/SPI/UART) | oui (défaut) | oui | libre | Documenté |
| RAM/flash/PSRAM | 8 Mo flash + 8 Mo PSRAM (à confirmer) | selon module | libre | Documenté → À mesurer (L4) |
| Deep-sleep réel | à mesurer | à mesurer | à mesurer/optimiser | À mesurer (L5) |
| USB-C natif | oui (intégré) | selon module | à concevoir | Documenté |
| Radio Wi-Fi/BT | oui | oui | oui (ESP32-S3) | Documenté |
| Charge batterie | embarquée | selon module | à concevoir (L6) | Documenté |
| Dimensions / intégration poche | très compact | variable | optimisable | Documenté → À mesurer (L5) |
| Remplaçabilité (socket) | à valider | à valider | n/a (soudé) | À mesurer (L5) |
| Disponibilité / pérennité | à sourcer | à sourcer | dépend BOM | Documenté (sourcing continu) |
| Effort d'intégration / maintenance | faible (module prêt) | faible-moyen | **élevé** (RF/USB/alim) | Documenté |

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

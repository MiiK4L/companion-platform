<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 5 — Déclencheurs d'arbitrage (autre module / carte custom) (*Proposé*)

> **Statut : Ouvert / Proposé.** Définit **ce qui déclenche** l'étude d'une
> **alternative de cœur** (autre module ou carte custom). **Aucun rejet
> automatique**, **aucune sélection implicite** : un déclencheur **lance une
> comparaison documentée**. Peut **rouvrir [ADR-0004](../../adr/0004-coeur-de-calcul-socket.md)
> / `DEC-L1-002`**. Valeurs `[BL]` ; **aucune `[M]`**.

## Déclencheurs (à confirmer par mesure)

| Déclencheur | Signal mesuré | Conséquence |
|-------------|---------------|-------------|
| **Deep-sleep hors cible** (R3) | courant deep-sleep système > cible (L6) | **structurant** — arbitrage cœur (autre module / custom) |
| **Budget GPIO non couvrable** | besoin > E/S exposées, même avec expander (L1) | réexamen module / custom |
| **RAM/PSRAM insuffisante** | budget moteur graphique (L4) hors capacité | réexamen module / custom |
| **Contact/épaisseur socket inacceptables** | résistance/dérive/épaisseur hors seuils (L5) | revoir support **ou** intégration soudée (custom) |
| **Disponibilité / pérennité** | module candidat NRND/EOL (R8) | sourcing / alternative |

## Options d'arbitrage (au même niveau, aucune présélectionnée)

- **(A) Rester** sur le module de référence (optimisations logicielles/matérielles).
- **(B) Autre module** ESP32-S3 exposant davantage / meilleur deep-sleep.
- **(C) Carte custom** (ESP32-S3 nu) — intègre le cœur (proche de la baseline
  soudée), au prix de l'effort RF/USB/alim ([ADR-0004](../../adr/0004-coeur-de-calcul-socket.md) option C).

## Traçabilité

Tout déclenchement est **documenté** (mesure → déclencheur → arbitrage), et une
bascule **rouvre explicitement** [ADR-0004](../../adr/0004-coeur-de-calcul-socket.md)
/ `DEC-L1-002` (cf. [risques](../risk-register.md) R3).

## Alimente

- **`DEC-L1-002`** (cœur) et **`DEC-L5-001`** (support) — sans arbitrage automatique.

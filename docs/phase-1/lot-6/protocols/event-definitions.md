<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 6 — Définitions instrumentées des événements (énergie)

> **Statut : Brouillon.** Référentiel **commun** aux
> [protocoles énergie](README.md). Chaque événement est **observable et
> mesurable**. **Seuils `[BL]`** (définis au baselining) ; **aucune `[M]`**.

## Événements et définition instrumentée

| Événement | Définition instrumentée | Observable | Seuil |
|-----------|-------------------------|------------|-------|
| **Conso par mode** | courant moyen dans un mode donné, à une tension `VBAT` | µA/mA-mètre, fenêtre par mode | valeur mesurée (budget) |
| **Erreur d'estimation d'état** | \|SOC estimé − SOC référence coulomb\| | approche testée vs coulombmètre de référence | ≤ **[BL]** |
| **Fuite module coupé** | courant résiduel vers le slot, `VMOD` coupé | µA-mètre sur `VMOD` | ≤ **[BL]** µA |
| **ΔT en charge** | échauffement max pendant charge complète | thermocouple / caméra IR | ≤ **[BL]** |
| **Capacité utile** | charge restituée en décharge réelle (mAh) | coulombmètre de référence | valeur mesurée |
| **Coupure sous-tension (UVP)** | arrêt propre à `VBAT` bas | tension + état système | seuil **[BL]** |

## Notes

- **Coins de tension `VBAT`** : mesures répétées à **4,2 / 3,7 / 3,4 / 3,0 V**
  (représentatifs de la décharge) ; ces tensions sont des **conditions de mesure**,
  pas des seuils de verdict.
- **Référence** d'estimation = **coulombmètre** indépendant (vérité terrain).
- Corrélation : chaque relevé horodaté, rattaché au DUT (cellule/carte) + config +
  commit firmware ; SHA-256 au rapport.

## Renvois

- [Index des protocoles & cycle de vie](README.md) · [Cadre de validation](../../validation-framework.md)

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 4 — Énergie & rétroéclairage (*Proposé*, `DEC-L4-001`)

> **Statut : Ouvert / Proposé.** L'écran est un **poste de consommation dominant
> attendu** (R4). Analyse **documentaire** ; les valeurs réelles sont **mesurées**
> (protocoles, ↔ [L6](../plan.md)). Valeurs `[DS]/[H]/[BL]` ; **aucune `[M]`**.

## 1. Poste rétroéclairage (familles rétroéclairées)

| Aspect | Contenu | Étiquette |
|--------|---------|-----------|
| Conso **par niveau** de rétroéclairage (PWM) | courbe conso ↔ luminosité | `banc` |
| Plage de luminosité utile | min lisible → max | **[BL]** |
| Gradation automatique | selon usage/ambiance | **[H]** |

> **Familles sans rétroéclairage** : **Memory LCD** (réflectif, ~µW statique
> **[DS]**) et **OLED** (émissif, conso liée au **contenu**) ; **transflectif** =
> appoint faible. Le poste « backlight » y est **nul ou marginal**.

## 2. Extinction automatique & reprise

| Comportement | Exigence candidate | Étiquette |
|--------------|--------------------|-----------|
| Extinction auto (veille) | après délai ; coupe backlight / met l'écran en veille | **[BL]** |
| **Reprise** | réveil écran + ré-affichage cohérent, latence bornée | `banc` (perçue) |
| Conservation de contenu | Memory LCD **garde l'image** sans alim active **[DS]** ; TFT/OLED non | **[DS]** |

> La **latence de reprise** est un critère de **performance perçue** (voir
> [protocole dédié](protocols/perceived-performance.md)).

## 3. Lien budget d'autonomie

La conso écran mesurée doit être **compatible de la cible d'autonomie** (arbitrée
en **L6**). Un dépassement peut **réduire la résolution/luminosité** ou
**reconsidérer la famille** (ex. repli Memory LCD) — **sans décider ici**.

## Alimente

- **`DEC-L4-001`** — la conso écran (par famille/niveau) est un **critère
  d'arbitrage** ; mesurée en L4/L6. **Aucune cible figée ici.**

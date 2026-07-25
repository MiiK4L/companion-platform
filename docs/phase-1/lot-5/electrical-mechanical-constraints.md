<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 5 — Contraintes électriques & mécaniques (*Proposé*, `DEC-L5-001`)

> **Statut : Ouvert / Proposé.** Cadre les contraintes que **tout support** doit
> satisfaire. Valeurs `[DS]/[H]/[BL]` ; **aucune `[M]`**.

## 1. Contraintes électriques

| Contrainte | Exigence candidate | Étiquette |
|-----------|--------------------|-----------|
| **Résistance de contact** | ≤ `[BL]` mΩ ; dérive bornée sur cycles (cf. [usure](protocols/support-wear-repeated-swap.md)) | **[BL]** |
| **Intégrité du signal** | supporte SPI/QSPI écran + CX-Bus sans dégradation (R2) | `banc` |
| **Courant admissible / contact** | ≥ courant d'alim du cœur (pics inclus) | **[BL]** |
| **Résistance série ↔ conso** | contribution du support à la conso (surtout deep-sleep) mesurée | `banc` |
| **Continuité sous sollicitation** | 0 intermittence > seuil sous vibration | **[P]/[BL]** |

## 2. Contraintes mécaniques

| Contrainte | Exigence candidate | Étiquette |
|-----------|--------------------|-----------|
| **Épaisseur / hauteur ajoutée** | minimisée (impact boîtier, Phase 4) ; **mesurée** | `banc` |
| **Rétention** | tenue du cœur en usage nomade (anti-arrachement/desserrage) | **[BL]** |
| **Tenue à la vibration** | maintien contact + alignement sous profil défini | `banc` |
| **Alignement / détrompage** | insertion correcte, anti-inversion | **[H]** |
| **Endurance d'insertion** | ≥ `[BL]` cycles sans dégradation | **[P]/[BL]** |

## 3. Couplage boîtier (Phase 4)

L'**épaisseur** du support est une **entrée** de la mécanique du boîtier (Phase 4)
et de l'enveloppe « poche » ([budgets v0 L1](../lot-1/budgets-v0.md)). Le Lot 5
**mesure** l'épaisseur ajoutée ; il ne fige pas le boîtier.

## Alimente

- **`DEC-L5-001`** — ces contraintes sont les **critères d'arbitrage** du support,
  mesurés par les [protocoles](protocols/README.md). **Aucun seuil figé ici.**

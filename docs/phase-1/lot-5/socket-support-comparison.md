<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 5 — Comparaison des supports de cœur (*Proposé*, `DEC-L5-001`)

> **Statut : Ouvert / Proposé.** Comparaison **au même niveau**, **sans
> présélection ni gagnant**. **5 familles socketables** + **baseline non
> démontable** (module soudé). Valeurs `[DS]/[H]/[BL]/inconnu` ; **aucune `[M]`**.
> Séries représentatives documentaires **sourcées** ([composants](candidate-components.md)).

## Familles comparées (socketables)

- **(S1) Embases 2,54 mm (barrettes)** — référence de prototypage.
- **(S2) Embases de précision (tulipe)** — contacts machinés multi-doigts.
- **(S3) Board-to-board / mezzanine** — lien [Lot 2B](../lot-2b/families-comparison.md).
- **(S4) Socket ZIF/LIF** — compression / levier (faible/nulle force d'insertion).

## Baseline non démontable (référence, **non remplaçable**)

- **(B0) Module directement soudé** — **castellations ou solution équivalente**
  selon l'implémentation. **Point de comparaison** électrique/mécanique/conso,
  **pas une solution remplaçable** (voir [README](README.md)).

## Grille comparative (critères communs)

`banc` = à mesurer (protocoles).

| Critère | (S1) Barrettes 2,54 | (S2) Tulipe précision | (S3) Mezzanine | (S4) ZIF/LIF | (B0) Soudé (baseline) |
|---------|---------------------|-----------------------|----------------|--------------|------------------------|
| Encombrement (surface) | moyen **[H]** | moyen **[H]** | faible **[H]** | élevé (mécanisme) **[H]** | **minimal** **[H]** |
| Hauteur / épaisseur | +2–3 mm **[H]** (à mesurer) | +2–3 mm **[H]** | faible (empilage) **[H]** | variable **[H]** | **minimale** `banc` |
| Facilité de remplacement | bonne **[H]** | bonne **[H]** | moyenne (rem-plug) **[H]** | **très bonne** (levier) **[H]** | **N/A** (soudé) |
| Cycles d'insertion | `banc` | `banc` (élevé attendu) | `banc` (faible attendu) | `banc` | **N/A** |
| Qualité / résistance de contact | `banc` | **meilleure attendue** (tulipe) **[H]** | `banc` | `banc` | **référence** `banc` |
| Coût | faible **[H]** | **élevé** **[H]** | moyen **[H]** | élevé **[H]** | **le plus faible** **[H]** |
| Disponibilité / secondes sources | multi-source **[DS]** | multi-source **[DS]** | multi-source **[DS]** | à évaluer **[H]** | N/A |
| Simplicité d'assemblage | simple **[H]** | simple **[H]** | SMT précis **[H]** | complexe (mécanisme) **[H]** | **soudure directe** |
| Risque de mauvais contact sous vibration | `banc` | **plus faible attendu** **[H]** | `banc` | `banc` (compression) | **nul** (soudé) `banc` |
| Facilité de prototypage | **excellente** **[H]** | bonne **[H]** | moyenne **[H]** | moyenne **[H]** | faible (dé/ressoudure) **[H]** |
| Impact sur la consommation (s'il existe) | `banc` (résistance série) | `banc` | `banc` | `banc` | **référence** `banc` |
| Évolutivité vers carte cœur custom | bonne **[H]** | bonne **[H]** | bonne **[H]** | moyenne **[H]** | **directe** (intégration) **[H]** |

*(Aucune conclusion : les cellules `banc` s'enrichissent des mesures.)*

## Synthèse par famille (avantages / limites / risques)

- **(S1) Barrettes 2,54 mm** — **+** coût, prototypage, dispo ; **−** encombrement,
  contact/vibration à valider ; **risque** desserrage en usage nomade.
- **(S2) Tulipe précision** — **+** contact et tenue vibration attendus meilleurs ;
  **−** coût ; **risque** hauteur/épaisseur.
- **(S3) Mezzanine** — **+** faible hauteur, densité ; **−** peu adapté au rem-plug
  fréquent (cf. L2B) ; **risque** usure en remplacements répétés.
- **(S4) ZIF/LIF** — **+** remplacement aisé, faible force ; **−** encombrement,
  coût, dispo pour un **support de module** (souvent orienté FFC) ; **risque**
  fiabilité mécanisme sous vibration.
- **(B0) Soudé (baseline)** — **+** conso/contact/épaisseur **de référence** ;
  **−** **non remplaçable** ; sert **uniquement** de point de comparaison.

## Alimente

- **`DEC-L5-001`** — la stratégie de socketage/support se fera **après** mesures
  (contact, endurance, vibration, épaisseur, conso) et arbitrage. **Aucun support
  retenu.** La baseline soudée **borne** la comparaison sans être une option
  remplaçable.

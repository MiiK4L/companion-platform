<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L5-RCONTACT — Résistance de contact du support

> **Statut : Brouillon (en attente de baselining).** Essai **matériel**.
> [Conventions](README.md) · [définitions](event-definitions.md). Seuils
> `[P]/[BL]` ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L5-001`** (qualité de contact du support).

## Objectif (une question mesurable)

La **résistance de contact** de chaque contact du support reste-t-elle dans sa
borne, avec une **dérive** bornée au fil des cycles ?

## Méthode 4 fils (Kelvin)

Source de courant et mesure de tension **séparées**, **par contact identifié** ;
comparaison **socketé** vs **soudé (baseline)**.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| R_contact initiale | ≤ `[BL]` mΩ | **[BL]** |
| Dérive cumulée `ΔR = Rn − R0` | ≤ `[BL]` mΩ | **[BL]** |
| Écart vs baseline soudée | ≤ `[BL]` | **[BL]** |

### Champs à finaliser au baselining (`[BL]`)

- R_contact max, dérive cumulée admissible, courant de mesure 4 fils.

## Plan d'essai (matériel)

- **`n_dut`** ≥ 2 supports · **tous** les contacts tracés · relevés couplés à
  l'[endurance](insertion-endurance-vibration.md) et à l'[usure](support-wear-repeated-swap.md) ·
  **`n_campaigns`** ≥ 2.

## Critères d'arrêt immédiat

R_contact hors borne franche répétée ; mesure 4 fils incohérente.

## Données brutes attendues

R_contact par contact et par palier, incertitude ; SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

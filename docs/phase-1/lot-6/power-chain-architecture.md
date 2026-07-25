<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 6 — Architecture d'alimentation du Host (*Proposé*, `DEC-L6-001`)

> **Statut : Ouvert / Proposé.** Vue **par fonctions d'architecture** de la chaîne
> Host. Les sous-fonctions **charge/power-path** et **régulation** sont détaillées
> dans leurs docs. Valeurs `[DS]/[H]/[BL]` ; **aucune `[M]`**.

## Chaîne (schéma logique)

```
USB-C (VBUS ≈ 5 V) ──▶ [Charge + power-path] ──┬──▶ système (3V3 régulé)
                                               └──▶ batterie (VBAT)
Batterie (VBAT) ──────────────────────────────────▶ [Régulation] ──▶ 3V3
[Estimation d'état] lit VBAT / courant                (DEC-L6-003)
[Power-gating module VMOD] ──────────────────────▶ slot   (relève de DEC-L2-003)
```

## Fonctions (chacune comparée séparément)

| Fonction | Question | Doc / décision |
|----------|----------|----------------|
| **Entrée USB-C** | tension/courant d'entrée ; OVP | ce doc |
| **Charge / power-path** | charge via cœur ou circuit dédié ? power-path ? | [charge](charge-powerpath-comparison.md) · `DEC-L6-001` |
| **Régulation** | LDO/buck/buck-boost ; courant dispo ; rails | [régulation](regulation-comparison.md) · `DEC-L6-001` |
| **Protections** | OVP/UVP/OCP, thermique, ESD entrée | ce doc |
| **Domaines d'alimentation** | quels rails, quels commutables | ce doc |
| **Alim du slot (`VMOD`)** | power-gating module | **`DEC-L2-003`** (non traité ici) |

## Domaines d'alimentation (candidats, non figés)

- **`VBUS`** (USB) : présent par intermittence ; charge + alim filaire.
- **`VBAT`** : source principale hors USB.
- **`3V3`** : rail logique régulé (cœur + périphériques).
- **`VMOD`** : rail **commuté** du slot — **`DEC-L2-003`** (rappel, non décidé ici).

## Protections (à cadrer)

OVP entrée (surtension USB), UVP batterie (coupure basse), OCP, thermique
(charge), ESD entrée USB-C. Les **seuils** sont `[BL]` (baselining).

## Entrées externes (budget)

Conso par mode ← [protocoles](protocols/README.md) ; deep-sleep ←
[L5](../lot-5/deep-sleep-measurement.md) ; conso écran ← [L4](../lot-4/power-and-backlight.md).

## Alimente

- **`DEC-L6-001`** — architecture d'alim (charge + power-path + régulation +
  protections + domaines), arbitrée **après** mesures. **Aucune architecture
  retenue.**

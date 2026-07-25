<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 6 — Architectures de régulation (*Proposé*, sous-fonction `DEC-L6-001`)

> **Statut : Ouvert / Proposé.** **Sous-comparaison** de l'[architecture
> d'alimentation](power-chain-architecture.md) — **pas** une décision indépendante.
> Les composants **illustrent**. Valeurs `[DS]/[H]/[BL]` ; **aucune `[M]`**.

## Architectures comparées

| Architecture | Principe | Illustré par (non retenu) |
|--------------|----------|---------------------------|
| **(R1) LDO** | régulation linéaire ; faible bruit ; rendement = Vout/Vin | TLV757P (Iq ~25 µA **[DS]**) |
| **(R2) Buck** | abaisseur à découpage ; **haut rendement** ; Iq très faible possible | TPS62840 (Iq ~60 nA **[DS]**) |
| **(R3) Buck-boost** | maintient 3V3 même quand `VBAT` < 3,3 V (fin de décharge) | (à sélectionner) |
| **(R4) Multi-rails** | plusieurs domaines (logique / écran / RF) régulés séparément | (combinaison) |

## Grille comparative

| Critère | (R1) LDO | (R2) Buck | (R3) Buck-boost | (R4) Multi-rails |
|---------|----------|-----------|-----------------|-------------------|
| Rendement | faible si Vin≫Vout **[H]** | **élevé** **[DS]** | élevé | selon |
| Conso à vide (Iq) | ~25 µA **[DS]** | **~60 nA** **[DS]** | à relever | somme des rails |
| Bruit / ondulation | **faible** | plus élevé (découpage) | découpage | selon rail |
| Maintien 3V3 en fin de décharge (`VBAT` < 3,3 V) | **non** | non (abaisseur) | **oui** | selon |
| Complexité / surface | faible | moyenne (inductance) | moyenne-élevée | élevée |
| Impact deep-sleep | Iq dominant | **Iq très faible** | à relever | somme |

> **Impact deep-sleep** : la conso à vide du régulateur est **critique** pour
> l'autonomie (R4) — mesurée conjointement au [deep-sleep L5](../lot-5/deep-sleep-measurement.md).

## Synthèse

- **LDO** : simple, faible bruit, mais rendement/Iq limitants ; **Buck** : rendement
  et Iq excellents, bruit à gérer ; **Buck-boost** : maintient 3V3 en fin de
  décharge (utile selon chimie) ; **Multi-rails** : découplage écran/RF/logique.

## Alimente

- **`DEC-L6-001`** — la régulation fait **partie** de l'architecture d'alim ;
  arbitrée avec elle **après** mesures (rendement, Iq, bruit). **Aucune retenue.**

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2A — Isolation & commutation du bus (*Proposé*, `DEC-L2-001`)

> **Statut : Ouvert / Proposé.** Options **au même niveau**, **sans
> présélection**. Valeurs **[DS]/[C]/[H]/[P]** (voir
> [convention](README.md)) ; **aucune valeur mesurée [M]**.

## 1. Objectifs de sûreté à satisfaire

1. **Module non alimenté ⇒ haute impédance** sur les lignes de bus (pas de
   charge parasite, pas de back-powering — voir [back-powering](back-powering.md)).
2. **Couper** l'alimentation module (`VMOD`) hors usage (power-gating).
3. **Borner l'inrush** à la mise sous tension (rampe contrôlée).
4. **Survivre** à un court-circuit franc côté slot sans dégât ni reset Host.
5. **Détecter et récupérer** un bus tenu bas (stuck) — voir
   [bus-stuck](bus-stuck-and-transaction.md).

## 2. Options de commutation d'alimentation (`VMOD`)

| Option | Principe | Atouts (analyse) | Points de vigilance |
|--------|----------|------------------|---------------------|
| **(C1) Load switch dédié** | CI load switch avec contrôle de rampe | Rampe/inrush maîtrisés, souvent protection intégrée (OCP/thermique) | Coût CI ; disponibilité |
| **(C2) P-MOSFET + rampe RC** | MOSFET discret + réseau de grille | Faible coût, flexible | Protection à ajouter ; réglage rampe manuel |
| **(C3) eFuse / power-switch protégé** | Commutation + limitation active de courant | Protection court-circuit robuste | Coût ; complexité |

Le choix relève de `DEC-L2-001` **après** mesures (inrush, court-circuit). Aucun
n'est présélectionné.

## 3. Options d'isolation des lignes de bus

| Option | Principe | Atouts (analyse) | Points de vigilance |
|--------|----------|------------------|---------------------|
| **(I1) Répéteur/tampon I²C isolant** (type TCA9517) | Tampon avec offset, isole capacité et niveaux | Découple la capacité du slot ; supporte level-shift | Ajoute un composant actif ; alimentation propre requise |
| **(I2) Level-shifter passif** (type PCA9306) | MOSFET bidirectionnel | Simple, faible coût | Pas de limitation active ; capacité de bus à surveiller |
| **(I3) Commutation active des lignes** (bus switch) | Déconnexion physique des lignes quand module absent/non alimenté | Garantit haute-Z ; isole les défauts | Coût ; latence de commutation |
| **(I4) Série résistive + clamp** | Résistances série + diodes de clamp | Très simple | Ne coupe pas ; protection limitée |

> Les options SPI (écran partagé, cf. **R2**) et I²C n'ont pas les mêmes
> contraintes : le SPI rapide tolère mal la capacité additionnelle — à vérifier
> au [protocole hot-plug/injection](protocols/hot-plug-injection.md) et en L4.

## 4. Séquencement (exigences fonctionnelles, non figées)

Séquence **esquissée** (à valider, non gelée) :

1. Détection présence module (ligne dédiée) **avant** toute mise sous tension.
2. Lignes de bus maintenues **haute-Z** tant que `VMOD` n'est pas stable.
3. Montée `VMOD` **à rampe contrôlée** (inrush borné).
4. Activation des tampons/level-shifters une fois `VMOD` stable.
5. Au retrait : bus haute-Z, coupure `VMOD`, décharge contrôlée.

## 5. Critères chiffrés proposés (à mesurer)

| Critère | Seuil | Étiquette | Alimente |
|---------|-------|-----------|----------|
| Impédance lignes bus, module non alimenté | ≥ 1 MΩ (borne d'analyse) | **[P]** | `DEC-L2-001` |
| Rampe `VMOD` | ≥ 1 ms | **[P]** | `DEC-L2-001` |
| Pic d'inrush | ≤ 2× courant établi + plafond absolu à fixer | **[P]** | `DEC-L2-001` |

Détails et conditions dans les [protocoles](protocols/README.md).

## 6. Alimente

- **`DEC-L2-001`** — principe d'isolation/commutation retenu **après** mesures.

## Renvois

- [Architecture de puissance](power-architecture.md) · [Composants candidats](candidate-components.md)
- [Protocoles](protocols/README.md) · [Risques électriques](electrical-risk-analysis.md)

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2A-INRUSH — Inrush & rampe de `VMOD`

> **Statut : Brouillon (en attente de baselining).** Seuils `[P]/[DS]/[C]/[H]` ;
> champs ouverts `[BL]` à finaliser et geler avant tout essai ; **aucune `[M]`**.
> Modèle : [protocole de test](../../templates/test-protocol-template.md) ·
> [définitions des événements](event-definitions.md).

## Lot & décision visée

Lot 2A · alimente **`DEC-L2-001`** (sûreté électrique). **Ne pas** référencer de
numéro d'ADR (créée après mesures).

## Objectif (une question mesurable)

À la mise sous tension de `VMOD` vers un module, **le pic d'inrush et la durée de
rampe respectent-ils les seuils**, à tous les coins de tension ?

## Conditions initiales

- `VMOD` coupé, module (ou charge équivalente) connecté, bus au repos.
- Load switch / commutation selon la variante testée
  ([isolation](../isolation-and-switching.md), options C1–C3).

## Instrumentation requise

- Oscilloscope (≥ 100 MHz) + **sonde de courant** (ou shunt + différentiel).
- Alimentation de labo à **limitation de courant** réglable.
- Charge : module réel **ou** charge capacitive/résistive représentative **[H]**.

## Montage / fixture

Banc d'injection ([hot-plug](../hot-plug.md) §1) commutant `VMOD` par
interrupteur/relais ; mesure du courant en série sur `VMOD`, tension aux bornes
de la charge.

## Conditions d'alimentation

Voir la [matrice partagée](../electrical-risk-analysis.md) (§2). **Rail observé :
`VMOD`.** Variantes testées : **VA** (`VMOD` régulé commuté) **et** **VB**
(`VMOD` = `VBAT` exposé) — l'inrush dépend de la topologie. Coins applicables :
VA → régulé ±5 % **+ near-dropout** (`VBAT` ≈ 3,0 V) ; VB → **batterie 3,0 / 4,2 V**.
État USB : **présent et absent**. Source : batterie et USB, documentée par run.

## Courant max / modèle de charge

Limitation alim réglée au **plafond d'inrush toléré** ; modèle de charge
documenté (capacité d'entrée module **[H]**).

## Seuils de réussite / échec chiffrés

Verdicts instrumentés : voir [définitions des événements](event-definitions.md).

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Pic d'inrush | ≤ 2× courant établi ; **plafond absolu** = `[BL]` A | **[P]/[BL]** |
| Durée de rampe `VMOD` | ≥ 1 ms | **[P]** |
| Reset Host pendant l'essai | 0 (def. instrumentée) | **[P]** |

### Champs à finaliser au baselining (`[BL]`)

- **Plafond d'inrush absolu** (A) : justifier depuis le courant établi max du
  module candidat et la tenue du commutateur (source à citer).
- **Modèle de charge** exact (capacité d'entrée) une fois le module candidat fixé.

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 (exemplaires distincts) · **`n_runs`** ≥ 10 (mises sous tension
  par variante et par coin) · **`n_campaigns`** ≥ 2 (indépendantes) ·
  **`n_cycles`** = n/a (pas d'endurance ici).
- **Total** = `n_dut` × `n_variantes×coins` × `n_runs` × `n_campaigns`
  (valeur exacte figée au baselining une fois VA/VB × coins arrêtés).
- **Répartition** : équilibrée par variante (VA/VB) × coin × DUT × campagne.
- **Ordre** : coins **randomisés** au sein d'une campagne (graine journalisée).
- **Repos** : refroidissement du commutateur entre séries (durée `[BL]`).
- **Reprise après échec** : sur arrêt immédiat, la série est **invalidée** et
  reprise après remise en état ; l'incident est journalisé (non rétroactif).
- **Méthode** : min/max/percentiles ; règle d'aberrants définie avant essai.

## Critères d'arrêt immédiat

- Courant > plafond de sécurité de l'alim.
- Échauffement anormal du commutateur (odeur/fumée) ou tension `VMOD` divergente.
- Reset Host répété.

## Remise en état entre campagnes

Couper `VMOD`, décharger les capacités, vérifier le commutateur (R_on/absence de
dommage), reconnecter, re-vérifier l'étalonnage de la sonde de courant.

## Éléments susceptibles d'être détruits

Load switch / MOSFET, module de test (charge), sonde de courant en cas de
dépassement.

## Données brutes attendues

Captures oscilloscope (courant + tension `VMOD`) par run/campagne, horodatées ;
stockage dans le dossier du lot (hash SHA-256 par fichier au moment du rapport).

## Sécurité opérateur

Limitation de courant active en permanence ; lunettes ; pas de contact sous
tension ; extincteur adapté à proximité.

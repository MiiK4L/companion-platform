<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2A-INRUSH — Inrush & rampe de `VMOD`

> **Statut : Proposé (figé avant essai).** Seuils **[P]/[DS]/[C]/[H]** ; **aucune
> [M]**. Modèle : [protocole de test](../../templates/test-protocol-template.md).

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

## Plage de tension

`3V3` ±5 % ; batterie ≈ 3,0 V et ≈ 4,2 V **[DS]** (coins, voir
[risques électriques](../electrical-risk-analysis.md) §2).

## Courant max / modèle de charge

Limitation alim réglée au **plafond d'inrush toléré** ; modèle de charge
documenté (capacité d'entrée module **[H]**).

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Pic d'inrush | ≤ 2× courant établi **+ plafond absolu à fixer** | **[P]** |
| Durée de rampe `VMOD` | ≥ 1 ms | **[P]** |
| Reset Host pendant l'essai | 0 | **[P]** |

## Reproductibilité

- `n_dut` ≥ 2 · `n_runs` ≥ 10 · `n_campaigns` ≥ 2 (indépendantes) · justifier
  selon dispersion. Méthode : min/max/percentiles ; règle d'aberrants définie
  avant essai.

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

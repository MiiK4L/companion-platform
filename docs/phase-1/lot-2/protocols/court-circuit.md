<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2A-SHORT — Court-circuit franc côté slot

> **Statut : Proposé (figé avant essai).** Seuils **[P]/[DS]/[H]** ; **aucune
> [M]**. Modèle : [protocole de test](../../templates/test-protocol-template.md).
> Essai **destructif potentiel**.

## Lot & décision visée

Lot 2A · alimente **`DEC-L2-001`** (protection court-circuit / choix du
commutateur protégé).

## Objectif (une question mesurable)

Lors d'un **court-circuit franc** de `VMOD` vers `GND` côté slot, le Host
**reste-t-il indemne** (0 reset, 0 dégât) et **reprend-il** à la levée du défaut ?

## Conditions initiales

- Module remplacé par une **fixture de court-circuit** commandée (relais/MOSFET).
- Commutation `VMOD` selon variante ([isolation](../isolation-and-switching.md)).

## Instrumentation requise

- Oscilloscope + sonde de courant ; alimentation à **limitation de courant**.
- Relais/MOSFET de court-circuit **commandé** (durée maîtrisée) ; thermomètre IR.

## Montage / fixture

Court-circuit appliqué **en aval** du commutateur `VMOD` ; mesure du courant de
défaut et de la tension `3V3`/`VBAT` Host pendant le défaut.

## Plage de tension

`3V3` ±5 % ; batterie ≈ 3,0 V et ≈ 4,2 V **[DS]** (coins).

## Courant max / modèle de charge

Limitation alim **impérative** ; profil : court-circuit **franc maintenu 10 s**,
répété **× 100** **[P]**.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Reset Host | 0 sur 100 défauts | **[P]** |
| Dégât matériel Host | 0 | **[P]** |
| Reprise à la levée du défaut | 100 % | **[P]** |
| Temps de coupure/limitation | ≤ valeur à fixer | **[P]** |

## Reproductibilité

`n_dut` ≥ 2 · `n_runs` = 100 défauts · `n_campaigns` ≥ 2 · justifier (caractère
destructif ⇒ documenter chaque DUT).

## Critères d'arrêt immédiat

- Chute de `VBAT`/`3V3` Host au-delà d'un seuil (risque reset généralisé).
- Échauffement > seuil au commutateur, fumée, odeur.
- Comportement non répétable du dispositif de court-circuit.

## Remise en état entre campagnes

Inspecter/remplacer le commutateur si dérive R_on ; laisser refroidir ;
re-vérifier l'alim et la fixture ; consigner tout composant remplacé.

## Éléments susceptibles d'être détruits

Load switch/MOSFET `VMOD`, pistes de la fixture, éventuellement l'étage de charge
Host si la protection est insuffisante (**c'est précisément l'objet du test**).

## Données brutes attendues

Captures courant de défaut + tensions Host, température, journal des 100 défauts,
horodatage ; SHA-256 par fichier au rapport.

## Sécurité opérateur

Limitation de courant obligatoire ; écran/lunettes ; manipulation à distance du
déclenchement ; surface ininflammable ; extincteur adapté.

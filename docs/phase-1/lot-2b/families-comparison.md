<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2B — Comparaison des familles de connecteurs (*Proposé*, `DEC-L2-002`)

> **Statut : Ouvert / Proposé.** Comparaison **fonctionnelle**, **au même
> niveau**, **sans présélection ni gagnant**. Les appréciations sont
> **documentaires [DS]/[H]** ; endurance et résistance de contact sont **à mesurer
> sur banc**. **Aucun MPN** (recherché seulement après qu'une famille soit
> retenue). **Aucune [M].**

## Familles comparées

- **(F1) Board-to-board / mezzanine** — carte-à-carte à pas fin (empilage).
- **(F2) Card-edge** — bord du PCB module inséré dans un slot (type cartouche).
- **(F3) FPC / FFC** — nappe flexible + connecteur ZIF.
- **(F4) Spring / pogo-pin** — contacts à ressort.
- **(F5) Filaire verrouillable (témoin)** — wire-to-board polarisé/verrouillable
  (familles type JST / Molex / Hirose), servant de **référence témoin**.

## Grille fonctionnelle

`banc` = à établir par la mesure (protocoles) ; le reste = appréciation documentaire.

| Critère | (F1) Mezzanine | (F2) Card-edge | (F3) FPC/FFC | (F4) Pogo | (F5) Filaire (témoin) |
|---------|----------------|----------------|--------------|-----------|------------------------|
| Coût estimatif | moyen | faible | faible-moyen | moyen-élevé | faible-moyen |
| Encombrement | faible (empilage) | fin | très fin | variable (hauteur ressort) | volumineux (câble+boîtier) |
| Nb de contacts réalisables | élevé | moyen-élevé | élevé | moyen | moyen |
| Pas disponible | fin | variable | fin | moyen | large |
| Courant admissible / contact | faible-moyen | moyen | faible | moyen | élevé |
| Séquencement des contacts | difficile (contacts égaux) | **possible** (doigts échelonnés) | difficile | **possible** (pins de longueurs ≠) | non (simultané) |
| Comportement en hot-plug | peu adapté (rem-plug) | adapté (cartouche) | peu adapté | adapté | manuel, peu ergonomique |
| Endurance (cycles) | `banc` | `banc` | `banc` (faible attendu) | `banc` | `banc` |
| Résistance de contact | `banc` (4 fils) | `banc` (4 fils) | `banc` (4 fils) | `banc` (4 fils) | `banc` (4 fils) |
| Tolérance mécanique (désalignement) | faible (alignement précis) | moyenne | faible | **bonne** (compliance) | bonne |
| Facilité d'assemblage | SMT standard | slot + bord PCB | connecteur ZIF | SMT / spécifique | sertissage / manuel |
| Disponibilité multi-fournisseur | bonne | bonne | bonne | moyenne | très bonne |
| Risque de mauvaise insertion | faible | moyen (sens) | moyen | faible-moyen | faible |
| Possibilité de détrompage | oui (usuel) | à concevoir (encoche) | selon série | à concevoir | oui (polarisé) |
| Évolutivité (contacts futurs) | bonne | moyenne | bonne | moyenne | moyenne |
| Impact sur la mécanique produit | empilage (épaisseur) | fente dans le boîtier | logement de nappe | surface + guidage | logement câble |
| Complexité de fabrication | faible (SMT std) | faible (bord PCB) | moyenne | moyenne-élevée | moyenne (assemblage câble) |

*(Aucune conclusion : la grille s'enrichit des mesures des protocoles.)*

## Synthèse par famille (avantages / limites / risques)

### (F1) Mezzanine
- **Avantages** : densité, faible hauteur, détrompage usuel, dispo, fabrication simple.
- **Limites** : peu adapté au **rem-plug fréquent** ; tolérance d'alignement faible ; séquencement de contacts difficile.
- **Risques** : usure prématurée si insertions répétées ; sensibilité au désalignement.

### (F2) Card-edge
- **Avantages** : coût faible, hot-plug « cartouche » naturel, **séquencement possible** (doigts de longueurs différentes), fin.
- **Limites** : détrompage à concevoir ; endurance/résistance de contact du **bord de PCB à valider**.
- **Risques** : usure du bord PCB, oxydation des contacts, insertion en sens inverse.

### (F3) FPC / FFC
- **Avantages** : très fin, nombreux contacts, dispo.
- **Limites** : **peu adapté au rem-plug** utilisateur ; endurance faible attendue ; fragilité de la nappe.
- **Risques** : rupture de nappe, déboîtement du ZIF en usage nomade.

### (F4) Spring / pogo-pin
- **Avantages** : insertion/retrait **sans usure de bord**, bonne **compliance** (tolérance), hot-plug aisé, **séquencement possible**.
- **Limites** : coût, rétention à concevoir, dispo moyenne.
- **Risques** : perte de force de contact dans le temps, contamination des pointes.

### (F5) Filaire verrouillable (témoin)
- **Avantages** : courant élevé, verrouillage/détrompage natifs, très disponible, robuste électriquement.
- **Limites** : **encombrant**, non « cartouche », ergonomie de rem-plug médiocre.
- **Risques** : contrainte mécanique sur les fils, format incompatible « poche » compact.

## Alimente

- **`DEC-L2-002`** — la sélection de famille se fera **après** mesures (endurance,
  résistance de contact) et arbitrage ; **aucune famille n'est retenue ici**.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L5-SWAP — Usure du support (remplacements successifs de cartes cœur)

> **Statut : Brouillon (en attente de baselining).** Essai **matériel** couvrant
> le **cas d'usage principal du projet** : **remplacement successif de plusieurs
> cartes cœur *différentes* sur un même support**. [Conventions](README.md) ·
> [définitions](event-definitions.md). Seuils `[P]/[BL]` ; **aucune `[M]`**.
> Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Décision visée

Alimente **`DEC-L5-001`** (aptitude du support au **remplacement répété**).

## Objectif (une question mesurable)

Après de **nombreux remplacements** de **cartes cœur différentes** sur **un même
support**, le support conserve-t-il sa **qualité de contact**, son **alignement**,
et **sans déformation ni détérioration** ?

## Spécificité (à ne pas confondre avec l'endurance simple)

- [Endurance simple](insertion-endurance-vibration.md) : **une même** carte,
  cycles répétés → usure **combinée** support+carte.
- **Ce protocole** : **plusieurs cartes cœur différentes** insérées tour à tour
  sur **le même support** → on isole l'**usure du support** et sa capacité à
  **accueillir des cartes distinctes** (tolérances/variabilité inter-cartes).

## Grandeurs mesurées (par cycle de remplacement)

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| **Usure du support** (marquage/déformation) | pas de dégradation compromettant le contact | **[P]** |
| **Qualité de contact** (R_contact, dérive) | ≤ borne + dérive `[BL]` (cf. [4 fils](contact-resistance-socket.md)) | **[P]/[BL]** |
| **Maintien de l'alignement** | dans tolérance `[BL]` sur toutes les cartes | **[P]/[BL]** |
| **Déformation / détérioration** | 0 (inspection + force de rétention ≥ `[BL]` N) | **[P]/[BL]** |
| **Variabilité inter-cartes** | contact/alignement conformes **quelle que soit** la carte | **[BL]** |

### Champs à finaliser au baselining (`[BL]`)

- **Nombre de remplacements** cible ; **nombre de cartes cœur distinctes** ;
  tolérances d'alignement ; grille de cotation de l'usure ; seuils R_contact/force.

## Plan d'essai (matériel)

- **`n_dut`** ≥ 2 **supports** · **≥ K cartes cœur différentes** (`[BL]`) permutées ·
  **`n_remplacements`** ≥ `[BL]` (par support) · **`n_campaigns`** ≥ 2 · 4 fils +
  inspection + mesure d'alignement + force de rétention à intervalles.
- **Traçabilité** : chaque relevé rattaché au **support** ET à la **carte** utilisée
  (numérotation stable) et au **numéro de remplacement**.

## Critères d'arrêt immédiat

Déformation visible, perte de rétention, dérive de contact franche.

## Remise en état / éléments destructibles

Essai **destructif par usure** du support ; **supports neufs par campagne** ; les
cartes cœur sont **réutilisées** (numérotées), remplacées si elles-mêmes s'usent.

## Données brutes attendues

R_contact/alignement/force par (support × carte × remplacement), photos d'usure ;
SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard ; manipulation soignée des cartes.

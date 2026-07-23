<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2B-ENDUR — Endurance d'insertion

> **Statut : Brouillon (en attente de baselining + annexe fixture par famille).**
> Générique, indépendant de la famille. Respecte les
> [conventions communes](README.md) et les
> [définitions d'événements](event-definitions.md). Seuils `[P]/[BL]` ; **aucune
> [M]**. Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 2B · alimente **`DEC-L2-002`** (endurance de la famille).

## Objectif (une question mesurable)

Après **≥ 1000 cycles** d'insertion/retrait, la famille conserve-t-elle sa
**continuité**, sa **rétention** et une **résistance de contact** dans les bornes
(dérive < 50 mΩ /100 cycles), **sans intermittence** > 1 µs ?

## Conditions initiales & fixture

Selon les [conventions communes](README.md) ; **fixture propre à la famille** =
**annexe créée au moment de l'essai** (non incluse ici). Charge électrique
appliquée pendant les cycles (`[BL]`).

## Instrumentation

Cycleur mécanique motorisé (vitesse réglable), mesure **4 fils** par contact,
détecteur d'intermittence, compteur de cycles, thermomètre.

## Seuils de réussite / échec chiffrés

Verdicts instrumentés : voir [définitions](event-definitions.md).

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Nombre de cycles | ≥ 1000 | **[P]** |
| Dérive de résistance de contact | < 50 mΩ /100 cycles | **[P]** |
| Intermittence | 0 événement > 1 µs | **[P]** |
| Force de rétention en fin d'endurance | ≥ `[BL]` N | **[P]/[BL]** |

### Champs à finaliser au baselining (`[BL]`)

- **Vitesse** de cyclage (mm/s), **alignement/désalignement** appliqués, **charge**
  électrique (A), **température**, **nettoyage** (autorisé/interdit) — cf. conventions.
- **Force de rétention minimale** acceptable (N).

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 (connecteurs distincts de la famille) · **`n_cycles`** ≥ 1000 ·
  **`n_runs`** = mesure 4 fils tous les **100 cycles** · **`n_campaigns`** ≥ 2.
- **Total** = `n_dut` × 1000 cycles × `n_campaigns` (figé au baselining).
- **Répartition** : par DUT × campagne ; relevés périodiques (tous les 100 cycles).
- **Reprise après échec** : défaillance de contact → arrêt sur ce contact,
  journalisation, non rétroactif.

## Critères d'arrêt immédiat

Défaillance mécanique (perte de rétention, casse) ; intermittence répétée au-delà
du seuil ; échauffement anormal du cycleur.

## Remise en état / éléments destructibles

Essai **destructif par usure** ; documenter chaque DUT. Remplacer le DUT usé entre
campagnes. Vérifier l'étalonnage 4 fils et l'alignement du cycleur.

## Données brutes attendues

R_contact/100 cycles par contact, journal d'intermittences, force de rétention,
photos d'inspection ; SHA-256 par fichier au rapport.

## Sécurité opérateur

Cycleur motorisé : protections mécaniques, arrêt d'urgence, pas de main dans la
zone de course.

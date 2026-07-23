<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2B-INTERM — Intermittence de contact

> **Statut : Brouillon (en attente de baselining + annexe fixture par famille).**
> Générique. [Conventions communes](README.md) ·
> [définitions d'événements](event-definitions.md). Seuils `[P]/[BL]` ; **aucune
> [M]**. Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 2B · alimente **`DEC-L2-002`** (tenue des contacts aux perturbations).

## Objectif (une question mesurable)

Sous **sollicitation** (vibration, choc léger, contrainte en usage nomade) et
sous charge, un contact présente-t-il une **ouverture transitoire > 1 µs** ?

## Conditions & instrumentation

Détecteur d'intermittence (seuil de tension/impédance, fenêtre temporelle) **par
contact**, sous charge électrique `[BL]` ; sollicitation mécanique définie
(profil `[BL]`). Fixture propre à la famille = **annexe créée à l'essai**.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Intermittence | 0 événement > 1 µs | **[P]** |
| Durée de détection | ≤ `[BL]` (résolution du détecteur) | **[BL]** |

### Champs à finaliser au baselining (`[BL]`)

- **Profil de sollicitation** (vibration/choc) représentatif de l'usage nomade.
- **Seuil et résolution** du détecteur d'intermittence.
- **Règle après intermittence** (poursuite/arrêt) — cf. [événements](event-definitions.md).

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 · tous les contacts surveillés · **`n_runs`** = durée/profil de
  sollicitation `[BL]` · **`n_campaigns`** ≥ 2.
- **Corrélation** obligatoire intermittence ↔ contact ↔ instant.

## Critères d'arrêt immédiat

Intermittences répétées au-delà du seuil (défaillance de contact) ; desserrage de
la fixture faussant la mesure.

## Remise en état / éléments destructibles

Faible risque ; vérifier le maintien mécanique de la fixture et l'étalonnage du
détecteur entre campagnes.

## Données brutes attendues

Journal horodaté des intermittences par contact, profil de sollicitation appliqué,
charge ; SHA-256 par fichier au rapport.

## Sécurité opérateur

Selon le moyen de sollicitation (vibreur/pot vibrant) : fixation, protections,
arrêt d'urgence.

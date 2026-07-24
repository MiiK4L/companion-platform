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

Détecteur d'intermittence **par contact**, sous charge électrique `[BL]`. Pour
qu'un critère « 0 événement **> 1 µs** » ait un sens, le détecteur doit être
**nettement plus rapide** que le seuil :

- **Résolution temporelle cible** : ≤ **1/10** du seuil → **≤ 0,1 µs** `[BL]`.
- **Bande passante minimale** et **fréquence d'échantillonnage** cohérentes avec
  cette résolution (`[BL]`, à justifier).
- **Seuil électrique d'ouverture** (tension/impédance au-delà duquel le contact
  est « ouvert ») `[BL]`.
- **Validation du détecteur** : **injection d'impulsions connues** (durées
  calibrées autour de 1 µs) avant campagne, pour prouver la détection.
- **Protection contre les faux événements** : rejet des artefacts de **fixture**
  et du **rebond normal d'insertion** (fenêtrage / masquage documenté).

## Deux scénarios distincts (à ne pas confondre)

1. **Surveillance pendant l'insertion/retrait complet** — transitoires
   d'engagement (le rebond d'insertion est **attendu** et **masqué**, pas compté).
2. **Contact déjà accouplé soumis à vibration/choc** — recherche d'ouvertures
   transitoires **en service** (aucun masquage : toute ouverture > seuil compte).

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Intermittence (scénario « accouplé sous vibration ») | 0 événement > 1 µs | **[P]** |
| Résolution temporelle du détecteur | ≤ 0,1 µs (1/10 du seuil) | **[BL]** |

### Champs à finaliser au baselining (`[BL]`)

- **Bande passante / fréquence d'échantillonnage / résolution** du détecteur.
- **Seuil électrique d'ouverture** et méthode de **validation par injection**.
- **Profil de sollicitation** (vibration/choc) représentatif de l'usage nomade.
- **Fenêtrage anti-faux-événements** (rebond d'insertion, artefacts fixture).
- **Règle après intermittence** — cf. [événements](event-definitions.md).

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

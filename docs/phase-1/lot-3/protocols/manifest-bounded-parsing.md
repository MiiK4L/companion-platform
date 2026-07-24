<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L3-PARSE — Parsing borné du Manifest

> **Statut : Brouillon (en attente de baselining).** [Conventions](README.md) ·
> [définitions d'événements](event-definitions.md). Seuils `[P]/[BL]` ; **aucune
> `[M]`**. Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 3 · alimente **`DEC-L3-002`** (format du Manifest — **parser Host borné**).

## Objectif (une question mesurable)

Le parser Host reste-t-il **borné** (RAM/temps plafonnés) et **refuse-t-il** toute
trame malformée **sans jamais allouer selon la taille annoncée** par le module ?

## Vecteurs de test (jeux de Manifests forgés)

Manifests valides + **malformés** : taille annoncée > limite Host, longueur
incohérente, TLV de type inconnu, version future, CRC faux, troncature, champ
obligatoire manquant.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Allocation selon taille annoncée non plafonnée | **0** (jamais) | **[P]** |
| RAM de parse | ≤ `[BL]` octets (plafond) | **[P]/[BL]** |
| Temps de parse | ≤ `[BL]` ms (borné) | **[P]/[BL]** |
| TLV inconnu | ignoré (sauté via longueur), 0 crash | **[P]** |
| Trame malformée | rejet borné, 0 crash / 0 débordement | **[P]** |

### Champs à finaliser au baselining (`[BL]`)

- **Limite Host** de taille de Manifest (octets) ; **plafonds RAM/temps** de parse.
- **Jeu de vecteurs** malformés figé ; périmètre exact du **CRC**.

## Plan d'échantillonnage

- **Vecteurs** : ensemble figé (valides + malformés) · **`n_runs`** = tous les
  vecteurs × répétitions · **`n_campaigns`** ≥ 2 (firmware Host + commit exact).
- Instrumentation : mesure RAM/temps (traces firmware), détection de crash/débordement.

## Critères d'arrêt immédiat

Crash / débordement mémoire (le test est précisément là pour l'exclure) →
consigner le vecteur fautif.

## Données brutes attendues

Journaux de parse (verdict par vecteur, RAM, temps), horodatés ; SHA-256 au rapport.

## Sécurité opérateur

Essai logiciel ; sans risque électrique particulier.

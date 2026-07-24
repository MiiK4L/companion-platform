<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 3 — Provisioning en production (chaîne de confiance, *Proposé*)

> **Statut : Ouvert / Proposé.** Le provisioning est traité comme une **chaîne de
> confiance opérationnelle**, pas un simple « écrire l'ID ». Valeurs `[H]/[BL]` ;
> **aucune `[M]`**. Alimente `DEC-L3-001` (identité) et informe `DEC-L3-002`
> (données du Manifest).

## Principe : l'unicité ne se déclare pas, elle se **vérifie**

> **Aucune identité n'est réputée unique au seul motif qu'un fournisseur
> l'annonce.** La stratégie de production doit **détecter les doublons**
> elle-même (registre + contrôle), quelle que soit la garantie fournisseur.

## Étapes de la chaîne (à couvrir au minimum)

| Étape | Exigence | Étiquette |
|-------|----------|-----------|
| **Génération ou lecture de l'identité** | native (EUI/ROM ID lus) **ou** générée + gravée | **[H]** |
| **Unicité & détection des doublons** | registre de production + contrôle **anti-doublon** | **[BL]** |
| **Écriture initiale** | séquence définie, idempotente | **[BL]** |
| **Vérification après écriture** | **relecture + comparaison** (read-back) obligatoire | **[H]** |
| **Protection / verrouillage** | write-protect / lock des zones fabricant | **[BL]** |
| **Reprise après échec** | reprise sûre (partiel/interrompu) sans doublon | **[BL]** |
| **Rework / RMA** | procédure de re-provisioning tracée (ou interdiction) | **[BL]** |
| **Traçabilité** | lien **identité ↔ lot de production ↔ révision matérielle** | **[BL]** |
| **Identité déjà utilisée** | détectée et **refusée** (collision registre) | **[H]** |
| **Séparation des données** | **fabricant (verrouillées)** vs **utilisateur (modifiables)** | **[BL]** |

## Séparation fabricant / utilisateur

Deux domaines distincts dans la mémoire :

- **Zone fabricant** — identité, révision, données d'usine : **verrouillée** après
  provisioning (write-protect).
- **Zone utilisateur** (si prévue) — données modifiables : **ne doit pas** pouvoir
  altérer l'identité ni invalider le Manifest fabricant.

## Selon l'architecture (renvoi)

Le provisioning diffère par architecture (identité **native** vs **provisionnée**,
cf. [architectures](identification-architectures.md) §1) : EUI/ROM ID **usine**
(lecture + registre) vs identité **entièrement gravée** (génération + anti-doublon
+ verrouillage). Détails et reproductibilité :
[protocole provisioning](protocols/provisioning-duplicate-detection.md).

## Alimente

- **`DEC-L3-001`** — la faisabilité et la **reproductibilité** du provisioning
  (avec **détection de doublons**) font partie des critères d'arbitrage.

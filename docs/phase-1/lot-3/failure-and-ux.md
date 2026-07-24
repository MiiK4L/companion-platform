<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 3 — Comportement en panne & expérience utilisateur (*Proposé*)

> **Statut : Ouvert / Proposé.** Définit les comportements attendus en cas
> d'anomalie et leur **traduction UX**. Valeurs `[H]/[BL]` ; **aucune `[M]`**.

## 1. Cas d'anomalie (comportement attendu, borné)

| Cas | Comportement attendu | Étiquette |
|-----|----------------------|-----------|
| **Corruption** (CRC invalide) | Manifest **rejeté** ; module « présent mais non décrit » ; pas d'exploitation des capacités | **[H]** |
| **Périphérique défaillant** (pas de réponse) | présence non confirmée ; état « module illisible » | **[H]** |
| **Réponse lente / bloqué** | **timeout borné** ([protocole](protocols/peripheral-absent-slow-stuck.md)) puis état d'échec | **[BL]** |
| **Identité partiellement lisible** | **jamais** de complétion « devinée » ; identité **incomplète = non fiable** | **[H]** |
| **Taille annoncée incohérente** | rejet borné (parser non aveugle, cf. [Manifest](manifest-format.md)) | **[H]** |
| **Doublon d'identité** (au champ) | signalé ; non traité comme deux instances distinctes | **[BL]** |

> Principe : en cas de doute, **dégrader explicitement** (module présent, non
> exploité) plutôt que **supposer**. Aucune décision produit sur une identité ou
> un Manifest **non fiables**.

## 2. Modèle de confiance (rappel, déjà acté)

Le **Module Manager publie** les capacités lues ; l'**App Manager décide** ;
**aucune installation/action automatique** sur la seule foi du Manifest. Un
Manifest **corrompu/partiel** ⇒ capacités **non publiées**.

## 3. Impact UX (détection, insertion, retrait, erreurs)

| Moment | Attendu côté utilisateur | Étiquette |
|--------|--------------------------|-----------|
| **Détection / insertion** | retour clair « module reconnu / non reconnu » ; latence bornée | **[BL]** |
| **Module illisible / corrompu** | message **distinct** de « pas de module » (aide au diagnostic) | **[H]** |
| **Module inconnu mais lisible** | « type non pris en charge » ≠ « module défaillant » | **[H]** |
| **Retrait** | état « module retiré » propre ; pas de blocage (cf. 2A retrait) | **[H]** |
| **Retrait pendant lecture** | pas de crash ; retour à **« aucune présence logique détectée » / « module non détectable »**, **sans conclure à l'absence physique** | **[H]** |
| **Erreurs répétées** | pas de boucle ; message stable ; journal pour diagnostic | **[BL]** |

> **Modèle interne & journaux** (le libellé utilisateur peut rester simple, mais
> l'état interne conserve la distinction) :
>
> - **absence physique confirmée** (uniquement si un mécanisme de présence dédié existe) ;
> - **absence de réponse** (découvrabilité logique négative) ;
> - **composant d'identification défaillant** (présent mais illisible) ;
> - **alimentation ou insertion indéterminée**.

> Distinguer nettement, pour l'utilisateur, **quatre états** : *aucune réponse* ·
> *module reconnu* · *module inconnu (mais lisible)* · *module illisible/défaillant*.
>
> ⚠️ **Sans signal de présence dédié**, *aucune réponse* **ne distingue pas**
> « module absent » de « module présent mais périphérique d'ID défaillant / mal
> alimenté / pas encore prêt » : le Host conclut à une **absence de présence
> logique**, non à une absence physique (cf.
> [architectures §1](identification-architectures.md)). Le message UX reste donc
> « aucun module détecté » sans **affirmer** l'absence physique.

## Alimente

- **`DEC-L3-001`** — la **gestion des pannes** (dont identité partielle) est un
  critère d'arbitrage ; **`DEC-L3-002`** — le mode **dégradé** du Manifest.

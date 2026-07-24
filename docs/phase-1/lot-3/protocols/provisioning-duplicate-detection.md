<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L3-PROV — Provisioning & détection des doublons

> **Statut : Brouillon (en attente de baselining).** [Conventions](README.md) ·
> [définitions d'événements](event-definitions.md). Seuils `[P]/[BL]` ; **aucune
> `[M]`**. Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 3 · alimente **`DEC-L3-001`** (faisabilité et reproductibilité du provisioning,
détection de doublons). Voir [provisioning](../provisioning.md).

## Objectif (une question mesurable)

La chaîne de provisioning est-elle **reproductible**, **vérifiée après écriture**,
**verrouillable**, et **détecte-t-elle les doublons** — sans jamais réputer une
identité unique sur la seule foi du fournisseur ?

## Déroulé testé

Génération/lecture d'identité → **contrôle anti-doublon (registre)** → écriture →
**relecture/comparaison** → verrouillage → cas d'échec/reprise → cas d'identité
déjà utilisée → séparation zone fabricant/utilisateur.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Vérification après écriture (read-back) | 100 % | **[P]** |
| Détection d'un doublon injecté | 100 % | **[P]** |
| Reproductibilité (même entrée → même état) | 100 % | **[P]** |
| Reprise après écriture interrompue sans doublon | 100 % | **[P]** |
| Écriture zone fabricant après verrouillage | **0** (refusée) | **[P]** |
| Altération de l'identité via zone utilisateur | **0** | **[P]** |

### Champs à finaliser au baselining (`[BL]`)

- **Format du registre** de production + règle d'unicité ; **traçabilité**
  identité ↔ lot ↔ révision.
- Politique **rework/RMA** (re-provisioning tracé ou interdit).

## Plan d'essai (type **provisioning**)

- **Nombre de pièces** ≥ `[BL]` (lot d'essai représentatif) · **registre de
  production** (simulé ou réel) · doublon(s) **injecté(s) volontairement** ·
  **répétitions / `n_campaigns`** ≥ 2 (script + commit exact).

## Données brutes attendues

Journaux de provisioning (par pièce : identité, read-back, verrou, registre),
horodatés ; SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard ; **données d'identité de test** uniquement (pas
d'identités de production réelles).

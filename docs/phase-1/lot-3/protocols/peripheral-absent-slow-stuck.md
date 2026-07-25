<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L3-ABSENT — Périphérique absent / lent / bloqué

> **Statut : Brouillon (en attente de baselining).** [Conventions](README.md) ·
> [définitions d'événements](event-definitions.md). Seuils `[P]/[BL]` ; **aucune
> `[M]`**. Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 3 · alimente **`DEC-L3-001`** (robustesse de lecture : absence, lenteur,
blocage).

## Objectif (une question mesurable)

Face à un périphérique d'identification **absent**, **lent** ou **bloqué**, le Host
**borne-t-il** son attente (timeout) et **retourne-t-il un état d'échec propre**
sans se bloquer ?

## Cas testés

Aucun module ; module présent mais périphérique d'ID muet ; réponse lente
(clock-stretch I²C prolongé) ; ligne maintenue (stuck) ; réponse tronquée.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Timeout de détection borné | ≤ `[BL]` ms | **[P]/[BL]** |
| Blocage du Host (boucle/hang) | **0** | **[P]** |
| État d'échec exposé (correct) | 100 % | **[P]** |
| Récupération du bus après stuck | 100 % (cf. [bus-stuck 2A](../../lot-2/bus-stuck-and-transaction.md)) | **[P]** |

### Champs à finaliser au baselining (`[BL]`)

- **Valeurs de timeout** (détection, lecture) ; profil de « réponse lente ».
- Lien avec la **récupération de bus** du Lot 2A.

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 · cas figés (absent/lent/bloqué/tronqué) · **`n_runs`** ≥ `[BL]`
  par cas · **`n_campaigns`** ≥ 2.

## Critères d'arrêt immédiat

Blocage durable non récupérable du bus/pilote (le test doit l'exclure) → consigner.

## Données brutes attendues

Journaux (cas → délai → état), captures bus ; SHA-256 au rapport.

## Sécurité opérateur

Faibles tensions ; ESD standard.

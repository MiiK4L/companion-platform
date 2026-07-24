<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 3 — Compatibilité faible consommation & hot-plug (*Proposé*)

> **Statut : Ouvert / Proposé.** Vérifie que l'identification est compatible avec
> les contraintes **déjà étudiées** en [Lot 2A](../lot-2/README.md) (sûreté
> électrique, hot-plug par injection) et [Lot 2B](../lot-2b/README.md)
> (connecteur). Valeurs `[H]/[BL]` ; **aucune `[M]`**.

## 1. Lecture **sans alimenter le reste du module**

Exigence : identifier le module **sans mettre sous tension l'ensemble** de ses
périphériques (économie d'énergie, sûreté). Implications par architecture :

| Architecture | Lecture rail d'identification isolé | Remarque |
|--------------|-------------------------------------|----------|
| EEPROM (+EUI) / simple | **Host-powered possible** **[H]** | alimentée par un rail d'ID limité, `VMOD` coupé |
| 1-Wire | Host-powered / parasite **[H]** | une ligne + masse |
| µC actif | **Module-powered** (doit démarrer) **[H]** | conso au boot ; à cadrer |
| Secure Element (+mém.) | Host-powered possible **[H]** | selon alimentation |

> Le **rail d'identification isolé** est une **entrée du Lot 2A**
> ([isolation/commutation](../lot-2/isolation-and-switching.md),
> [back-powering](../lot-2/back-powering.md)) : lire l'ID **avant** d'alimenter
> `VMOD`. Le choix `VBAT`/rails exposés reste `DEC-L2-003`.

## 2. Résolution non ambiguë (rappel neutre, cf. architectures §5)

Le mécanisme doit **résoudre sans ambiguïté** le module présent en **topologie
V1** et **définir son comportement** en cas de conflit / réponse multiple /
adresse non conforme — **décliné par bus** (I²C : collisions d'adresse ; 1-Wire :
recherche ROM ; µC : énumération). **Pas** de « 0 collision I²C » imposé.

## 3. Hot-plug pendant l'identification

- L'identification doit **tolérer** l'insertion/retrait (cf. hot-plug **électrique**
  du [Lot 2A](../lot-2/hot-plug.md)) : lecture **interrompue** = échec **propre**,
  pas de blocage (voir [protocole hot-plug pendant lecture](protocols/hotplug-during-read.md)).
- Séquencement : l'ID est lu **après** établissement de la masse et du rail d'ID,
  **avant** l'alimentation complète — cohérent avec le **séquencement candidat**
  du [Lot 2B](../lot-2b/pinout.md) (**à confirmer**, non figé).

## 4. Budget énergétique (renvoi)

La **consommation** (veille/lecture/écriture) est un **critère de comparaison**
([architectures](identification-architectures.md) §2) ; les valeurs réelles
relèvent des mesures (L5/L6). Ici : **compatibilité de principe** seulement.

## Alimente

- **`DEC-L3-001`** — « lecture sûre sans exposer les autres périphériques » +
  compat hot-plug/faible conso sont des **critères d'arbitrage**.

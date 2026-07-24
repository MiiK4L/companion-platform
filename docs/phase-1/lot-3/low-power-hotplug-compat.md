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
périphériques (économie d'énergie, sûreté). Cela suppose un **domaine
d'alimentation permettant, si l'architecture retenue l'exige, d'alimenter
uniquement la fonction d'identification sans activer les autres charges du
module** — **sans présumer** ici d'un rail ou d'un contact CX-Bus **nouveau**.

> **Le Lot 3 ne crée ni rail ni contact.** L'exposition des rails et la stratégie
> de puissance restent **ouvertes** en **`DEC-L2-003`** (arbitrage L2). Le Lot 3
> **exprime seulement des exigences de compatibilité future**.

**Exigences (candidates) du domaine d'alimentation de l'identification** :

| Exigence | Contenu | Étiquette |
|----------|---------|-----------|
| Courant maximal | plafond de la fonction d'ID seule | **[BL]** |
| Absence de back-powering | pas de ré-alimentation parasite (entrée [2A](../lot-2/back-powering.md)) | **[H]** |
| Temps de disponibilité | délai avant que l'ID soit lisible | **[BL]** |
| Comportement `VMOD` coupé | ID lisible **ou** non, défini ; pas de réveil des autres charges | **[H]** |
| Haute-Z des autres interfaces | interfaces non-ID en haute impédance pendant la lecture d'ID | **[H]** |

Implications **par architecture** (compatibilité de principe, non un choix de rail) :

| Architecture | Alimentation de la fonction d'ID | Remarque |
|--------------|----------------------------------|----------|
| EEPROM (+EUI) / simple | **Host-powered possible** **[H]** | passive |
| 1-Wire | Host-powered / parasite **[H]** | une ligne + masse |
| µC actif | **Module-powered** (doit démarrer) **[H]** | conso au boot ; à caractériser |
| Secure Element (+mém.) | Host-powered possible **[H]** | selon alimentation |

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

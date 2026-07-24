<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 3 — Architectures d'identification (*Proposé*, `DEC-L3-001`)

> **Statut : Ouvert / Proposé.** Comparaison **au même niveau**, **sans
> présélection ni gagnant**. Valeurs `[DS]/[C]/[H]/[BL]/N-A/inconnu` ; **aucune
> `[M]`**. Séries représentatives documentaires **sourcées**
> ([composants candidats](candidate-components.md)) ; **aucun MPN retenu**.

On distingue **trois natures** à ne pas mettre au même niveau :

1. **Architectures complètes d'identification** (§2) — fournissent présence +
   identité (produit/instance).
2. **Briques de stockage du Manifest** (§3) — stockent une **description**, **sans
   logique d'identification propre** ; **ne sont pas pénalisées ni favorisées**
   comme si elles répondaient seules aux trois couches.
3. **Compositions hybrides** (§4) — assemblages des briques précédentes.

## 1. Couverture des quatre informations (native / provisionnée / dérivée)

| Information | EEPROM + EUI | EEPROM simple | 1-Wire | µC actif | Secure Element (+ mém.) |
|-------------|--------------|---------------|--------|----------|--------------------------|
| Présence physique | dérivée (ACK bus) | dérivée | dérivée (présence 1-Wire) | dérivée | dérivée |
| Identité de produit | provisionnée | provisionnée | provisionnée | provisionnée / firmware | provisionnée |
| Identité d'instance | **native (EUI-64)** **[DS]** | provisionnée | **native (ROM ID 64 b)** **[DS]** | provisionnée / firmware | **native (clé/serial)** **[DS]** |
| Description (Manifest) | provisionnée (mémoire) | provisionnée | provisionnée (petite) | générée / provisionnée | provisionnée (**mémoire externe**) |

## 2. Architectures complètes — matrice sur critères communs

Colonnes = architectures ; toutes évaluées sur les **mêmes** critères.

| Critère | EEPROM + EUI | EEPROM simple | 1-Wire | µC actif | Secure Element (+ mém.) |
|---------|--------------|---------------|--------|----------|--------------------------|
| Consommation (veille/lecture/écriture) | faible (passif) **[H]** | faible **[H]** | faible **[H]** | **plus élevée** (actif) **[H]** | moyenne **[H]** |
| Temps de détection | à mesurer **[BL]** | **[BL]** | **[BL]** (recherche ROM) | **[BL]** (boot µC) | **[BL]** |
| Capacité mémoire utile | 2 Kbit **[DS]** | 64 Kbit **[DS]** | 1024 bit **[DS]** | selon µC **[H]** | limitée + mém. externe **[H]** |
| Endurance écriture | ~10⁶ (EEPROM) **[H]** | ~10⁶ **[H]** | par page **[DS]** | selon µC **[H]** | selon SE **[H]** |
| Rétention | longue (EEPROM) **[H]** | longue **[H]** | longue **[H]** | selon µC **[H]** | longue **[H]** |
| Provisioning production | grave EUI usine + data **[DS]** | tout provisionné | ROM ID usine + data **[DS]** | flash firmware + data | clés/certs sécurisés |
| Risque de corruption | mémoire (CRC requis) **[H]** | idem | idem + bus | logiciel + mémoire | mémoire (protégée) |
| Récupération après corruption | relecture / CRC **[BL]** | idem | idem | watchdog + relecture | relecture **[BL]** |
| Comportement lecture partielle | possible **[H]** | possible | possible | selon protocole | selon protocole |
| Périphérique absent | pas d'ACK **[H]** | pas d'ACK | pas de présence 1-Wire | pas de réponse | pas d'ACK |
| Réponse lente / bloqué | clock-stretch I²C **[H]** | idem | timing 1-Wire | dépend firmware | clock-stretch |
| Compat hot-plug | bonne (passif) **[H]** | bonne **[H]** | bonne **[H]** | à cadrer (boot) **[H]** | à cadrer **[H]** |
| Host-powered vs Module-powered | **Host-powered possible** **[H]** | Host-powered | Host/parasite | **Module-powered** (actif) **[H]** | Host-powered possible |
| Coût | faible **[H]** | faible **[H]** | moyen **[H]** | moyen **[H]** | plus élevé **[H]** |
| Disponibilité | bonne **[H]** | **très bonne** **[H]** | moyenne **[H]** | bonne **[H]** | moyenne **[H]** |
| Multi-fournisseur | moyen (EUI spécifique) **[H]** | **bon** **[H]** | faible (mono-source) **[H]** | bon **[H]** | faible-moyen **[H]** |
| Évolution vers auth crypto | **faible** (pas de crypto) **[H]** | faible **[H]** | partielle (variantes sécurisées) **[H]** | logicielle (non HW-sûre) **[H]** | **native (chemin privilégié)** **[H]** |
| Complexité firmware Host | faible **[H]** | faible **[H]** | moyenne (1-Wire) **[H]** | moyenne **[H]** | **élevée (pile crypto)** **[H]** |
| Complexité firmware Module | nulle (passif) **[H]** | nulle **[H]** | nulle **[H]** | **élevée** **[H]** | moyenne **[H]** |
| Testabilité en production | facile (relecture) **[H]** | facile **[H]** | moyenne **[H]** | à cadrer **[H]** | à cadrer **[H]** |

> **Secure Element** : porté **principalement comme chemin d'évolutivité vers
> l'authentification** (§ [évolutivité](authentication-evolutivity.md)). Il **ne
> doit pas** devenir implicitement une **nécessité V1** ; seul il **décrit mal**
> le module (d'où la « + mém. » de description).

## 3. Briques de stockage du Manifest (pas une identification complète)

Une **brique de stockage** (ex. **FRAM/Flash I²C ou SPI dédiée**) porte la
**description** mais **ne fournit ni présence ni identité** à elle seule. Elle est
comparée **en tant que brique**, jamais comme réponse aux trois couches.

| Critère | FRAM I²C (ex.) | Flash dédiée |
|---------|----------------|--------------|
| Rôle | stockage Manifest | stockage Manifest |
| Endurance écriture | **≥ 10¹²** **[DS]** (FRAM) | ~10⁴–10⁵ **[H]** |
| Écriture (vitesse / polling) | rapide, sans polling **[DS]** | par pages **[H]** |
| Fournit une identité ? | **Non** (N/A identité) | **Non** |
| Intérêt | découpler **Identité ↔ Manifest** | idem |

> Étudier la **séparation Identité ↔ Manifest** est un **intérêt architectural**
> (mises à jour du Manifest sans toucher à l'identité), **pas** une identité en soi.

## 4. Compositions hybrides (assemblages des briques)

Étudiées explicitement comme **compositions**, non comme technologies
supplémentaires :

- **EEPROM EUI + Manifest séparé** — identité native + description sur mémoire
  dédiée (FRAM/Flash), mises à jour découplées.
- **µC actif + mémoire externe** — logique d'ID active + grande description.
- **Secure Element + mémoire de description** — identité protégée + Manifest,
  **chemin d'évolutivité** vers l'authentification.

Chaque hybride hérite des critères de ses briques ; la matrice §2/§3 s'applique
par composant. **Aucune composition retenue.**

## 5. Exigence de résolution (neutre, non liée à I²C)

> Le mécanisme d'identification doit **garantir une résolution non ambiguë du
> module présent dans la topologie V1** et **définir son comportement en cas de
> conflit, de réponse multiple ou d'adresse non conforme.**

Déclinaisons par bus (à valider) : **I²C** → collisions/conflits d'adresse,
adresses réservées ; **1-Wire** → recherche/ROM ID, appareils multiples ;
**µC actif** → protocole d'énumération. Le critère est **fonctionnel**, pas
« 0 collision I²C ».

## Alimente

- **`DEC-L3-001`** — la sélection d'architecture se fera **après** comparaison,
  essais et arbitrage documenté ; **aucune architecture retenue ici**.

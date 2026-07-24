<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 3 — Format du Manifest (*Proposé*, `DEC-L3-002`)

> **Statut : Ouvert / Proposé.** Format **candidat**, **non figé**. Le Manifest
> relève de la couche **Description** (« que suis-je ? ») — **distincte** de
> l'identité. Valeurs `[H]/[BL]` ; **aucune `[M]`**. → **ADR future (n° attribué à
> la création)**.

## 1. Principe : un parser Host **borné**, jamais aveugle

> Le **parser Host** doit rester **borné** et **ne jamais allouer aveuglément**
> selon une taille annoncée par le module. Toute taille/longueur déclarée est
> **plafonnée** par une limite Host **avant** toute allocation ou lecture.

## 2. Éléments à définir (candidats, `[BL]` au baselining)

| Élément | Exigence candidate | Étiquette |
|---------|--------------------|-----------|
| **Champs obligatoires / optionnels** | jeu minimal obligatoire + extensions optionnelles | **[BL]** |
| **Longueur totale & limites** | taille max **plafonnée côté Host** | **[BL]** |
| **Endianness** | fixée et documentée (ex. little-endian) | **[BL]** |
| **Version du format** | champ de version en tête | **[H]** |
| **Compatibilité ascendante / descendante** | règles explicites par version | **[BL]** |
| **Champs inconnus** | **ignorables** (TLV : type-longueur-valeur) | **[H]** |
| **Capacités inconnues** | ignorées sans blocage ; journalisées | **[H]** |
| **CRC & périmètre couvert** | CRC + **plage exacte** qu'il protège | **[BL]** |
| **Valeur invalide / non provisionnée** | motif réservé « non provisionné » | **[BL]** |
| **Lecture partielle** | détectable (longueur + CRC) → rejet/dégradé | **[H]** |
| **Taille annoncée incohérente** | rejet borné (jamais d'allocation aveugle) | **[H]** |
| **Limites de ressources Host** | RAM/temps de parse plafonnés | **[BL]** |
| **Politique de rejet / dégradé** | définie (voir §4) | **[BL]** |

## 3. Structure candidate (illustrative, non figée)

Encodage **TLV** pressenti (extensibilité + champs inconnus ignorables) :

```
[ en-tête : magic | version_format | longueur_totale | CRC ]
[ TLV : identité produit (type, fabricant, modèle, révision) ]
[ TLV : capacités déclarées ... ]
[ TLV : réservé évolutivité (voir authentication-evolutivity) ]
```

- **magic** + **version_format** lus en premier ; **longueur_totale** **validée
  contre la limite Host** avant toute suite.
- Chaque TLV : **longueur bornée** ; TLV de **type inconnu** → **sauté** via sa
  longueur (jamais interprété).

## 4. Politique face à l'inconnu / l'invalide

| Situation | Politique candidate |
|-----------|---------------------|
| Version de format plus récente | lire les champs connus, **ignorer** le reste (si compat annoncée) ; sinon **dégradé** |
| CRC invalide | **rejet** (Manifest non fiable) ; module non décrit |
| Taille annoncée > limite Host | **rejet borné**, pas d'allocation |
| Lecture partielle | **rejet** ou **dégradé** documenté (jamais « deviné ») |
| Capacité inconnue | ignorée + journalisée ; n'empêche pas les capacités connues |

> Le mode **dégradé** (module présent mais Manifest non pleinement exploitable)
> est un comportement **explicite**, relié au [modèle de confiance](failure-and-ux.md).

## Alimente

- **`DEC-L3-002`** — format Manifest arrêté **après** comparaison + preuve d'un
  **parsing borné** ([protocole dédié](protocols/manifest-bounded-parsing.md)).
  **Aucun format figé ici.**

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

## 2bis. Règles de parsing déterministe & sûr (à figer)

Un parser réellement **déterministe et sûr** exige, en plus des bornes du §1 :

| Règle | Politique candidate | Étiquette |
|-------|---------------------|-----------|
| **TLV dupliqué** | rejet **ou** « premier gagne » **ou** « dernier gagne » — **une** règle fixée | **[BL]** |
| **Ordre des TLV** | **pas d'ordre imposé** (ou ordre canonique explicite) | **[BL]** |
| **Imbrication** | **interdite** (ou profondeur max bornée) | **[BL]** |
| **`offset + longueur`** | validés **sans débordement entier** (arithmétique bornée) | **[H]** |
| **Longueur TLV > octets restants** | **rejet** (jamais de lecture hors trame) | **[H]** |
| **Alignement / padding** | défini (ou aucun) ; padding **vérifié** | **[BL]** |
| **Nombre maximal de TLV** | plafonné (anti « milliers de petits TLV ») | **[BL]** |
| **Taille maximale d'un champ** | plafonnée par champ | **[BL]** |
| **Encodage des chaînes** | jeu + **terminaison/longueur** définis (pas de dépassement) | **[BL]** |
| **Unicité des champs obligatoires** | un obligatoire **répété** → rejet | **[H]** |
| **Capacités contradictoires** | politique définie (rejet / priorité / dégradé) | **[BL]** |
| **Versions compatibles** | **liste/plage exacte** de versions acceptées | **[BL]** |

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

## 5. Cohérence atomique (écriture / mise à jour interrompue)

Une écriture/mise à jour **interrompue** (coupure) ne doit pas laisser un Manifest
**ambigu**. **Stratégie candidate — aucune retenue** :

- **Manifest fabricant immuable** après provisioning (pas de MAJ sur le terrain) ;
- **double copie A/B** (bascule atomique) ;
- **compteur de génération** (la plus haute génération valide gagne) ;
- **marqueur de validité écrit en dernier** (Manifest « armé » seulement complet) ;
- **journal / mécanisme équivalent** ;
- **retour à la dernière copie valide**.

Précisions à figer :

| Question | À définir |
|----------|-----------|
| Parties **réellement modifiables** après production | zone fabricant (verrouillée) vs zone utilisateur | 
| La **zone utilisateur** appartient-elle au Manifest principal ? | oui / non (défini) |
| Le **CRC** couvre-t-il la zone fabricant seule ou aussi l'utilisateur ? | périmètre exact |
| **MAJ interrompue vs corruption permanente** | distinguées (marqueur/génération, cf. ci-dessus) |

> Ces scénarios sont **testés** par les protocoles
> [corruption/partiel](protocols/corruption-partial-read.md) et
> [provisioning](protocols/provisioning-duplicate-detection.md) — **sans** créer
> de septième protocole.

## Alimente

- **`DEC-L3-002`** — format Manifest arrêté **après** comparaison + preuve d'un
  **parsing borné** ([protocole dédié](protocols/manifest-bounded-parsing.md)) +
  **stratégie d'atomicité** validée. **Aucun format figé ici.**

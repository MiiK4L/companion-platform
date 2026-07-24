<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 3 — Identification des modules

> **Statut : en cours (exécution documentaire).** Ce lot **compare des
> architectures avant toute présélection**. **Aucune technologie privilégiée** ;
> l'**EEPROM I²C est une baseline expérimentale uniquement**. **Aucune ADR**,
> **rien en Accepté**, **aucun prototype exécuté**, **aucune mesure inventée**.
> Voir le [plan](../plan.md) (Lot 3) et le
> [cadre de validation](../validation-framework.md).

## Trois couches distinctes (à ne jamais confondre)

| Couche | Question | Statut V1 |
|--------|----------|-----------|
| **Identité** | *Qui suis-je ?* | décision de ce lot (`DEC-L3-001`) |
| **Description (Manifest)** | *Que suis-je ?* | décision de ce lot (`DEC-L3-002`) |
| **Authentification** | *Puis-je être considéré digne de confiance ?* | **hors V1** — exigence d'**évolutivité**, **aucune décision** |

> **CRC ≠ authenticité** ; **identification ≠ authentification**. Aucune DEC
> d'authentification n'est ouverte : elle le sera **seulement** si une vraie
> décision se présente (signature du Manifest, Secure Element, certificats,
> challenge-réponse).

## Quatre informations distinctes (à séparer explicitement)

1. **Présence physique** — un module est inséré.
2. **Identité de produit** — type, fabricant, modèle, révision.
3. **Identité d'instance** — identifiant unique de **cet exemplaire**.
4. **Description des capacités** — le **Manifest**.

Toutes les architectures ne fournissent pas ces quatre informations : la
[matrice](identification-architectures.md) montre lesquelles sont **natives**,
**provisionnées** ou **dérivées**.

## Décisions ouvertes (*Ouvert / Proposé*, aucune ADR)

- **`DEC-L3-001`** — **architecture d'identification** → ADR-0016.
- **`DEC-L3-002`** — **format et contenu du Manifest** → **ADR future, n°
  attribué à la création** (non réservé).

Voir le [registre des décisions](../decisions-register.md).

## Terminologie & conventions

- **Série représentative documentaire** : voir la définition unique en
  [Lot 2B](../lot-2b/README.md) (série réelle citée pour établir des plages,
  **jamais retenue**, aucun MPN final).
- **Statut des valeurs** : `[DS]` datasheet · `[C]` calculé · `[H]` hypothèse ·
  `[BL]` à finaliser au baselining · `N/A` · `inconnu` · `[M]` mesuré (**aucune
  `[M]`**).

## Livrables (tous *Proposé*)

| Livrable | Contenu |
|----------|---------|
| [Architectures d'identification](identification-architectures.md) | Complètes vs briques de stockage vs hybrides ; matrice sur critères communs |
| [Format du Manifest](manifest-format.md) | Format candidat borné, versionné, extensible (`DEC-L3-002`) |
| [Évolutivité — authentification](authentication-evolutivity.md) | Chemin crypto **sans l'imposer** en V1 ; **aucune décision** |
| [Provisioning production](provisioning.md) | Chaîne de confiance opérationnelle |
| [Pannes & UX](failure-and-ux.md) | Corruption, périphérique défaillant, identité partielle ; expérience utilisateur |
| [Faible conso & hot-plug](low-power-hotplug-compat.md) | Lecture sans alimenter le reste du module ; résolution non ambiguë ; hot-plug (2A/2B) |
| [Composants candidats](candidate-components.md) | Séries représentatives documentaires sourcées |
| [Protocoles](protocols/README.md) | Six protocoles **génériques** distincts (Brouillon, `[BL]`) |

## Ce que ce lot NE fait pas

- Ne **retient** aucune technologie, ne **fige** aucun format, ne crée **aucune ADR**.
- **Sélectionne** les options à prototyper **sans les arbitrer** ; les
  **prototypes et résultats** relèvent d'une **PR/campagne ultérieure**.
- **Aucune option ne passe à `Accepté`** sur la seule comparaison documentaire.

## Critères de sortie

- Quatre informations (présence / produit / instance / Manifest) **distinguées**.
- Architectures **complètes** comparées ; **briques de stockage** comparées
  séparément ; **compositions hybrides** étudiées ; **aucun gagnant**.
- Format Manifest candidat **borné** (parser Host jamais aveugle).
- Provisioning en **chaîne de confiance** documenté.
- Pannes/UX et compat faible-conso/hot-plug **cadrées**.
- Six protocoles génériques **distincts** en Brouillon ; `DEC-L3-001`/`DEC-L3-002`
  **ouverts** ; options à prototyper **listées, non arbitrées**.

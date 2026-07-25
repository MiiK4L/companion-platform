<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L4-PWR — Consommation & rétroéclairage

> **Statut : Brouillon (en attente de baselining).** Essai **matériel**.
> [Conventions](README.md) · [définitions](event-definitions.md). Seuils
> `[P]/[BL]` ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 4 · alimente **`DEC-L4-001`** (conso écran par famille/niveau ; ↔ L6).

## Objectif (une question mesurable)

Quelle est la **consommation de l'écran** selon le **niveau de rétroéclairage**
(familles rétroéclairées) ou le **contenu** (OLED/réflectif), et l'**extinction
auto + reprise** fonctionne-t-elle proprement ?

## Cas mesurés

- **Rétroéclairage par niveau** (PWM) : courbe conso ↔ luminosité (familles TFT).
- **Contenu** : conso OLED selon taux de pixels allumés ; Memory LCD statique/dyn.
- **Extinction auto** : passage en veille après délai ; conso résiduelle.
- **Reprise** : réveil + ré-affichage (latence perçue → [perçu](perceived-performance.md)).

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Conso par niveau de backlight | courbe mesurée | `[BL]` (compatibilité cible L6) |
| Conso résiduelle en veille | ≤ `[BL]` | **[P]/[BL]** |
| Reprise après extinction | 100 % (ré-affichage cohérent) | **[P]** |

### Champs à finaliser au baselining (`[BL]`)

- **Niveaux** de luminosité testés ; **cible de conso** (dérivée L6) ; délai
  d'extinction ; définition de « veille ».

## Plan d'essai (type **matériel**)

- **`n_dut`** ≥ 2 panneaux · niveaux de backlight / contenus figés ·
  **`n_campaigns`** ≥ 2 · alimentation mesurée (sonde de courant).

## Critères d'arrêt immédiat

Échauffement anormal du panneau/pilote de backlight.

## Données brutes attendues

Courbes conso ↔ niveau/contenu, conso veille, journal reprise ; SHA-256 au rapport.

## Sécurité opérateur

Limitation de courant ; ESD standard.

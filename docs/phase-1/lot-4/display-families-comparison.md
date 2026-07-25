<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 4 — Familles d'affichage (*Proposé*, `DEC-L4-001`)

> **Statut : Ouvert / Proposé.** Comparaison **par familles de technologie de
> dalle**, **au même niveau**, **sans présélection ni gagnant**. **Interface
> physique** et **format** sont des **axes/critères distincts** (pas des
> familles). Valeurs `[DS]/[H]/[BL]/inconnu` ; **aucune `[M]`**. Séries
> représentatives documentaires **sourcées** ([composants](candidate-components.md)).

## Axes distincts (ne pas confondre)

- **Famille de dalle** (technologie) : TFT couleur · **Memory LCD** (réflectif
  mono) · **OLED couleur** · **transflectif** · (autres pertinentes).
- **Interface physique** (axe séparé) : **SPI** · **QSPI** · **RGB parallèle** · …
- **Format** (critère) : **rond / carré / rectangulaire**, taille, résolution.
- **Contrôleur** : ST7789 / ILI9341 / GC9A01 / SSD1351 / … (**un contrôleur n'est
  pas une famille** ; GC9A01 = contrôleur TFT SPI rond).

## Familles comparées

| Famille | Principe | Rétroéclairage | Représentatifs (voir [composants](candidate-components.md)) |
|---------|----------|----------------|-------------------------------------------------------------|
| **TFT couleur (transmissif)** | LCD couleur rétroéclairé | **requis** | ST7789, ILI9341, GC9A01 (rond) |
| **Memory LCD (réflectif mono)** | Memory-in-pixel réflectif | **aucun** (lumière ambiante) | Sharp LS013B7DH03, LS027B7DH01 |
| **OLED couleur** | Émissif | **aucun** (émissif) | SSD1351 (+ 2ᵉ série à sourcer) |
| **Transflectif** | LCD lisible en réflectif **et** transmissif | **appoint** (faible) | dalles TFT/Memory transflectives — **séries à sourcer** |

> **Transflectif inclus explicitement** (lisibilité soleil + faible conso), **même
> s'il n'est pas retenu** : compromis couleur/consommation/lisibilité à documenter.

## Grille comparative (critères communs)

`banc` = à mesurer (protocoles).

| Critère | TFT couleur | Memory LCD | OLED couleur | Transflectif |
|---------|-------------|------------|--------------|--------------|
| Couleur / profondeur | oui (16/18 bit) **[DS]** | **mono** **[DS]** | oui (262K) **[DS]** | selon dalle (souvent limitée) **[H]** |
| Interface(s) physiques | SPI (± parallèle/RGB) **[DS]** | SPI **[DS]** | SPI/parallèle **[DS]** | SPI/parallèle **[H]** |
| Format(s) disponibles | rond/carré/rect **[DS]** | rect (mono) **[DS]** | petit rect/carré **[DS]** | à sourcer |
| Rétroéclairage / conso écran | backlight **dominant** `banc` | **aucun**, ~µW statique **[DS]** | émissif (dépend du contenu) `banc` | appoint faible `banc` |
| Lisibilité plein soleil | faible-moyenne **[H]** | **excellente** (réflectif) **[DS]** | moyenne (émissif) **[H]** | **bonne** **[H]** |
| Contrainte mémoire Host imposée | framebuffer couleur (voir [budget](memory-and-fps-budget.md)) | faible (mono) | GDDRAM embarquée (SSD1351) **[DS]** | selon dalle |
| Rafraîchissement / animation | bon `banc` | **lent** (Memory LCD) **[H]** | rapide `banc` | modéré `banc` |
| Disponibilité / secondes sources | multi-fabricant (contrôleurs) **[H]** | **≈ mono-fabricant** (Sharp) **[DS]** | plusieurs **[H]** | à évaluer |
| Intégration poche | compact **[H]** | compact **[H]** | compact **[H]** | selon dalle |

*(Aucune conclusion : les cellules `banc` s'enrichissent des mesures.)*

## Synthèse par famille (avantages / limites / risques)

- **TFT couleur** — **+** couleur, formats variés, contrôleurs multi-sources ;
  **−** **rétroéclairage = poste de conso dominant** (R4) ; **risque** autonomie.
- **Memory LCD** — **+** conso quasi nulle, lisibilité soleil ; **−** **mono**,
  rafraîchissement lent ; **risque** UX animée limitée.
- **OLED couleur** — **+** contraste, rapidité, émissif ; **−** conso liée au
  contenu, burn-in, tailles limitées ; **risque** autonomie/longévité.
- **Transflectif** — **+** compromis lisibilité/conso ; **−** couleur souvent
  limitée, **sourcing plus rare** ; **risque** disponibilité.

## Alimente

- **`DEC-L4-001`** — dalle + contrôleur + **interface physique** + format +
  **contraintes mémoire matérielles** ; arbitrage **après** mesures. **Aucune
  famille retenue.**

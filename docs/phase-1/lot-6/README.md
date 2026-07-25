<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 6 — Énergie : alimentation, stockage & estimation d'état

> **Statut : en cours (exécution documentaire).** Conçoit **sur le papier** et
> **cadre la mesure** de la chaîne d'énergie, et pose la **méthode** d'un budget
> énergétique mesuré (cible d'autonomie **provisoire, révisable**). **Aucune techno
> privilégiée**, **aucune ADR**, **rien en Accepté**, **aucune mesure inventée**,
> **aucun prototype exécuté**. Risque central : **R4** (autonomie). Voir le
> [plan](../plan.md) (Lot 6) et le [cadre de validation](../validation-framework.md).

## Trois décisions séparées (*Ouvert / Proposé*)

- **`DEC-L6-001` — architecture d'alimentation du Host** : **charge, power-path,
  régulation, protections, domaines d'alimentation** → **ADR-0019**. *(La
  **régulation** est une **sous-fonction** de cette architecture, pas une décision
  séparée.)*
- **`DEC-L6-002` — architectures de stockage d'énergie + budget & autonomie** : la
  **chimie** (LiPo, Li-Ion, LiFePO4…) est une **propriété** des solutions comparées,
  **pas** une famille de premier niveau → **ADR-0020**.
- **`DEC-L6-003` — stratégie d'estimation de l'état de batterie** (ADC seul, jauge
  dédiée, hybride, autre) → **ADR future non réservée**. *(Titre large : ne
  présuppose pas de jauge dédiée.)*

## Frontière avec les décisions déjà ouvertes (pas de duplication)

- **Power-gating module / rails exposés / `VBAT`** = **`DEC-L2-003`** (ouvert en
  L2B). Le Lot 6 traite la chaîne **Host** et **renvoie** à `DEC-L2-003` pour
  l'alimentation du slot — **sans la rouvrir ni la dupliquer**.
- **Deep-sleep** = mesure **R3** ([L5](../lot-5/deep-sleep-measurement.md)), **entrée**
  du budget. **Conso écran** ([L4](../lot-4/power-and-backlight.md)) et **contraintes
  L1** = entrées du budget.

## Méthode : raisonner **par fonctions d'architecture**

On compare **les architectures d'une fonction**, puis les **solutions** à
l'intérieur ; **les composants illustrent une solution, ne définissent pas la
fonction**.

| Fonction | Options d'architecture | Décision |
|----------|------------------------|----------|
| Charge / power-path | simple · power-path · PMIC intégré | `DEC-L6-001` |
| Régulation (sous-fonction) | LDO · buck · buck-boost · multi-rails | `DEC-L6-001` |
| Estimation d'état | ADC · jauge dédiée · hybride | `DEC-L6-003` |
| Stockage d'énergie | mono/multi-cellule · protection intégrée · format ; **chimie = propriété** | `DEC-L6-002` |

## Conventions

- **Série représentative documentaire** : voir [Lot 2B](../lot-2b/README.md) (série
  réelle **illustrative**, **jamais retenue**, aucun MPN final) — **≥ 2/fonction**.
- **Statut des valeurs** : `[DS]` · `[C]` · `[H]` · `[BL]` · `N/A` · `inconnu` ·
  `[M]` (**aucune `[M]`**). **Les seuils (précision, fuite, ΔT…) restent `[BL]`**,
  définis au **baselining**, **pas** figés dans ce plan documentaire.

## Livrables (tous *Proposé*)

| Livrable | Décision | Contenu |
|----------|----------|---------|
| [Architecture d'alimentation](power-chain-architecture.md) | L6-001 | Vue par fonctions : USB-C, charge/power-path, régulation, protections, domaines |
| [Charge / power-path](charge-powerpath-comparison.md) | L6-001 | Architectures de charge (solutions illustratives) |
| [Régulation](regulation-comparison.md) | L6-001 | Architectures de régulation (sous-fonction) |
| [Estimation d'état](battery-state-estimation.md) | L6-003 | ADC / jauge / hybride |
| [Stockage d'énergie](energy-storage-comparison.md) | L6-002 | Architectures de stockage ; chimie = propriété |
| [Méthode budget énergie](energy-budget-method.md) | L6-002 | Budget mesuré + cible d'autonomie provisoire (entrées L1/L4/L5) |
| [Composants candidats](candidate-components.md) | tous | Séries sourcées **illustratives** (≥ 2/fonction) |
| [Protocoles](protocols/README.md) | tous | Typés (Brouillon, seuils `[BL]`) : conso/mode, précision d'estimation, fuite, charge/ΔT, décharge réelle |

## Ce que ce lot NE fait pas

- Ne **retient** aucune techno, ne **fige** aucun seuil (tout `[BL]`), ne crée
  **aucune ADR**.
- **Sélectionne** des options à prototyper **sans les arbitrer** ; mesures en
  **PR/campagne ultérieure**. **Aucun `Accepté`** sur documentaire seul.

## Critères de sortie

- **3 décisions ouvertes** ; fonctions d'architecture comparées, **aucun gagnant**.
- Frontière `DEC-L2-003` **respectée** (pas de duplication du power-gating module).
- **Méthode** du budget énergie posée (entrées L1/L4/L5) ; cible **provisoire**.
- Protocoles typés en Brouillon, **seuils `[BL]`** ; options à prototyper **listées,
  non arbitrées**.

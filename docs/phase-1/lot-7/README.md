<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 7 — Base de temps & persistance

> **Statut : en cours (exécution documentaire).** Valide **comment** obtenir une
> **base de temps fiable** + **Δt au réveil** et un **modèle de persistance**
> robuste (**principe acté**, [ADR-0009](../../adr/0009-rtc-externe-et-persistance-etat.md) ;
> l'usage d'un **RTC externe non**). **Aucune techno privilégiée**, **aucune ADR**,
> **rien en Accepté**, **aucune mesure inventée**, **aucun prototype exécuté**.
> Seuils **`[BL]`**. Voir le [plan](../plan.md) (Lot 7) et le
> [cadre de validation](../validation-framework.md).

## Deux décisions séparées (*Ouvert / Proposé*)

- **`DEC-L7-001` — base de temps** : source de temps (RTC interne/externe/TCXO),
  **validité de l'heure**, sauvegarde de l'horloge, précision, **sources de
  réveil** (comme critère), **stratégie de resynchronisation** → **ADR-0021**.
- **`DEC-L7-002` — modèle de persistance** : **support** (NVS/LittleFS…) **et**
  **modèle de cohérence** (immédiate/différée/journal/transaction/A-B/checkpoint),
  reprise après coupure, stockage heure+état, politique d'écriture → **ADR future
  non réservée**.

## Distinctions structurantes (deux axes par décision)

- **`DEC-L7-001`** : **source de temps ≠ validité ≠ provenance.** Choisir un RTC
  **ne garantit pas** une heure **fiable après une perte d'alimentation**. Trois
  choses distinctes : la **source** (RTC interne/externe/TCXO) ; l'**état de
  validité** (**heure valide / heure inconnue** — deux valeurs) ; la **provenance /
  niveau de confiance** (RTC conservé, resync USB, source externe, utilisateur…).
  **« resynchronisée » est un ÉVÉNEMENT**, pas un état (voir
  [base de temps](timebase-comparison.md)).
- **`DEC-L7-002`** : **support ≠ modèle de cohérence ≠ politique d'écriture.** Le
  **support** (NVS/LittleFS, *où*), le **modèle de cohérence** (journal/transaction/
  A-B/checkpoint/checksum, *comment*) et la **politique d'écriture** (immédiate/
  différée/à l'événement/périodique/au repos, *quand*) sont **trois axes distincts**,
  comparés **séparément** (voir [persistance](persistence-comparison.md)).

## Frontières avec les décisions déjà ouvertes

- **Budget GPIO / IRQ de réveil** = **`DEC-L1-001`** (entrée, non rouvert).
- **Conso RTC** = entrée du **budget énergie** ([L6](../lot-6/energy-budget-method.md)).
- **Abstraction horloge / stockage** = **L8** (ports/adaptateurs) : ce lot
  **spécifie les capacités**, il **ne conçoit pas** l'abstraction.

## Conventions

- **Série représentative documentaire** : voir [Lot 2B](../lot-2b/README.md) (série
  réelle **illustrative**, **jamais retenue**, aucun MPN final) — **≥ 2/fonction**.
- **Statut des valeurs** : `[DS]` · `[C]` · `[H]` · `[BL]` · `N/A` · `inconnu` ·
  `[M]` (**aucune `[M]`**). **Les seuils (dérive, conso, Δt…) restent `[BL]`**.

## Livrables (tous *Proposé*)

| Livrable | Décision | Contenu |
|----------|----------|---------|
| [Base de temps](timebase-comparison.md) | L7-001 | Source (interne/externe/TCXO), **validité** (valide/inconnue), **provenance** ; réveil = critère |
| [Stratégie de resynchronisation](resync-strategy.md) | L7-001 | **déclencheurs** × **politique** (deux dimensions) |
| [Persistance](persistence-comparison.md) | L7-002 | **support** × **cohérence** × **politique d'écriture** (trois axes) |
| [Composants candidats](candidate-components.md) | L7-001 | Séries sourcées illustratives (PCF8563 / RV-3028-C7 / DS3231 / interne) |
| [Protocoles](protocols/README.md) | tous | Typés (Brouillon, `[BL]`) : dérive, conso RTC, Δt/cycles, reprise sur coupures |

## Ce que ce lot NE fait pas

- Ne **retient** aucune techno, ne **fige** aucun seuil (`[BL]`), ne crée **aucune ADR**.
- **Sélectionne** des options à prototyper **sans les arbitrer** ; mesures en
  **PR/campagne ultérieure**. **Aucun `Accepté`** sur documentaire seul.

## Critères de sortie

- **2 décisions ouvertes** ; distinctions explicites : **source ≠ validité ≠
  provenance** · **déclencheurs ≠ politique** (resync) · **support ≠ cohérence ≠
  politique d'écriture**.
- Base de temps, resynchronisation et persistance comparées ; **aucun gagnant**.
- Frontières L1/L6/L8 respectées ; protocoles typés en Brouillon (`[BL]`).

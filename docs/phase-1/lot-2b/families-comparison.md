<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2B — Comparaison des familles de connecteurs (*Proposé*, `DEC-L2-002`)

> **Statut : Ouvert / Proposé.** Comparaison **fonctionnelle**, **au même
> niveau**, **sans présélection ni gagnant**, **sans MPN final**. Pour être
> **crédible**, elle s'appuie sur des **séries représentatives documentaires**
> (≥ 2 par famille si disponibles) : elles servent à **établir des plages** et à
> vérifier que la famille possède des solutions crédibles — **elles ne sont pas
> retenues**. Le **MPN final** sera choisi **après** sélection de famille et
> essais. **Aucune [M].**

## Statut des valeurs & échelles

Chaque cellule porte un **statut** : **[DS]** (plage issue de séries
représentatives, source citée) · **[C]** calculé · **[H]** hypothèse · **[BL]** à
fixer au baselining · **N/A** non applicable · **inconnu** (non documenté à ce
stade). Aucune appréciation « fait » sans source ni définition.

Échelles qualitatives (employées **uniquement** faute de plage chiffrée) :

- **Coût** (relatif, à sourcer) : `faible` < `moyen` < `élevé`.
- **Aptitude / disponibilité / tolérance** : `faible` < `moyenne` < `bonne`.

> « Aptitude au hot-plug », « disponibilité », « coût » etc. sont des **[H]**
> qualitatives explicitées, **non** des faits datasheet.

## Séries représentatives documentaires (NON retenues)

> Sources **consultées le 2026-07-23** (voir la section « Sources datasheet » en
> fin de page). Champs non documentés = **inconnu** (jamais inventés).

### (F1) Board-to-board / mezzanine
| Série (fab.) | Pas · contacts | Cycles annoncés | Courant nom. · derating | R_contact init/max | Force ins./rétention | Temp. | Détrompage / séquencement | Source |
|--------------|----------------|-----------------|-------------------------|--------------------|----------------------|-------|---------------------------|--------|
| **Hirose DF40** | 0,4 mm · jusqu'à 100 | ≈ 30 (à confirmer) **[DS]** | 0,3 A/contact **[DS]** · derating inconnu | inconnu | click/auto-align 0,33 mm **[DS]** | inconnu | guidage/click ; séquencement **N/A** (contacts égaux) | DF40 cat. |
| **Molex SlimStack (0,4 mm)** | 0,4 mm · série | inconnu (à relever) | à relever | inconnu | à relever | à relever | guidage ; séquencement **N/A** | Molex |

### (F2) Card-edge
| Série (fab.) | Pas · contacts | Cycles annoncés | Courant nom. · derating | R_contact init/max | Force ins./rétention | Temp. | Détrompage / séquencement | Source |
|--------------|----------------|-----------------|-------------------------|--------------------|----------------------|-------|---------------------------|--------|
| **EDAC 342** | selon variante | à relever | à relever | inconnu | à relever | à relever | encoche possible ; **séquencement possible** (doigts échelonnés) **[H]** | EDAC 342 |
| **Smiths Eclipta (edge card)** | selon variante | à relever | 0,5 A tous contacts / 2 A un seul **[DS]** | inconnu | à relever | à relever | keying à concevoir | Smiths |

### (F3) FPC / FFC
| Série (fab.) | Pas · contacts | Cycles annoncés | Courant nom. · derating | R_contact init/max | Force ins./rétention | Temp. | Détrompage / séquencement | Source |
|--------------|----------------|-----------------|-------------------------|--------------------|----------------------|-------|---------------------------|--------|
| **Hirose FH12** | 0,5 / 1,0 mm · jusqu'à 50+ | ≥ 30 min **[DS]** | à relever | ≤ 20 mΩ **[DS]** | ZIF (levier) ; rétention faible **[H]** | −25…+85 °C **[DS]** | détrompage selon série ; séquencement **N/A** | FH12 cat. |
| **Amphenol F33D** | 0,5 mm · série | à relever | 0,4 A **[DS]** | inconnu | ZIF | à relever | selon série ; séquencement **N/A** | F33D DS |

### (F4) Spring / pogo-pin
| Série (fab.) | Pas · contacts | Cycles annoncés | Courant nom. · derating | R_contact init/max | Force ins./rétention | Temp. | Détrompage / séquencement | Source |
|--------------|----------------|-----------------|-------------------------|--------------------|----------------------|-------|---------------------------|--------|
| **Mill-Max 815/817** (ultra-low profile) | série · 1 ou 2 rangs | **100 000–1 000 000** **[DS]** | 2 A cont. / 3 A max **[DS]** | inconnu | compliance ressort ; rétention **à concevoir** **[H]** | à relever | à concevoir ; **séquencement possible** (longueurs de pins) **[H]** | Mill-Max |
| **Mill-Max 836/838** (pas 2 mm) | 2 mm · 1 ou 2 rangs | 100 000+ **[DS]** | à relever | inconnu | ressort | à relever | à concevoir ; séquencement possible **[H]** | Mill-Max |

### (F5) Filaire verrouillable (**témoin** — voir « Statut du témoin filaire » plus bas)
| Série (fab.) | Pas · contacts | Cycles annoncés | Courant nom. · derating | R_contact init/max | Force ins./rétention | Temp. | Détrompage / séquencement | Source |
|--------------|----------------|-----------------|-------------------------|--------------------|----------------------|-------|---------------------------|--------|
| **JST GH** | 1,25 mm · 2–15 | ≈ 30 min **[DS]** | 1 A/circuit **[DS]** | inconnu | **verrouillage** natif **[DS]** | à relever | polarisé + verrou ; séquencement **N/A** (simultané) | JST GH |
| **JST PH** | 2,0 mm · 2–16 | ≈ 30 min **[DS]** | 2 A **[DS]** | inconnu | ergots latéraux **[DS]** | à relever | polarisé ; séquencement **N/A** | JST PH |

## Grille comparative (plages documentées ou `inconnu`)

| Critère | (F1) Mezzanine | (F2) Card-edge | (F3) FPC/FFC | (F4) Pogo | (F5) Filaire (témoin) |
|---------|----------------|----------------|--------------|-----------|------------------------|
| Coût estimatif | moyen **[H]** | faible **[H]** | faible-moyen **[H]** | moyen-élevé **[H]** | faible-moyen **[H]** |
| Encombrement | faible **[H]** | fin **[H]** | très fin **[H]** | variable **[H]** | volumineux **[H]** |
| Courant/contact | 0,3 A **[DS]** | ≈ 0,5 A (tous) **[DS]** | 0,4–1 A **[DS]** | 2–3 A **[DS]** | 1–2 A **[DS]** |
| Cycles annoncés | ≈ 30 **[DS]** | inconnu | ≥ 30 **[DS]** | 10⁵–10⁶ **[DS]** | ≈ 30 **[DS]** |
| R_contact (fiche) | inconnu | inconnu | ≤ 20 mΩ **[DS]** | inconnu | inconnu |
| Séquencement des contacts | N/A (égaux) | possible **[H]** | N/A | possible **[H]** | N/A |
| Aptitude hot-plug | faible **[H]** | bonne **[H]** | faible **[H]** | bonne **[H]** | faible (manuel) **[H]** |
| Tolérance désalignement | faible **[H]** | moyenne **[H]** | faible **[H]** | bonne **[H]** | bonne **[H]** |
| Détrompage | oui **[DS]** | à concevoir **[BL]** | selon série **[H]** | à concevoir **[BL]** | oui (polarisé) **[DS]** |
| Rétention/verrouillage | click **[DS]** | à concevoir **[BL]** | levier ZIF **[H]** | à concevoir **[BL]** | verrou natif **[DS]** |
| Disponibilité multi-fournisseur | bonne **[H]** | bonne **[H]** | bonne **[H]** | moyenne **[H]** | bonne **[H]** |
| Impact mécanique produit | empilage **[H]** | fente boîtier **[H]** | logement nappe **[H]** | surface+guidage **[H]** | logement câble **[H]** |
| Complexité de fabrication | faible **[H]** | faible **[H]** | moyenne **[H]** | moyenne-élevée **[H]** | moyenne **[H]** |

*(Endurance, résistance de contact et forces **réelles** sont `[M]` à produire par
les [protocoles](protocols/README.md) ; les plages `[DS]` ci-dessus ne les
préjugent pas.)*

## Synthèse par famille (avantages / limites / risques)

- **(F1) Mezzanine** — **+** densité, faible hauteur, auto-alignement/click ;
  **−** **cycles annoncés faibles (≈ 30)**, peu adaptée au rem-plug, séquencement N/A ;
  **risque** : usure prématurée en rem-plug fréquent.
- **(F2) Card-edge** — **+** coût, hot-plug « cartouche », séquencement possible ;
  **−** cycles non documentés (à relever), détrompage/rétention à concevoir ;
  **risque** : usure du bord PCB, insertion en sens inverse.
- **(F3) FPC/FFC** — **+** très fin, R_contact documentée (≤ 20 mΩ) ; **−** **cycles
  faibles (≥ 30)**, **inadapté au rem-plug utilisateur**, fragilité nappe ;
  **risque** : rupture de nappe en usage nomade.
- **(F4) Pogo** — **+** **cycles très élevés (10⁵–10⁶)**, courant élevé, compliance ;
  **−** coût, **rétention à concevoir**, dispo moyenne ; **risque** : perte de force
  de contact, contamination des pointes.
- **(F5) Filaire (témoin)** — **+** courant, **verrouillage/détrompage natifs** ;
  **−** encombrant, non « cartouche », ergonomie de rem-plug médiocre ; **risque** :
  contrainte sur fils, **incompatible format poche** (voir statut ci-dessous).

## Statut du témoin filaire

Le connecteur **filaire verrouillable** est inclus **uniquement comme baseline
fonctionnelle** : il **borne** les valeurs atteignables (courant, verrouillage,
rétention) et sert de point de comparaison. **Il n'est pas éligible** à la
sélection `DEC-L2-002` **tant qu'il ne satisfait pas l'ergonomie « cartouche »
poche** ([exigences](connector-requirements.md)). **Un bon courant ou une bonne
rétention ne peuvent pas, à eux seuls, lui faire « gagner » la décision.** S'il
devait devenir réellement éligible, ce changement de statut serait **explicitement
tracé**.

## Sources datasheet

> Séries **représentatives documentaires** (non retenues), consultées le
> **2026-07-23** ; valeurs `[DS]` issues de ces documents, `inconnu` sinon.

| Série | Fabricant | Document / série | Date consultée | Référence stable |
|-------|-----------|------------------|----------------|------------------|
| DF40 | Hirose | *DF40 0.4 mm B2B/FPC Connectors* (catalogue) | 2026-07-23 | `hirose.com/product/en/products/DF40` |
| SlimStack | Molex | famille SlimStack 0,4 mm | 2026-07-23 | `molex.com` (SlimStack) |
| 342 | EDAC | *Card Edge Connectors — 342 series* | 2026-07-23 | `edac.net/series/342` |
| Eclipta | Smiths Interconnect | *Edge Card* (0,5 A tous / 2 A un contact) | 2026-07-23 | `smithsinterconnect.com` (edge-card) |
| FH12 | Hirose | *FH12 0.5/1.0 mm FPC/FFC* (cat., ≥30 cycles, R≤20 mΩ) | 2026-07-23 | `farnell.com/datasheets/2043906.pdf` |
| F33D | Amphenol | *FFC/FPC 0.50 mm — F33D* (0,4 A) | 2026-07-23 | `amphenol-cs.com` (F33D) |
| 815/817 · 836/838 | Mill-Max | *Spring-Loaded (Pogo) Connectors* (10⁵–10⁶, 2–3 A) | 2026-07-23 | `mill-max.com` (spring-loaded) |
| GH | JST | *GH connector* (1,25 mm, 1 A, verrou) | 2026-07-23 | `jst-mfg.com/product/pdf/eng/eGH.pdf` |
| PH | JST | *PH connector* (2,0 mm, 2 A) | 2026-07-23 | `jst.com` (PH) |

## Alimente

- **`DEC-L2-002`** — la sélection de famille se fera **après** mesures (endurance,
  résistance de contact, force) et arbitrage ; **aucune famille retenue ici**, le
  **témoin filaire** restant **hors éligibilité** en l'état.

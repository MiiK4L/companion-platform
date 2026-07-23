<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2A — Protocoles d'essai (sûreté électrique)

> **Statut : Brouillon (rédigés, en attente de revue de baselining).** Ces
> protocoles **ne sont pas encore figés** : plusieurs seuils de verdict restent
> **à finaliser** (étiquette **`[BL]`**). Ils **spécifient** les campagnes ;
> **aucune n'est exécutée** dans cette PR et **aucune valeur mesurée `[M]`** n'y
> figure. Les seuils sont proposés `[P]` / datasheet `[DS]` / calcul `[C]` /
> hypothèse `[H]` / **à finaliser au baselining `[BL]`** (voir
> [convention](../README.md)).

## Cycle de vie d'un protocole

```
Brouillon → Revu → Baseline d'essai → Exécuté → Rapporté
```

| État | Signification |
|------|---------------|
| **Brouillon** | Rédigé ; des champs obligatoires ou seuils `[BL]` restent ouverts. |
| **Revu** | Relu ; revue de baselining planifiée. |
| **Baseline d'essai** | **Tous** les champs obligatoires et seuils de verdict **finalisés et gelés** (date + version du protocole). **Prérequis absolu au premier essai.** |
| **Exécuté** | Campagne réalisée selon la baseline gelée. |
| **Rapporté** | Rapport de mesure produit et tracé. |

> **Règle de gel** : une fois l'état **Baseline d'essai** atteint, toute
> modification d'un seuil est un **écart au protocole tracé** (motif, date,
> auteur) et **ne s'applique pas rétroactivement** aux campagnes déjà exécutées.
> **Aucune campagne ne peut commencer tant qu'un `[BL]` subsiste.**

### Fiche de baseline d'essai (à remplir au passage en *Baseline d'essai*)

Pour qu'une campagne ne soit **jamais** rattachée à une version ambiguë du
protocole, le gel produit une **fiche de baseline** consignée avec le protocole :

| Champ | Contenu |
|-------|---------|
| **Commit Git du protocole baseliné** | SHA exact du protocole au moment du gel |
| **Date** | Date du gel |
| **Réviseur / approbateur** | Qui a revu et approuvé la baseline |
| **Identifiant de campagne autorisée** | Réf. de la campagne couverte par cette baseline |
| **Liste des `[BL]` levés** | Chaque `[BL]` finalisé + valeur retenue + source/justification |
| **Hash firmware de test** | SHA-256 du binaire (si applicable) |
| **Hash / réf. de la fixture** | Empreinte ou référence documentaire du montage/fixture |

> Un protocole reste en **Brouillon** tant que cette fiche n'est pas complète et
> qu'un `[BL]` subsiste.

## Contenu imposé de chaque protocole

Conformément au [modèle de protocole](../../templates/test-protocol-template.md),
chaque fiche précise **avant essai** :

- conditions initiales ;
- instrumentation requise **et schéma des masses** (voir point ci-dessous) ;
- montage / fixture ;
- **matrice des conditions d'alimentation** (topologie × source × rail observé ×
  coins applicables) — les coins batterie ne s'appliquent **que** si le chemin
  étudié en dépend ;
- **plan d'échantillonnage** : `n_dut` / `n_runs` / `n_campaigns` / `n_cycles`,
  **total**, répartition par scénario / DUT / campagne, ordre ou randomisation,
  repos éventuel, **règle de reprise après échec** ;
- seuils de verdict **instrumentés** (voir
  [définitions des événements](event-definitions.md)) ;
- critères d'**arrêt immédiat** ;
- procédure de **remise en état** entre deux campagnes ;
- éléments **susceptibles d'être détruits** ;
- **données brutes attendues** et leur corrélation aux traces ;
- **décision `DEC-*` alimentée**.

Les événements comptés comme erreurs (**reset**, **corruption écran**,
**blocage bus**, **latch-up**, **réveil parasite**, **état connu**) sont définis
de façon **instrumentée** dans un référentiel commun :
[définitions des événements](event-definitions.md).

## Matrice de traçabilité — protocole → rapport futur → décision

> **Aucun rapport concret n'est versionné** (pas de gabarit vide). Le rapport
> sera créé **au démarrage de la campagne** correspondante, à partir du
> [modèle générique](../../templates/measurement-report-template.md), sous
> l'identifiant `RAPP-*` indiqué.

| Protocole | Identifiant | État | Rapport futur (`RAPP-*`) | `DEC-*` | Risque(s) |
|-----------|-------------|------|--------------------------|---------|-----------|
| [Inrush & rampe](inrush-et-rampe.md) | PROTO-L2A-INRUSH | Brouillon | RAPP-L2A-INRUSH | `DEC-L2-001` | R5 |
| [Court-circuit](court-circuit.md) | PROTO-L2A-SHORT | Brouillon | RAPP-L2A-SHORT | `DEC-L2-001` | R5 |
| [Bus-stuck](bus-stuck.md) | PROTO-L2A-STUCK | Brouillon | RAPP-L2A-STUCK | `DEC-L2-001` | collisions I²C |
| [Hot-plug (injection)](hot-plug-injection.md) | PROTO-L2A-HOTPLUG | Brouillon | RAPP-L2A-HOTPLUG | `DEC-L2-001` | R5, R2, back-powering |
| [Retrait en transaction](retrait-en-transaction.md) | PROTO-L2A-REMOVE | Brouillon | RAPP-L2A-REMOVE | `DEC-L2-001` | R5 |

> **Tous les protocoles sont en état Brouillon** : leur passage en **Baseline
> d'essai** (gel) est un prérequis à toute campagne (voir cycle de vie ci-dessus).

## Note (frontière Lot 2B)

Aucun protocole d'**endurance mécanique de connecteur** ni de **résistance de
contact** ne figure ici : ils relèvent du **Lot 2B**. Le hot-plug est étudié via
un **banc d'injection** indépendant du connecteur final.

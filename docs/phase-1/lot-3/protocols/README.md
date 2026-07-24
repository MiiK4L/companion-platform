<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 3 — Protocoles d'essai (identification)

> **Statut : Brouillon (rédigés, en attente de baselining).** Protocoles
> **génériques**, **un objectif par protocole** (pas de protocole fourre-tout).
> **Aucune campagne exécutée**, **aucune `[M]`**. Seuils `[P]/[BL]`.

## Cette PR sélectionne, elle n'arbitre pas

> Cette PR **documentaire** aboutit à une **liste d'options à prototyper**
> (≥ 2 options crédibles, cf. plan), **sans les arbitrer**. Les **prototypes et
> résultats** relèvent d'une **PR / campagne ultérieure**. **Aucune option ne
> passe à `Accepté`** sur la seule comparaison documentaire.

## Cycle de vie (identique aux lots précédents)

```
Brouillon → Revu → Baseline d'essai → Exécuté → Rapporté
```

Passage en **Baseline d'essai** = tous les `[BL]` levés + **fiche de baseline**
(commit du protocole, date, approbateur, campagne autorisée, `[BL]` levés, hash
firmware/fixture) — même modèle qu'en
[Lot 2A](../../lot-2/protocols/README.md) / [Lot 2B](../../lot-2b/protocols/README.md).
Gel : toute modif post-baseline = **écart tracé, non rétroactif** ; **aucune
campagne tant qu'un `[BL]` subsiste**.

## Conventions communes

- **DUT** : un **module d'essai** (ou carte porteuse) équipé de l'architecture
  d'identification testée, identifié de façon stable ; côté **Host de test**
  (firmware + commit exact).
- **Alimentation** : lecture d'ID **sans alimenter le reste du module** quand
  l'architecture le permet (cf. [faible conso & hot-plug](../low-power-hotplug-compat.md)).
- **Verdicts instrumentés** : voir [définitions des événements](event-definitions.md).
- **Données brutes** : journaux + captures bus, horodatés ; SHA-256 au rapport.

## Type d'essai (le plan d'échantillonnage s'y adapte)

Le modèle `n_dut` **ne s'applique pas partout**. Chaque protocole précise **son
type** :

| Type | Grandeurs de couverture |
|------|-------------------------|
| **Matériel** (détection, absent/lent/bloqué, hot-plug) | `n_dut`, séries et **révisions physiques**, `n_campaigns` |
| **Logiciel** (parsing borné) | **corpus versionné**, **seeds** de fuzzing, durée bornée, **versions du parser**, sanitizers |
| **Provisioning** | **nombre de pièces**, **registre de production** (simulé/réel), répétitions |
| **Panne bus** | **fixture** + architecture physique testée |

## Six protocoles distincts (un objectif chacun)

| Protocole | Objectif | `DEC-*` |
|-----------|----------|---------|
| [Détection & lecture d'identité](identity-detection-read.md) | présence + identité, résolution non ambiguë | `DEC-L3-001` |
| [Parsing borné du Manifest](manifest-bounded-parsing.md) | parser Host jamais aveugle | `DEC-L3-002` |
| [Corruption & lecture partielle](corruption-partial-read.md) | CRC, partiel, taille incohérente → rejet/dégradé | `DEC-L3-001/002` |
| [Provisioning & détection des doublons](provisioning-duplicate-detection.md) | chaîne de confiance + anti-doublon | `DEC-L3-001` |
| [Périphérique absent / lent / bloqué](peripheral-absent-slow-stuck.md) | timeouts bornés, états d'échec | `DEC-L3-001` |
| [Hot-plug pendant la lecture](hotplug-during-read.md) | interruption propre, pas de blocage | `DEC-L3-001` |

> Le [modèle générique de rapport](../../templates/measurement-report-template.md)
> sert de base ; **aucun rapport vide versionné**.

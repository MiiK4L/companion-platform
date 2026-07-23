<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2A — Sûreté électrique du CX-Bus

> **Statut : en cours (exécution documentaire — aucune mesure physique).** Ce lot
> **n'engage aucune mesure** (oscilloscope, alimentation de labo, analyseur
> logique) : il **cadre**, **analyse** et **spécifie les essais**. Toutes les
> décisions restent **Ouvert / Proposé**, **aucune ADR n'est créée** et **aucune
> valeur n'est présentée comme mesurée**. Les campagnes sur banc et l'éventuel
> passage Proposé→Accepté relèvent d'une mise à jour ultérieure (ou d'une PR de
> résultats dédiée). Voir le [plan](../plan.md) (Lot 2) et le
> [cadre de validation](../validation-framework.md).

## Découpage Lot 2A / Lot 2B

Le Lot 2 du plan est scindé pour garder des revues ciblées :

- **Lot 2A (ce document)** — **sûreté électrique** du bus : architecture et
  chemins de puissance, isolation/commutation, back-powering, comportement
  **électrique** du hot-plug, bus-stuck, retrait en transaction, ESD
  préliminaire, risques électriques, composants candidats, protocoles chiffrés.
- **Lot 2B (PR ultérieure)** — **sélection et endurance du connecteur** :
  comparaison de familles, brochage, rails exposés (dont **`VBAT`**),
  contraintes mécaniques, endurance (cycles), résistance de contact. **Non
  traité ici.**

> ⚠️ **Le Lot 2 n'est pas terminé à l'issue de cette PR** : seul le volet
> sûreté (2A) l'est, au niveau documentaire.

## Frontière stricte avec le connecteur (Lot 2B)

Cette PR **ne contient pas** : comparaison de familles de connecteurs, choix de
connecteur, protocole d'endurance mécanique, ni seuil de résistance de contact
propre à une famille. Le comportement **électrique** du hot-plug est étudié avec
un **banc d'injection contrôlé** (interrupteur / relais / fixture équivalente)
**indépendant du connecteur final**. L'**ordre des contacts** est traité en
**scénarios** et **exigences fonctionnelles**, **sans être figé** avant le
Lot 2B.

## Portée ESD (préliminaire)

Sans connecteur final ni PCB intégré, l'analyse ESD reste **préliminaire** :
interfaces potentiellement exposées, chemins de décharge, principes de placement,
familles de protections, contraintes de capacité / tension / courant de fuite.
Le **dimensionnement final** et le **choix définitif** des protections **restent
ouverts** jusqu'à connaissance du connecteur, du routage et de la mécanique.
**Aucune affirmation de conformité normative** (préconformité/certification hors
périmètre Phase 1).

## Convention : statut d'une valeur numérique

Toute valeur chiffrée de ce lot est **étiquetée par sa nature et son statut**.
**Aucune valeur n'est « mesurée » sans donnée de banc.**

| Étiquette | Signification | Peut arbitrer une décision ? |
|-----------|---------------|------------------------------|
| **[DS]** | Valeur de **datasheet** (source citée) | Non — borne/entrée d'analyse |
| **[C]** | Valeur **calculée** (formule + hypothèses explicites) | Non — à confirmer |
| **[H]** | **Hypothèse de conception** | Non — à valider |
| **[P]** | **Seuil proposé** (critère d'acceptation à vérifier) | Non tant que non mesuré |
| **[M]** | Valeur **réellement mesurée** sur banc | Oui (si reproductible) — **aucune [M] à ce stade** |

## Livrables de ce lot (tous *Proposé*)

| Livrable | Contenu |
|----------|---------|
| [Architecture & chemins de puissance](power-architecture.md) | Rails, distribution vers le slot, question `VBAT` (ouverte, tranchée en 2B) |
| [Isolation & commutation](isolation-and-switching.md) | Options au même niveau (load switch, isolation I²C, haute-Z) — sans présélection |
| [Back-powering](back-powering.md) | Chemins de ré-alimentation parasite ; entrée de la décision `VBAT`/connecteur (2B) |
| [Hot-plug (électrique)](hot-plug.md) | Banc d'injection ; scénarios d'ordre des contacts (non figés) |
| [Bus-stuck & retrait en transaction](bus-stuck-and-transaction.md) | Détection/récupération ; comportement au retrait |
| [Chemins ESD (préliminaire)](esd-paths.md) | Interfaces exposées, chemins, familles de protections (non dimensionnées) |
| [Analyse des risques électriques](electrical-risk-analysis.md) | R2, R5, back-powering, collisions I²C (renvoi au [registre des risques](../risk-register.md)) |
| [Composants candidats (par fonction)](candidate-components.md) | Familles au même niveau + sourcing |
| [Protocoles d'essai](protocols/README.md) | 5 protocoles chiffrés + matrice protocole → rapport futur → `DEC-*` |

## Question(s) de décision ouverte(s)

- **`DEC-L2-001`** — sûreté électrique du CX-Bus (principe d'isolation/commutation,
  protections, séquencement ; seuils inrush / court-circuit / bus-stuck /
  hot-plug électrique / retrait en transaction). → ADR-0014 (différée).

Voir le [registre des décisions](../decisions-register.md).
Le connecteur et l'exposition de `VBAT` ouvriront **`DEC-L2-002`** (→ ADR-0015)
**en Lot 2B**.

## Ce que ce lot NE fait pas

- Aucune **mesure** ni valeur mesurée : les protocoles sont **spécifiés**, pas exécutés.
- Aucun **rapport de mesure** concret n'est versionné (voir la
  [matrice de traçabilité](protocols/README.md) ; le
  [modèle générique](../templates/measurement-report-template.md) sert de base).
- Aucun **gel** (topologie, séquence d'alimentation, `VBAT`, connecteur) ni **ADR**.
- Aucun élément **connecteur** (→ Lot 2B).

## Critères de sortie du lot

- Chemins de puissance, options d'isolation/commutation et back-powering
  **analysés et comparés** (sans présélection).
- Comportements hot-plug / bus-stuck / retrait **cadrés** (scénarios + exigences).
- ESD **préliminaire** posée, limites explicitées.
- **5 protocoles** entièrement renseignés (conditions, instrumentation, limites,
  arrêts, remise en état, données brutes attendues, `DEC-*` alimentée).
- `DEC-L2-001` **ouvert et tracé** ; aucune décision arbitrée, aucune ADR.

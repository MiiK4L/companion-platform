<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Spécification consolidée de Phase 1 (draft)

> **⚠️ Avertissement — statut : draft consolidé, NON figé.**
>
> - **Aucune `DEC-*` n'est arbitrée** : les contraintes issues des lots sont
>   **ouvertes** et **provisoires**.
> - **Aucune ADR 0013+ n'est créée ni acceptée** (règle « ADR *après* mesures
>   reproductibles », [cadre §3](../validation-framework.md)).
> - **Ce document n'est PAS `spec-v1.0.0-rc.1`.** La promotion vers `rc.1` est un
>   **résultat de la sortie de Phase 1** (après les campagnes de mesure du volet
>   expérimental), pas de ce checkpoint documentaire.
> - Les **interfaces** (brochage, signatures de ports) sont **candidates et
>   révisables**.
>
> Ce document **rassemble** l'acquis stable (architecture actée) et les
> contraintes encore ouvertes, pour offrir une **vue d'ensemble** — sans rien
> geler ni décider.

## 1. Objet & portée

La spécification consolidée agrège, en un seul point d'entrée, **ce qui est acté**
(décisions d'architecture des ADR 0000-0012) et **ce qui reste à trancher** (les
17 `DEC-*` de la Phase 1). Elle sert de **référence de lecture** avant les
**campagnes de mesure** (volet expérimental de la Phase 1) ; elle ne se substitue
ni aux ADR, ni aux lots, vers lesquels elle **renvoie**.

## 2. Acquis architectural acté (ADR 0000-0012)

Ces décisions sont **Acceptées** (principe) et forment le socle stable. Le détail
et le statut fin (principe *Accepté* vs éléments *Proposé*) restent dans chaque ADR.

| ADR | Décision (socle) |
|-----|------------------|
| [0000](../../adr/0000-adopter-les-adr.md) | Adoption du processus ADR |
| [0001](../../adr/0001-framework-firmware-esp-idf.md) | Framework firmware : ESP-IDF ; silicium ESP32-S3 |
| [0002](../../adr/0002-modele-execution-applications.md) | Modèle d'exécution des applications (runtime hybride extensible) |
| [0003](../../adr/0003-technologie-et-abstraction-ecran.md) | Abstraction de l'écran (technologie derrière une interface) |
| [0004](../../adr/0004-coeur-de-calcul-socket.md) | Cœur de calcul **remplaçable** (socket) |
| [0005](../../adr/0005-standard-cx-bus-et-identification.md) | Standard **CX-Bus** & principe d'identification des modules |
| [0006](../../adr/0006-nommage-cx-bus.md) | Nommage CX-Bus |
| [0007](../../adr/0007-hal-et-companion-sdk.md) | HAL & **Companion SDK** (ports/adaptateurs ; façade) |
| [0008](../../adr/0008-architecture-alimentation.md) | Architecture d'alimentation (principe power-gating + USB-C) |
| [0009](../../adr/0009-rtc-externe-et-persistance-etat.md) | Base de temps + persistance d'état (principe) |
| [0010](../../adr/0010-strategie-licences.md) | Stratégie de licences (Apache-2.0 / CERN-OHL-S-2.0 / CC-BY-4.0) |
| [0011](../../adr/0011-docs-as-code.md) | Docs-as-code (MkDocs strict) |
| [0012](../../adr/0012-monorepo-et-versioning.md) | Monorepo & versioning (SemVer `<domaine>-vX.Y.Z`) |

## 3. Contraintes ouvertes (provisoires — issues des `DEC-*`)

Ces contraintes **découlent** des lots mais **ne sont pas arbitrées**. Elles sont
listées ici pour **cadrage**, avec renvoi ; leur état de résolution est dans la
[synthèse](synthesis.md) et le [registre des décisions](../decisions-register.md).

| Domaine | Contrainte ouverte (provisoire) | `DEC-*` |
|---------|--------------------------------|---------|
| Cœur & E/S | Budget GPIO/bus ; topologie SPI (partagé vs séparé) ; choix du module cœur | `DEC-L1-001`, `DEC-L1-002` |
| Sûreté CX-Bus | Isolation/commutation, protections, séquencement | `DEC-L2-001` |
| Connecteur | Famille, mécanique, endurance, mapping physique candidat | `DEC-L2-002` |
| Rails exposés | Rails effectivement exposés (dont `VBAT`), stratégie de puissance | `DEC-L2-003` |
| Identification | Architecture de découverte/identité **logique** ; format Manifest | `DEC-L3-001`, `DEC-L3-002` |
| Affichage | Matériel (dalle/contrôleur/interface/format) ; moteur graphique | `DEC-L4-001`, `DEC-L4-002` |
| Socketage | Support du cœur remplaçable (mécanique + électrique) | `DEC-L5-001` |
| Énergie | Alimentation ; stockage (chimie = propriété) ; estimation d'état ; autonomie `[BL]` | `DEC-L6-001/002/003` |
| Temps & état | Base de temps (source/validité/resync) ; persistance (support × cohérence × écriture) | `DEC-L7-001`, `DEC-L7-002` |
| Logiciel | Runtime scripté V1 ; modèle de distribution des applications | `DEC-L8-001`, `DEC-L8-002` |

## 4. Interfaces candidates (révisables)

> Les interfaces ci-dessous sont **candidates** : elles servent le cadrage et la
> preuve d'architecture, **pas un gel**.

- **Brochage CX-Bus** — distinguer l'**allocation logique** (contrainte SPEC
  **révisable**) du **mapping physique candidat** (relève de `DEC-L2-002`) : voir
  [brochage L2B](../lot-2b/pinout.md). Les **rails exposés** (dont `VBAT`) relèvent
  de `DEC-L2-003` ([rails exposés](../lot-2b/exposed-rails-and-power.md)).
- **Ports logiciels (L8)** — interfaces C **candidates** du squelette host
  (`IClock`, `IStorage`, `IBus`, `IDisplay`, `IInput`, `IPower`, `ILog`,
  `IAppSource`, `IRuntime`) : voir [conception des ports](../lot-8/ports-design.md).
  Signatures **non figées** ; un changement n'impacte que les **adaptateurs**.
- **Modèles opaques** — `AppReference` et `AppArtifactView` (octets opaques,
  empruntés) : voir [règles d'injection](../lot-8/dependency-injection-rules.md).

## 5. Exigences (renvoi)

La [matrice des exigences](../requirements-matrix.md) tient la traçabilité
**exigence ↔ lot ↔ `DEC-*` ↔ protocole ↔ risque**. Les cibles chiffrées y sont
marquées « à confirmer » ; aucune valeur n'est décrétée dans la présente
spécification.

## 6. Critères d'acceptation (comment cette spec deviendra rc.1)

Le passage de **draft consolidé** à **`spec-v1.0.0-rc.1`** est un **résultat de la
sortie de Phase 1** (après les campagnes de mesure du volet expérimental) et suppose :

1. l'**exécution** des protocoles ([feuille de route](decision-roadmap.md)),
   produisant des **résultats reproductibles** ;
2. la **création des ADR 0013+** sur preuves ([plan des ADR](../adr-plan.md)),
   selon les 4 conditions du [cadre §3](../validation-framework.md) ;
3. la **résolution** des couplages transverses (SPI partagé, budget énergie) ;
4. la satisfaction des [critères de sortie de la Phase 1](../execution.md).

**`spec-v1.0.0` (sans `rc`)** reste **au-delà de la Phase 1** : il exigera une
carte Host **intégrée**, **plusieurs types de modules** et une **campagne
d'interopérabilité**.

## Renvois

- [Vue d'ensemble du Lot 9](README.md) · [Synthèse Phase 1](synthesis.md)
- [Feuille de route des décisions](decision-roadmap.md) · [Préparation des campagnes de mesure](campaign-readiness.md)
- [Registre des décisions](../decisions-register.md) · [Plan des ADR](../adr-plan.md) · [Registre des ADR](../../adr/README.md)

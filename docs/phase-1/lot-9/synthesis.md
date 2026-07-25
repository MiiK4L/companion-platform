<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Synthèse de la Phase 1 (L1 → L8)

> **Statut : Proposé — synthèse documentaire, sans décision ni ADR.** Ce document
> **agrège** et **renvoie** vers les lots L1 à L8 ; il n'en **recopie pas** le
> contenu (anti-duplication et anti-divergence). Il **étend** à L1–L8 la
> [revue d'architecture intermédiaire L1–L4](../architecture-review-l1-l4.md).
> **Les 17 `DEC-*` sont Ouvertes ; aucune ADR 0013+ n'existe.**

## 1. Carte des lots (renvois)

| Lot | Objet | Point d'entrée |
|-----|-------|----------------|
| L1 | Exigences & architecture du cœur (budget GPIO/bus, comparaison des cœurs) | [lot-1](../lot-1/README.md) |
| L2A | Sûreté électrique du CX-Bus | [lot-2](../lot-2/README.md) |
| L2B | Connecteur & endurance | [lot-2b](../lot-2b/README.md) |
| L3 | Identification **logique** des modules | [lot-3](../lot-3/README.md) |
| L4 | Écran & moteur graphique | [lot-4](../lot-4/README.md) |
| L5 | Cœur remplaçable & socketage | [lot-5](../lot-5/README.md) |
| L6 | Énergie (alimentation, stockage, estimation d'état) | [lot-6](../lot-6/README.md) |
| L7 | Base de temps & persistance | [lot-7](../lot-7/README.md) |
| L8 | Ports/adaptateurs & runtime scripté (preuve d'architecture) | [lot-8](../lot-8/README.md) |

## 2. État consolidé des 17 `DEC-*`

Toutes **Ouvertes / Proposé**. Détail, options et validation dans le
[registre des décisions](../decisions-register.md) ; conditions d'acceptation des
ADR dans le [plan des ADR](../adr-plan.md). L'**ordre** des arbitrages est traité
par la [feuille de route](decision-roadmap.md).

| `DEC-*` | Objet (résumé) | ADR prévue | Lot de validation |
|---------|----------------|-----------|-------------------|
| [DEC-L1-001](../decisions-register.md) | Allocation GPIO/bus de référence (+ expander / partage de bus) | 0013 | L2 (bring-up) |
| [DEC-L1-002](../decisions-register.md) | Architecture de cœur (A XIAO / B autre module / C custom) | 0018 ou dédiée ; peut rouvrir [ADR-0004](../../adr/0004-coeur-de-calcul-socket.md) | L4 + L5 |
| [DEC-L2-001](../decisions-register.md) | Sûreté électrique CX-Bus (isolation/commutation, protections, séquencement) | 0014 | L2 |
| [DEC-L2-002](../decisions-register.md) | Connecteur : famille, mécanique, endurance, mapping physique candidat | 0015 | L2 |
| [DEC-L2-003](../decisions-register.md) | Rails exposés (dont `VBAT`) + stratégie de puissance des modules | future (n° à la création) | L2 |
| [DEC-L3-001](../decisions-register.md) | Architecture de découverte & d'identification **logique** | 0016 | L3 |
| [DEC-L3-002](../decisions-register.md) | Format et contenu du **Manifest** | future (n° à la création) | L3 |
| [DEC-L4-001](../decisions-register.md) | Matériel d'affichage (dalle, contrôleur, interface physique, format) | 0017 | L4 |
| [DEC-L4-002](../decisions-register.md) | Moteur graphique (rendu, stratégie mémoire/buffers, pipeline) | future (n° à la création) | L4 |
| [DEC-L5-001](../decisions-register.md) | Stratégie de socketage/support du cœur remplaçable | 0018 | L5 |
| [DEC-L6-001](../decisions-register.md) | Architecture d'alimentation du Host (charge/power-path/régulation/protections) | 0019 | L6 |
| [DEC-L6-002](../decisions-register.md) | Architectures de stockage d'énergie (chimie = propriété) + autonomie | 0020 | L6 |
| [DEC-L6-003](../decisions-register.md) | Stratégie d'estimation d'état de batterie (ADC / jauge / hybride) | future (n° à la création) | L6 |
| [DEC-L7-001](../decisions-register.md) | Base de temps (source, validité de l'heure, resync) | 0021 | L7 |
| [DEC-L7-002](../decisions-register.md) | Persistance (support × cohérence × politique d'écriture) | future (n° à la création) | L7 |
| [DEC-L8-001](../decisions-register.md) | Runtime scripté V1 (Lua / WASM-WAMR) | 0022 | L8 (POC) |
| [DEC-L8-002](../decisions-register.md) | Distribution des applications (install dynamique vs bundle OTA) | future (n° à la création) | L8 (faisabilité) |

## 3. Couplages transverses (dépendances à surveiller, pas des contradictions)

Ces couplages **prolongent** les trois déjà identifiés en [revue L1–L4](../architecture-review-l1-l4.md)
et les étendent à L5–L8. Aucun n'est tranché ici.

1. **Topologie SPI partagé (R2)** — fil rouge : budget ([DEC-L1-001](../lot-1/gpio-bus-budget.md)),
   contention électrique ([DEC-L2-001](../lot-2/electrical-risk-analysis.md)),
   mapping physique ([DEC-L2-002](../lot-2b/pinout.md)), interface d'affichage
   ([DEC-L4-001](../lot-4/spi-sharing.md)), trafic/buffers ([DEC-L4-002](../lot-4/graphics-engine-comparison.md)).
   **Aucune autorité unique** : arbitrage au banc L2/L4.
2. **Rails exposés & `VBAT` ↔ énergie** — [DEC-L2-003](../lot-2b/exposed-rails-and-power.md)
   conditionne le **power-gating module** repris (non dupliqué) par
   [DEC-L6-001](../lot-6/power-chain-architecture.md), et le **domaine
   d'alimentation de l'identification** ([DEC-L3-001](../lot-3/low-power-hotplug-compat.md),
   lire l'ID sans alimenter le module).
3. **Budget énergétique = point de convergence** — le
   [budget énergie L6](../lot-6/energy-budget-method.md) **agrège** des entrées
   d'autres lots : deep-sleep ([mesure R3, L5](../lot-5/deep-sleep-measurement.md)),
   conso écran/rétroéclairage ([L4](../lot-4/power-and-backlight.md)), conso RTC
   ([L7](../lot-7/protocols/rtc-consumption.md)). La cible d'autonomie reste `[BL]`.
4. **Choix du cœur = décision tardive et structurante** —
   [DEC-L1-002](../lot-1/core-architecture-comparison.md) dépend des mesures
   **RAM/écran (L4)** et **deep-sleep/socket (L5)** ; son arbitrage peut
   **rouvrir [ADR-0004](../../adr/0004-coeur-de-calcul-socket.md)** (cf.
   [déclencheurs d'arbitrage](../requirements-matrix.md) et
   [risque R3](../risk-register.md)).
5. **Abstractions logicielles ↔ décisions matérielles** — le
   [squelette L8](../lot-8/architecture-skeleton.md) expose des **ports**
   (`IClock`, `IStorage`, `IDisplay`, `IPower`, `IBus`, `IRuntime`…) qui
   **absorbent** les choix matériels ouverts : base de temps ([DEC-L7-001](../lot-7/timebase-comparison.md)),
   persistance ([DEC-L7-002](../lot-7/persistence-comparison.md)), affichage
   ([DEC-L4-001/002](../lot-4/README.md)), énergie ([DEC-L6-*](../lot-6/README.md)).
   Les décisions matérielles **n'impactent que les adaptateurs**.
6. **Identification ↔ sûreté & connecteur** — [DEC-L3-001](../lot-3/low-power-hotplug-compat.md)
   réutilise le [banc hot-plug L2A](../lot-2/hot-plug.md), l'[analyse
   back-powering](../lot-2/back-powering.md) et les [contacts/brochage L2B](../lot-2b/pinout.md).

## 4. Graphe des dépendances entre décisions

Ordre logique de résolution (renvoi détaillé : [feuille de route](decision-roadmap.md)).
Le chemin critique documenté est **L1 → L2 → L6 → L9** ([execution](../execution.md)).

```text
DEC-L1-001 (GPIO/bus) ── préalable ──┬── DEC-L2-001 (sûreté) ── DEC-L3-001/002 (identification)
                                     ├── DEC-L2-002 (connecteur) ─ DEC-L2-003 (rails/VBAT)
                                     ├── DEC-L4-001/002 (écran/moteur)
                                     ├── DEC-L5-001 (socket)
                                     ├── DEC-L7-001/002 (temps/persistance)
                                     └── DEC-L8-001/002 (runtime/distribution)
                                                   │
   DEC-L4-001 (RAM écran) ┐                        ▼
   DEC-L5-001 (deep-sleep) ┼── alimentent ── DEC-L6-001/002/003 (énergie) ── convergence budget
                           └── alimentent ── DEC-L1-002 (choix du cœur, tardif)
```

## 5. Verdict de synthèse

- **Cohérence d'ensemble : bonne.** Les 17 `DEC-*` sont **distinctes, tracées** et
  renvoient à un protocole. Aucune contradiction ; les couplages sont **explicites**.
- **Vigilances transverses** : le **SPI partagé (R2)** et le **budget énergétique**
  (convergence L4/L5/L6/L7) restent les points les plus sensibles.
- **Rien n'est figé** : aucun composant retenu, aucune ADR, aucune mesure. La
  suite est **gouvernée** par la [feuille de route](decision-roadmap.md) et la
  [préparation à la Phase 2](phase-2-readiness.md).

## Renvois

- [Registre des décisions](../decisions-register.md) · [Plan des ADR](../adr-plan.md)
- [Registre des risques](../risk-register.md) · [Matrice des exigences](../requirements-matrix.md)
- [Feuille de route des décisions](decision-roadmap.md) · [Préparation à la Phase 2](phase-2-readiness.md)
- [Spécification consolidée (draft)](consolidated-specification.md)

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Feuille de route des décisions (campagnes de mesure de Phase 1)

> **Statut : Proposé — planification des arbitrages.** Pour chaque `DEC-*`, ce
> document indique les **prérequis**, le(s) **protocole(s)** qui la tranchera(ont),
> les **preuves attendues** et l'**ordre recommandé** des arbitrages lors des
> **campagnes de mesure de la Phase 1** (volet expérimental, encore à conduire).
> **Il ne tranche rien** : il ordonne le travail de mesure à venir. Les seuils
> chiffrés restent `[BL]` (fixés au baselining, en tête de campagne).

## Principe d'arbitrage (rappel)

Une `DEC-*` ne devient une **ADR Acceptée** qu'après un **résultat reproductible**
(`n_campaigns ≥ 2`) **et** les quatre conditions du
[cadre de validation §3](../validation-framework.md) : conformité aux exigences,
comparaison des alternatives, justification des compromis, traçabilité vers les
preuves. **Atteindre les seuils ne suffit pas.**

## Ordre recommandé (vagues)

L'ordre suit le chemin critique **L1 → L2 → L6 → L9** et les dépendances de
l'[ordre d'exécution](../execution.md). Les vagues 2 sont **parallélisables** une
fois L1 clos et un banc d'alimentation de base disponible.

| Vague | Décisions | Raison de l'ordre |
|-------|-----------|-------------------|
| **0 — Préalable** | `DEC-L1-001` | Le budget GPIO/bus **conditionne** tous les autres lots (topologie SPI, IRQ, enable). |
| **1 — HW critique & prérequis L3** | `DEC-L2-001`, `DEC-L2-002`, `DEC-L2-003` | La sûreté électrique et le connecteur **précèdent** l'identification (lecture sûre) et l'exposition des rails. |
| **2 — Sous-systèmes (parallèles)** | `DEC-L3-001/002`, `DEC-L4-001/002`, `DEC-L5-001`, `DEC-L7-001/002`, `DEC-L8-001/002` | Indépendants entre eux une fois L1/L2 disponibles ; L3 démarre après L2 (isolation/rail d'ID). |
| **3 — Convergence énergie** | `DEC-L6-001`, `DEC-L6-002`, `DEC-L6-003` | Le budget énergie **agrège** deep-sleep (L5), conso écran (L4) et conso RTC (L7). |
| **4 — Décision tardive & structurante** | `DEC-L1-002` | Le choix du cœur dépend de RAM/écran (L4) et deep-sleep/socket (L5) ; peut **rouvrir [ADR-0004](../../adr/0004-coeur-de-calcul-socket.md)**. |

### Chemin critique & parallélisme

**Chemin critique** (la plus longue chaîne de dépendances, à sécuriser en
priorité) : **L1 → L2 → L6 → arbitrage du cœur (`DEC-L1-002`)**. Tout retard sur
ce chemin retarde la clôture des campagnes.

```text
L1 (préalable — DEC-L1-001)
 ├──► L2 ─┬──► L3            (identification : après isolation/rail d'ID de L2)
 │        └──► (contention SPI partagée ↔ L4)
 ├──► L4 ┐
 ├──► L5 ┼──► L6  (convergence énergie) ──► DEC-L1-002  (arbitrage du cœur)
 ├──► L7 ┘        agrège deep-sleep (L5), conso écran (L4), conso RTC (L7)
 └──► L8          (preuve host déjà verte ; POC cible ensuite)
```

**Exécutables en parallèle** (une fois **L1** clos et un banc d'alimentation de
base disponible) : **L4, L5, L7, L8** — indépendants entre eux. **L3** démarre
**après L2**. **L6** est un **point de convergence** : il ne se conclut qu'une
fois disponibles les entrées de L4, L5 et L7. **`DEC-L1-002`** (choix du cœur) est
**la dernière** : elle consomme les résultats de L4, L5 et la cible d'autonomie L6.

> **Nota** : `DEC-L8-001/002` (logiciel) se **parallélisent sans contrainte
> matérielle** pour la partie host ; seuls leurs volets **cible** (RAM/flash
> embarqué, install dynamique) attendent une toolchain et des POC.

## Feuille de route par décision

Chaque ligne : **prérequis** → **protocole(s)** → **preuves attendues** (type de
résultat + grandeurs, seuils `[BL]`).

### Vague 0 — Préalable

| `DEC-*` | Prérequis | Protocole(s) | Preuves attendues |
|---------|-----------|--------------|-------------------|
| `DEC-L1-001` | Besoins plateforme et **budget GPIO documentaire de référence** disponibles, hypothèses **identifiées et révisables** ; candidat de cœur de référence disponible | Banc de **bring-up GPIO/bus** (protocole à instancier depuis le [modèle](../templates/test-protocol-template.md)) | Décompte GPIO **sans conflit** sur le candidat ; topologie SPI (partagé vs séparé) démontrée ; option expander/partage **comparée** en coût global |

### Vague 1 — HW critique & prérequis L3

| `DEC-*` | Prérequis | Protocole(s) | Preuves attendues |
|---------|-----------|--------------|-------------------|
| `DEC-L2-001` | Banc d'injection + alim à limitation + oscillo ; [analyse électrique](../lot-2/electrical-risk-analysis.md) | [inrush/rampe](../lot-2/protocols/inrush-et-rampe.md), [court-circuit](../lot-2/protocols/court-circuit.md), [bus-stuck](../lot-2/protocols/bus-stuck.md), [hot-plug](../lot-2/protocols/hot-plug-injection.md), [retrait en transaction](../lot-2/protocols/retrait-en-transaction.md) | Sûreté **reproductible** : inrush borné, court-circuit non destructif, bus-stuck récupéré, retrait sans corruption ; options isolation/commutation **comparées** |
| `DEC-L2-002` | Fixtures par famille ; DUT = paire accouplée ; [comparaison familles](../lot-2b/families-comparison.md) | [endurance](../lot-2b/protocols/endurance-insertion.md), [R_contact 4 fils](../lot-2b/protocols/contact-resistance.md), [intermittence](../lot-2b/protocols/intermittence.md), [force](../lot-2b/protocols/insertion-extraction-force.md), [inspection](../lot-2b/protocols/mechanical-inspection.md) | Endurance (`n_cycles`) + dérive R_contact (R0/Rn/ΔR) + force dans les bornes `[BL]` ; **familles comparées** |
| `DEC-L2-003` | [analyse rails exposés](../lot-2b/exposed-rails-and-power.md) ; banc back-powering 2A | [hot-plug](../lot-2/protocols/hot-plug-injection.md), [court-circuit](../lot-2/protocols/court-circuit.md) (entrée back-powering) | Sûreté d'exposition `VBAT` démontrée (back-powering 2A) ; options **P1/P2/P3 comparées** |

### Vague 2 — Sous-systèmes (parallélisables)

| `DEC-*` | Prérequis | Protocole(s) | Preuves attendues |
|---------|-----------|--------------|-------------------|
| `DEC-L3-001` | L2 (isolation + rail d'ID) ; banc HW+CODE ; [architectures d'ID](../lot-3/identification-architectures.md) | [détection/lecture d'identité](../lot-3/protocols/identity-detection-read.md), [corruption/lecture partielle](../lot-3/protocols/corruption-partial-read.md), [hot-plug pendant lecture](../lot-3/protocols/hotplug-during-read.md), [périphérique absent/lent/bloqué](../lot-3/protocols/peripheral-absent-slow-stuck.md), [provisioning/doublons](../lot-3/protocols/provisioning-duplicate-detection.md) | Lecture **sûre** de l'identité prouvée ; résolution des pannes ; ≥ 2 architectures **comparées** |
| `DEC-L3-002` | Jeu de Manifests de test (valides/dégradés) ; [format Manifest](../lot-3/manifest-format.md) | [parsing borné](../lot-3/protocols/manifest-bounded-parsing.md) | **Parsing borné prouvé** (aucun débordement sur entrée malformée) ; alternatives de format comparées |
| `DEC-L4-001` | Éventail d'écrans d'éval ; banc FPS/conso ; [familles d'affichage](../lot-4/display-families-comparison.md) | [FPS/rendu](../lot-4/protocols/rendering-fps.md), [conso rétroéclairage](../lot-4/protocols/backlight-power.md), [contention SPI](../lot-4/protocols/spi-contention.md) | Famille/contrôleur/interface conformes ; conso + FPS dans le budget `[BL]` ; **familles comparées** |
| `DEC-L4-002` | Moteur(s) candidat(s) portés ; [comparaison moteurs](../lot-4/graphics-engine-comparison.md) | [budget RAM](../lot-4/protocols/ram-budget.md), [FPS/rendu](../lot-4/protocols/rendering-fps.md), [performances perçues](../lot-4/protocols/perceived-performance.md) | Budget RAM du **moteur candidat** chiffré ; FPS/perçu dans le budget ; alternatives comparées ; **indépendant de la dalle** |
| `DEC-L5-001` | Fixtures de support ; cartes cœur d'éval ; [comparaison supports](../lot-5/socket-support-comparison.md) | [R_contact socket](../lot-5/protocols/contact-resistance-socket.md), [endurance + vibration](../lot-5/protocols/insertion-endurance-vibration.md), [usure remplacements](../lot-5/protocols/support-wear-repeated-swap.md), [GPIO cœur absent](../lot-5/protocols/gpio-core-absent.md) | Contact/endurance/vibration/épaisseur mesurés ; usure sur remplacements successifs ; **familles comparées** |
| `DEC-L7-001` | RTC candidats ; référence de temps ; [comparaison base de temps](../lot-7/timebase-comparison.md) | [dérive vs référence](../lot-7/protocols/drift-vs-reference.md), [conso RTC](../lot-7/protocols/rtc-consumption.md), [Δt sur cycles](../lot-7/protocols/delta-t-wake-cycles.md) | Dérive/conso/Δt mesurés ; validité de l'heure (valide/inconnue) instrumentée ; sources **comparées** |
| `DEC-L7-002` | Supports de persistance ; banc de coupures brutales ; [comparaison persistance](../lot-7/persistence-comparison.md) | [reprise sur coupures brutales](../lot-7/protocols/state-recovery-brutal-cuts.md) | **0 corruption** sur coupures brutales ; les 3 axes (support × cohérence × écriture) comparés |
| `DEC-L8-001` | Toolchain cible ; POC runtime ; [comparaison runtimes](../lot-8/scripted-runtime-comparison.md) | [faisabilité runtime](../lot-8/protocols/runtime-feasibility.md) | RAM/flash **par app** chiffrées ; runtimes **comparés** ; **abstrait par `IRuntime`** (déjà prouvé host) |
| `DEC-L8-002` | POC de chargement ; [modèle de distribution](../lot-8/app-distribution-model.md) | [faisabilité runtime](../lot-8/protocols/runtime-feasibility.md) (volet R7) | Faisabilité (ou limites) de l'install dynamique démontrée (R7) ; approches comparées ; **indépendant du runtime** |

### Vague 3 — Convergence énergie

| `DEC-*` | Prérequis | Protocole(s) | Preuves attendues |
|---------|-----------|--------------|-------------------|
| `DEC-L6-001` | Entrées conso L4/L5/L7 disponibles ; [architecture d'alim](../lot-6/power-chain-architecture.md) | [conso par mode](../lot-6/protocols/consumption-per-mode.md), [charge/thermique](../lot-6/protocols/charge-thermal.md), [fuite module coupé](../lot-6/protocols/module-leakage.md) | Chaîne mesurée (charge, power-path, régulation, fuite, ΔT) ; **architectures comparées** ; power-gating = `DEC-L2-003` |
| `DEC-L6-002` | Cellules candidates ; [comparaison stockage](../lot-6/energy-storage-comparison.md) ; [méthode budget](../lot-6/energy-budget-method.md) | [décharge réelle](../lot-6/protocols/real-discharge.md), [conso par mode](../lot-6/protocols/consumption-per-mode.md) | Capacité/décharge réelles ; **budget énergie** consolidé ; cible d'autonomie **provisoire** `[BL]` justifiée ; chimies (propriété) comparées |
| `DEC-L6-003` | Référence coulomb ; [estimation d'état](../lot-6/battery-state-estimation.md) | [précision d'estimation](../lot-6/protocols/state-estimation-accuracy.md) | Précision vs référence coulomb ; approches ADC/jauge/hybride **comparées** |

### Vague 4 — Décision tardive & structurante

| `DEC-*` | Prérequis | Protocole(s) | Preuves attendues |
|---------|-----------|--------------|-------------------|
| `DEC-L1-002` | Résultats **RAM/écran (L4)** + **deep-sleep/socket (L5)** + cible d'autonomie (L6) | [budget RAM L4](../lot-4/protocols/ram-budget.md), [deep-sleep L5](../lot-5/protocols/deep-sleep-current.md), + [déclencheurs d'arbitrage](../lot-5/arbitration-triggers.md) | Comparaison **A/B/C** sur GPIO/RAM/deep-sleep/intégration/appro ; arbitrage **tracé** ; peut rouvrir [ADR-0004](../../adr/0004-coeur-de-calcul-socket.md) |

## Renvois

- [Synthèse Phase 1](synthesis.md) · [Préparation des campagnes de mesure](campaign-readiness.md)
- [Registre des décisions](../decisions-register.md) · [Plan des ADR](../adr-plan.md) · [Registre des risques](../risk-register.md)
- [Cadre de validation](../validation-framework.md) · [Ordre d'exécution & critères de sortie](../execution.md)

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Phase 1 — Liste prévisionnelle des ADR 0013+ (non créées)

> **Statut : planification.** Cette liste est **prévisionnelle**. **Aucune ADR
> 0013+ n'est créée** dans la PR de planification.

## Modèle : question de décision (`DEC-*`) puis ADR *après* mesures

Le projet applique le modèle **`DEC-*`** unique défini dans le
[cadre de validation](validation-framework.md) (§2) — et **ne mélange pas** avec
un modèle « ADR créée en Proposé avant les essais » :

1. À l'ouverture d'un lot, chaque question de décision reçoit un identifiant
   **`DEC-<lot>-NNN`** (énoncé + options). Le protocole et le rapport référencent
   ce `DEC-*`.
2. L'**ADR (0013+) est créée *après* les mesures**, dans la PR du lot, lorsqu'un
   **arbitrage** est possible (rapport **reproductible**, options comparées,
   seuils atteints ou non).
3. À la création, l'ADR porte le statut correspondant : **Accepté** (seuils
   atteints), **Rejeté** (option écartée, avec trace), ou **Proposé** seulement
   si la décision est prise mais reste à confirmer. Les liens deviennent alors
   bidirectionnels ADR ↔ `DEC-*` ↔ preuves. Regroupements volontaires pour
   éviter l'inflation.

## Liste prévisionnelle

| ADR (prév.) | `DEC-*` associé | Sujet | Lot | Condition de passage en *Accepté* |
|-------------|-----------------|-------|-----|-----------------------------------|
| 0013 | DEC-L1-001 | Allocation GPIO/bus + éventuel GPIO expander | L1/L2 | budget GPIO validé sur le **banc de bring-up GPIO/bus**, sans conflit |
| 0014 | DEC-L2-001 | Sûreté électrique CX-Bus (isolation/commutation, protections, séquencement) | L2 | seuils inrush/court-circuit/bus-stuck/hot-plug atteints (reproductible) |
| 0015 | DEC-L2-002 | Connecteur CX-Bus (**famille + mécanique + mapping physique candidat + endurance**) | L2 | famille choisie + endurance (cycles + résistance de contact) atteintes |
| *(n° à attribuer à la création)* | DEC-L2-003 | **Rails exposés (dont `VBAT`) + stratégie de puissance des modules** | L2 | rails arbitrés après back-powering (2A) + **sûreté d'exposition `VBAT` validée** |
| 0016 | DEC-L3-001 | Mécanisme d'identification + **format binaire du Manifest** | L3 | comparaison d'architectures faite + lecture sûre démontrée |
| 0017 | DEC-L4-001 | **Architecture d'affichage et moteur graphique V1** | L4 | budget du **moteur graphique candidat retenu** + FPS + conso mesurés dans le budget |
| 0018 | DEC-L5-001 | Stratégie du cœur remplaçable (**socketage** ou alternative) | L5 | contact + deep-sleep mesurés ; sinon arbitrage (peut rouvrir ADR-0004) |
| 0019 | DEC-L6-001 | Chaîne d'alimentation (charge/power-path/régulation/load switch + **jauge**) | L6 | chaîne mesurée ; jauge validée (< ±5 %) ou décision ADC |
| 0020 | DEC-L6-002 | **Batterie + cible d'autonomie & budget énergétique** | L6 | budget énergie mesuré ; cible d'autonomie provisoire justifiée |
| 0021 | DEC-L7-001 | **RTC & base de temps** + persistance | L7 | dérive/conso/Δt/reprise d'état aux seuils |
| 0022 | DEC-L8-001 | **Runtime scripté et modèle de distribution des applications** | L8 | budget du **runtime scripté candidat** chiffré ; faisabilité (ou limites) d'installation dynamique démontrée |

**Regroupements & séparations** : connecteur + **mapping physique** ensemble
(0015) ; l'**allocation logique** des signaux CX-Bus reste une **contrainte SPEC
révisable** (hors ADR de connecteur) ; les **rails exposés / `VBAT`** forment une
**décision séparée** (`DEC-L2-003`), dont l'**ADR n'a pas de numéro réservé** —
il sera **attribué à sa création** pour éviter toute collision (l'ordre actuel est
**purement prévisionnel**). Alimentation + jauge ensemble (0019) ; batterie +
autonomie ensemble (0020). Le numérotage exact et d'éventuels regroupements
supplémentaires seront arrêtés à la création, dans la PR du lot.

## Ce qui reste hors périmètre de ces ADR (Phase 1)

- Promotion de la SPEC en **`spec-v1.0.0`** (Phase 1 ⇒ **`rc.1`** au maximum).
- Décisions de **production** (thermique, CEM, DFM/DFT, qualification batterie,
  mécanique finale) — phases ultérieures.
- **Authentification cryptographique** des modules (décision de sécurité
  distincte, si un jour retenue).

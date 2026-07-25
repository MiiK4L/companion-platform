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
3. À la création, l'ADR porte le statut correspondant. Le passage en **Accepté**
   suit la **règle du [cadre de validation](validation-framework.md)** : une
   option **répond aux exigences**, les **alternatives pertinentes ont été
   comparées**, et le **choix est justifié par les preuves et les compromis
   documentés** — l'**atteinte de seuils ne suffit pas** à elle seule. Statuts :
   **Accepté**, **Rejeté** (option écartée, avec trace), ou **Proposé** si la
   décision est prise mais reste à confirmer. Les liens deviennent
   bidirectionnels ADR ↔ `DEC-*` ↔ preuves. Regroupements volontaires pour
   éviter l'inflation.

## Liste prévisionnelle

| ADR (prév.) | `DEC-*` associé | Sujet | Lot | Condition de passage en *Accepté* |
|-------------|-----------------|-------|-----|-----------------------------------|
| 0013 | DEC-L1-001 | Allocation GPIO/bus + éventuel GPIO expander | L1/L2 | budget GPIO validé sur le **banc de bring-up GPIO/bus**, sans conflit |
| 0014 | DEC-L2-001 | Sûreté électrique CX-Bus (isolation/commutation, protections, séquencement) | L2 | exigences de sûreté satisfaites (reproductible) **+ alternatives isolation/commutation comparées + choix justifié (preuves/compromis)** |
| 0015 | DEC-L2-002 | Connecteur CX-Bus (**famille + mécanique + mapping physique candidat + endurance**) | L2 | endurance / R_contact / force satisfaites (reproductible) **+ familles comparées + choix justifié (preuves/compromis)** |
| *(n° à attribuer à la création)* | DEC-L2-003 | **Rails exposés (dont `VBAT`) + stratégie de puissance des modules** | L2 | rails arbitrés : back-powering (2A) + sûreté d'exposition `VBAT` **+ options P1/P2/P3 comparées + choix justifié (preuves/compromis)** |
| 0016 | DEC-L3-001 | **Architecture de découverte et d'identification logique** des modules (découvrabilité logique, identité produit/instance, provisioning ; présence physique = mécanisme dédié éventuel, pas un résultat implicite) | L3 | option conforme + **architectures comparées** + lecture sûre démontrée + **justification par preuves et compromis documentés** |
| *(n° à attribuer à la création)* | DEC-L3-002 | **Format et contenu du Manifest** (description) | L3 | format validé (**parsing borné prouvé**) + alternatives comparées + justification documentée |
| 0017 | DEC-L4-001 | **Matériel d'affichage** (dalle, contrôleur, **interface physique**, format/résolution, contraintes mémoire imposées) | L4 | famille/contrôleur conforme + **familles comparées** + conso/FPS compatibles + justification par preuves et compromis |
| *(n° à attribuer à la création)* | DEC-L4-002 | **Moteur graphique** (architecture de rendu, **stratégie mémoire/buffers**, budget exploité, pipeline) | L4 | budget du **moteur candidat** + FPS mesurés dans le budget + alternatives comparées ; **indépendant** de la dalle (via capacités exposées) |
| 0018 | DEC-L5-001 | **Stratégie de socketage/support** du cœur remplaçable (mécanique + électrique) | L5 | contact/endurance/vibration/épaisseur mesurés + **familles comparées** + choix justifié (preuves/compromis) |
| 0019 | DEC-L6-001 | **Architecture d'alimentation du Host** (charge/power-path/**régulation**/protections/domaines) | L6 | chaîne mesurée + **architectures comparées** + choix justifié (preuves/compromis) ; power-gating module = `DEC-L2-003` |
| 0020 | DEC-L6-002 | **Architectures de stockage d'énergie** (chimie = propriété) + cible d'autonomie & budget | L6 | budget énergie mesuré + **architectures/chimies comparées** + cible d'autonomie provisoire justifiée |
| *(n° à attribuer à la création)* | DEC-L6-003 | **Stratégie d'estimation de l'état de batterie** (ADC / jauge dédiée / hybride) | L6 | précision vs référence coulomb + approches comparées + choix justifié |
| 0021 | DEC-L7-001 | **Base de temps** (source RTC interne/externe/TCXO, **validité de l'heure**, réveil, resync) | L7 | dérive/conso/Δt mesurés + sources comparées + choix justifié |
| *(n° à attribuer à la création)* | DEC-L7-002 | **Modèle de persistance** — **trois axes** : **support** (NVS/LittleFS) + **modèle de cohérence** (journal/A-B/checkpoint/checksum) + **politique d'écriture** (immédiate/différée/à l'événement/périodique/au repos) | L7 | reprise sur coupures brutales (0 corruption) + approches comparées + choix justifié |
| 0022 | DEC-L8-001 | **Runtime scripté V1** (Lua / WASM-WAMR…) | L8 | budget RAM/flash du **runtime candidat** chiffré + runtimes comparés + choix justifié ; **abstrait par le port `IRuntime`** |
| *(n° à attribuer à la création)* | DEC-L8-002 | **Modèle de distribution des applications** (install dynamique vs bundle OTA) — **indépendant du runtime** | L8 | faisabilité (ou limites) d'install dynamique démontrée (R7) + approches comparées + choix justifié |

**Regroupements & séparations** : connecteur + **mapping physique** ensemble
(0015) ; l'**allocation logique** des signaux CX-Bus reste une **contrainte SPEC
révisable** (hors ADR de connecteur) ; les **rails exposés / `VBAT`** forment une
**décision séparée** (`DEC-L2-003`), dont l'**ADR n'a pas de numéro réservé** —
il sera **attribué à sa création** pour éviter toute collision (l'ordre actuel est
**purement prévisionnel**). Architecture d'alimentation Host (0019, **régulation
incluse**) ; **estimation d'état de batterie = décision séparée** (`DEC-L6-003`,
ADR future non réservée) ; stockage d'énergie + autonomie ensemble (0020). Le
numérotage exact et d'éventuels regroupements
supplémentaires seront arrêtés à la création, dans la PR du lot.

## Ce qui reste hors périmètre de ces ADR (Phase 1)

- Promotion de la SPEC en **`spec-v1.0.0`** (Phase 1 ⇒ **`rc.1`** au maximum).
- Décisions de **production** (thermique, CEM, DFM/DFT, qualification batterie,
  mécanique finale) — phases ultérieures.
- **Authentification cryptographique** des modules (décision de sécurité
  distincte, si un jour retenue).

## Statut au checkpoint documentaire (Lot 9)

À la clôture du **volet documentaire** de la Phase 1 ([Lot 9](lot-9/README.md)) —
le volet **expérimental** (mesures, ADR) restant ouvert :

- **Aucune ADR 0013+ n'est créée ni acceptée.** La liste ci-dessus reste
  **prévisionnelle** ; les numéros « à attribuer à la création » ne sont **pas**
  réservés (l'ordre est purement indicatif).
- La règle est respectée : l'**ADR se crée après les mesures** (campagnes de la
  Phase 1), sur un résultat **reproductible**, avec les 4 conditions du
  [cadre §3](validation-framework.md).
- L'**ordre recommandé** de création (donc d'arbitrage) est donné par la
  [feuille de route des décisions](lot-9/decision-roadmap.md) ; l'**état de
  préparation** de chaque mesure par la [préparation des campagnes](lot-9/campaign-readiness.md).
- Tant qu'aucune ADR 0013+ n'existe, la **spécification** reste un
  [draft consolidé](lot-9/consolidated-specification.md) — **non** `rc.1`.

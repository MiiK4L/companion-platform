<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Phase 1 — Liste prévisionnelle des ADR 0013+ (non créées)

> **Statut : planification.** Cette liste est **prévisionnelle**. **Aucune ADR
> 0013+ n'est créée** dans la PR de planification. Chaque ADR naîtra dans la PR
> du lot correspondant, d'abord au statut **Proposé**, et ne passera **Accepté**
> qu'avec une **preuve reproductible** atteignant ses seuils (voir
> [cadre de validation](validation-framework.md)).

## Conditions de création d'une ADR de décision

Une ADR 0013+ est créée **dans la PR de son lot** lorsque :

1. le lot a produit un **rapport de mesure reproductible** (protocole figé,
   seuils, incertitude, données brutes conservées) ; et
2. une décision est réellement **arbitrable** (options comparées, critères
   atteints/non atteints).

Statuts possibles à la création : **Proposé** (par défaut), puis **Accepté**
(preuve atteinte), **Rejeté** (option écartée, conservée pour la trace) ou
**Remplacé par ADR-xxxx**. Regroupements volontaires pour éviter l'inflation.

## Liste prévisionnelle

| ADR (prév.) | Sujet | Lot | Condition de passage en *Accepté* |
|-------------|-------|-----|-----------------------------------|
| 0013 | Allocation GPIO/bus + décision GPIO expander | L1/L2 | budget GPIO validé sur banc (P1), sans conflit |
| 0014 | Sûreté électrique CX-Bus (isolation/commutation, protections, séquencement) | L2 | seuils inrush/court-circuit/bus-stuck/hot-plug atteints (reproductible) |
| 0015 | Connecteur CX-Bus (**famille + brochage + rails exposés, dont `VBAT`**) | L2 | famille choisie + endurance (cycles + résistance de contact) OK + analyse `VBAT` |
| 0016 | Mécanisme d'identification + **format binaire du Manifest** | L3 | comparaison d'architectures faite + lecture sûre démontrée |
| 0017 | Écran (**contrôleur + format**) & budget LVGL | L4 | RAM/FPS/conso mesurés dans le budget |
| 0018 | Stratégie du cœur remplaçable (**socketage** ou alternative) | L5 | contact + deep-sleep mesurés ; sinon peut rouvrir ADR-0004 |
| 0019 | Chaîne d'alimentation (charge/power-path/régulation/load switch + **jauge**) | L6 | chaîne mesurée ; jauge validée (< ±5 %) ou décision ADC |
| 0020 | **Batterie + cible d'autonomie & budget énergétique** | L6 | budget énergie mesuré ; cible d'autonomie provisoire justifiée |
| 0021 | **RTC & base de temps** + persistance | L7 | dérive/conso/Δt/reprise d'état aux seuils |
| 0022 | **Runtime Lua & modèle d'installation d'apps** (faisabilité) | L8 | RAM/flash chiffrées ; faisabilité (ou limites) d'installation dynamique démontrée |

**Regroupements** : connecteur + brochage ensemble (0015) ; alimentation + jauge
ensemble (0019) ; batterie + autonomie ensemble (0020). Le numérotage exact et
d'éventuels regroupements supplémentaires seront arrêtés à la création, dans la
PR du lot.

## Ce qui reste hors périmètre de ces ADR (Phase 1)

- Promotion de la SPEC en **`spec-v1.0.0`** (Phase 1 ⇒ **`rc.1`** au maximum).
- Décisions de **production** (thermique, CEM, DFM/DFT, qualification batterie,
  mécanique finale) — phases ultérieures.
- **Authentification cryptographique** des modules (décision de sécurité
  distincte, si un jour retenue).

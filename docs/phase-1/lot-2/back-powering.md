<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2A — Analyse du back-powering (*Proposé*, `DEC-L2-001`)

> **Statut : Ouvert / Proposé.** Analyse **documentaire** des chemins de
> ré-alimentation parasite. **Entrée** de la décision sur les rails exposés /
> `VBAT` (**`DEC-L2-002`, Lot 2B**). Valeurs **[DS]/[C]/[H]/[P]** ; **aucune [M]**.

## 1. Définition et enjeu

Le **back-powering** (ou back-feeding) désigne l'alimentation **non intentionnelle**
d'un rail ou d'un composant via un chemin parasite : lignes de signal, diodes de
protection internes, ou rails partagés, alors que l'étage concerné est censé être
**hors tension**. Enjeux : alimentation « fantôme » du module malgré le
power-gating, courant de fuite, blocage du power-gating, comportement indéfini au
hot-plug.

## 2. Chemins parasites à examiner

| Chemin | Mécanisme | Étage concerné |
|--------|-----------|----------------|
| **Lignes de bus → module** | Niveaux logiques Host alimentant le module via les diodes ESD/clamp des entrées | `VMOD` coupé mais module « réveillé » par les I/O |
| **Pull-ups I²C** | `SDA`/`SCL` tirés vers `3V3` Host alimentant le module via les broches | Rail module |
| **`VBAT`/`VUSB` exposés** | Si exposés au slot, alimentation directe contournant le power-gating | Selon rails exposés (2B) |
| **Retour `GND` non établi** | Courant cherchant un retour par les lignes de signal | Hot-plug, ordre des contacts |

## 3. Parades possibles (au même niveau, non présélectionnées)

- **Haute-Z des lignes** quand `VMOD` est coupé (bus switch **I3**, ou tampon
  désactivé — voir [isolation](isolation-and-switching.md)).
- **Séquencement** : lignes actives seulement après `VMOD` stable.
- **Découplage des pull-ups** I²C côté module (ou pull-ups côté module alimentés
  par `VMOD`).
- **Ne pas exposer `VBAT`/`VUSB`** (option **P1** de la
  [power-architecture](power-architecture.md)) — mais c'est une décision
  **connecteur (2B)**.
- **Diode/idéal-diode** si un rail entrant doit être protégé contre le reflux.

## 4. Critères chiffrés proposés (à mesurer)

| Critère | Seuil | Étiquette | Alimente |
|---------|-------|-----------|----------|
| Courant de fuite vers le module, `VMOD` coupé | ≤ valeur à fixer (borne d'analyse) | **[P]** | `DEC-L2-001` |
| Tension résiduelle sur rail module, `VMOD` coupé, bus actif | ≈ 0 V (module non réveillé) | **[P]** | `DEC-L2-001` |

Mesure prévue au [protocole hot-plug/injection](protocols/hot-plug-injection.md)
(cas « module non alimenté, bus actif »).

## 5. Alimente

- **`DEC-L2-001`** — exigence « module non alimenté ⇒ haute-Z, pas de reflux ».
- **Entrée** de **`DEC-L2-002`** (exposition ou non de `VBAT`/`VUSB`) — **Lot 2B**.

## Renvois

- [Architecture de puissance](power-architecture.md) · [Isolation & commutation](isolation-and-switching.md)
- [Risques électriques](electrical-risk-analysis.md) · [Protocoles](protocols/README.md)

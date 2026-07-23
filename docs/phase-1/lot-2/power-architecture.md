<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2A — Architecture & chemins de puissance (*Proposé*, `DEC-L2-001`)

> **Statut : Ouvert / Proposé.** Analyse **documentaire** des chemins de courant
> Host ↔ slot ↔ module. Valeurs étiquetées **[DS]/[C]/[H]/[P]** (voir la
> [convention de statut des valeurs](README.md)) ; **aucune
> valeur mesurée [M]**. L'exposition de `VBAT` est **posée en question ouverte**
> et **tranchée en Lot 2B** (`DEC-L2-002`).

## 1. Nœuds et rails à considérer

| Nœud | Origine | Rôle | Remarque |
|------|---------|------|----------|
| `VUSB` (≈ 5 V) | USB-C | Charge batterie, alim en session filaire | Présence intermittente |
| `VBAT` | LiPo | Source principale hors USB | Plage typique **[DS]** ≈ 3,0–4,2 V |
| `VSYS` / `3V3` | Régulation Host | Alim logique Host + périphériques | Rail « propre » régulé |
| `VMOD` | **Commuté** vers le slot | Alim du module CX-Bus | **Power-gated** hors usage |
| Bus I²C / SPI / IRQ | Host | Données vers le slot | Isolables (voir [isolation](isolation-and-switching.md)) |
| `GND` | Commun | Retour | Ordre de contact critique au hot-plug |

## 2. Chemins de courant (schéma logique)

```
USB-C ──▶ [Charge LiPo] ──▶ VBAT ──▶ [Régulation Host] ──▶ 3V3 (logique Host)
                                  │
                                  └─▶ [Load switch VMOD] ──▶ slot (VMOD)  ← power-gating
Bus (I²C/SPI/IRQ) Host ──▶ [Isolation/level] ──▶ slot
```

- Le module est alimenté par **`VMOD` commuté**, distinct du `3V3` logique du
  Host, afin de **couper** le module hors usage et de **borner** son appel de
  courant (voir [protocole inrush](protocols/inrush-et-rampe.md)).
- Le **retour `GND`** doit être **établi en premier** et **rompu en dernier** au
  hot-plug (exigence fonctionnelle ; ordre des contacts non figé — voir
  [hot-plug](hot-plug.md)).

## 3. Question ouverte : quel(s) rail(s) exposer au slot ?

Options **au même niveau** (aucune présélection) :

- **(P1)** exposer **uniquement `VMOD` commuté + `GND` + bus** — le module ne voit
  jamais `VBAT` ni `VUSB` directement.
- **(P2)** exposer **`VBAT`** au slot (module gérant sa propre régulation).
- **(P3)** exposer **`VUSB`** au slot (alim module en session filaire seulement).

Chaque option a des conséquences de **sûreté** (back-powering, inrush, court-circuit)
analysées ici, mais le **choix des rails exposés fait partie du connecteur**
(brochage) : il est **tranché en Lot 2B** (`DEC-L2-002` → ADR-0015). Le présent
lot fournit les **entrées de sûreté** de cette décision.

## 4. Contraintes de dimensionnement (bornes, non figées)

| Grandeur | Valeur | Étiquette | Source / hypothèse |
|----------|--------|-----------|--------------------|
| Tension `VMOD` | 3,3 V nominal | **[H]** | Aligné sur la logique Host ; à confirmer selon module |
| Courant `VMOD` continu max | à définir (borne d'analyse) | **[H]** | Dépend du module ; plafond fixé au dimensionnement du load switch |
| Chute de tension admissible slot | à définir | **[H]** | Fonction de la résistance de contact (Lot 2B) |
| Plage `VBAT` | ≈ 3,0–4,2 V | **[DS]** | Chimie LiPo typique |

> Ces bornes cadrent les protocoles ; elles **ne fixent pas** de spécification.
> Les valeurs définitives dépendront du module, du connecteur (2B) et des mesures.

## 5. Alimente

- **`DEC-L2-001`** — via le principe de **commutation `VMOD` (power-gating)** et le
  séquencement d'alimentation.
- **Entrée** de **`DEC-L2-002`** (rails exposés / `VBAT`) — **décidée en Lot 2B**.

## Renvois

- [Isolation & commutation](isolation-and-switching.md) · [Back-powering](back-powering.md)
- [Composants candidats](candidate-components.md) · [Protocoles](protocols/README.md)

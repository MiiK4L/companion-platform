<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2A — Chemins ESD (analyse **préliminaire**, *Proposé*, `DEC-L2-001`)

> **Statut : Ouvert / Proposé — PRÉLIMINAIRE.** Sans connecteur final, routage ni
> mécanique, cette analyse **ne dimensionne pas** et **ne choisit pas** les
> protections définitives. **Aucune affirmation de conformité normative** ;
> essais **exploratoires uniquement** en aval, avec procédure et équipement
> adaptés. Valeurs **[DS]/[C]/[H]/[P]** ; **aucune [M]**.

## 1. Périmètre couvert (préliminaire)

- **Interfaces potentiellement exposées** au contact/décharge.
- **Chemins de décharge** vers `GND` (retour de courant ESD).
- **Principes de placement** des protections (au plus près du point d'entrée).
- **Familles de protections** candidates.
- **Contraintes** de capacité, tension de claquage et courant de fuite.

## 2. Périmètre explicitement HORS de ce lot

- **Dimensionnement final** (choix exact des composants ESD, valeurs).
- **Choix définitif** des protections — dépend du **connecteur (2B)**, du
  **routage** (PCB intégré) et de la **mécanique** (Phase 4).
- Toute **préconformité / certification** (IEC 61000-4-2 et al.) — **hors
  périmètre Phase 1**.

## 3. Interfaces potentiellement exposées

| Interface | Exposition | Remarque |
|-----------|-----------|----------|
| Contacts du slot CX-Bus | Élevée (insertion/retrait, doigts) | Interface principale — brochage 2B |
| USB-C | Moyenne (connecteur normalisé, souvent protégé) | Protection amont fréquente |
| Boutons / surfaces accessibles | Selon mécanique | Dépend du boîtier (Phase 4) |

## 4. Familles de protections (au même niveau, non dimensionnées)

| Famille | Usage | Contrainte clé |
|---------|-------|----------------|
| **Diodes TVS/ESD unidir./bidir.** (type PESD série) | Clamp rapide des lignes | **Capacité** faible requise sur lignes rapides (SPI) — **[DS]** à vérifier |
| **Réseaux ESD multi-lignes** | Protection compacte de plusieurs lignes | Capacité et diaphonie |
| **Ferrites / série** | Amortissement | Effet sur l'intégrité du signal |

> Sur les lignes **SPI rapides**, la **capacité** de la protection est critique
> (compromis protection ↔ intégrité du signal) — à confronter aux mesures L4.

## 5. Contraintes à respecter (bornes, non figées)

| Contrainte | Valeur | Étiquette | Source |
|-----------|--------|-----------|--------|
| Capacité protection sur ligne rapide | faible (borne à fixer) | **[H]** | Intégrité SPI |
| Tension de service (stand-off) | > tension max du rail concerné | **[H]** | Évite conduction en fonctionnement |
| Courant de fuite | ≤ budget deep-sleep | **[H]** | Lien conso (L5/L6) |

## 6. Alimente

- **`DEC-L2-001`** — au titre des **principes** de protection ; le
  **dimensionnement** est **différé** (connecteur 2B + routage + mécanique).

## Renvois

- [Composants candidats](candidate-components.md) · [Isolation & commutation](isolation-and-switching.md)
- [Analyse des risques électriques](electrical-risk-analysis.md)

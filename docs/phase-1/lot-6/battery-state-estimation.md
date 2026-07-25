<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 6 — Stratégie d'estimation de l'état de batterie (*Proposé*, `DEC-L6-003`)

> **Statut : Ouvert / Proposé.** Titre **large** : **ne présuppose pas** de jauge
> dédiée. Comparaison **par approche** ; les composants **illustrent**. Valeurs
> `[DS]/[H]/[BL]` ; **aucune `[M]`**. → **ADR future non réservée**.

## Approches comparées

| Approche | Principe | Illustré par (non retenu) |
|----------|----------|---------------------------|
| **(E1) ADC seul** | mesure de tension `VBAT` → estimation SOC (courbe de décharge) | ADC du cœur |
| **(E2) Jauge à modèle (voltage-based)** | algorithme de modèle batterie sans shunt | MAX17048, LC709203F |
| **(E3) Jauge coulomb (courant intégré)** | comptage de charge (shunt) | BQ27441 |
| **(E4) Hybride** | tension + coulomb + modèle | (combinaison) |

## Grille comparative (critères communs)

| Critère | (E1) ADC | (E2) Modèle | (E3) Coulomb | (E4) Hybride |
|---------|----------|-------------|--------------|--------------|
| Précision attendue | faible (tension seule) **[H]** | moyenne (modèle) **[H]** | **élevée** (courant) **[H]** | élevée **[H]** |
| Composant dédié | non (ADC intégré) | oui **[DS]** | oui (+ shunt) **[DS]** | oui |
| Conso propre | négligeable | ~µA (ex. 3 µA) **[DS]** | µA + shunt | µA |
| Complexité firmware | **élevée** (modèle maison) | faible (IC) | faible (IC) | moyenne |
| Dépendance chimie/batterie | forte (courbe) | modèle par chimie | robuste | robuste |
| Coût | **nul** (intégré) | moyen | plus élevé (shunt) | élevé |
| Disponibilité | N/A | multi-source **[H]** | à évaluer | selon |

> **Précision** : les **seuils** (erreur d'estimation vs référence coulomb) sont
> **`[BL]`**, définis au **baselining** (voir
> [protocole d'estimation](protocols/state-estimation-accuracy.md)) — **pas** figés ici.

## Synthèse

- **ADC** : gratuit mais précision/robustesse faibles (firmware lourd) ; **modèle**
  (voltage) : bon compromis, faible conso ; **coulomb** : plus précis, coût/shunt ;
  **hybride** : meilleure robustesse, complexité.

## Alimente

- **`DEC-L6-003`** — approche d'estimation arbitrée **après** mesures (précision vs
  référence, conso, robustesse). **Aucune approche retenue** ; **aucune jauge
  présupposée nécessaire.**

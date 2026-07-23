<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2A-STUCK — Bus-stuck (I²C tenu bas) : détection & récupération

> **Statut : Brouillon (en attente de baselining).** Seuils `[P]/[H]` ; champs
> ouverts `[BL]` à geler avant essai ; **aucune `[M]`**. Modèle :
> [protocole de test](../../templates/test-protocol-template.md) ·
> [définitions des événements](event-definitions.md).

## Lot & décision visée

Lot 2A · alimente **`DEC-L2-001`** (mécanisme de récupération de bus).

## Objectif (une question mesurable)

Lorsqu'un esclave/module **maintient `SDA` bas**, le Host **détecte-t-il** le
blocage ≤ 50 ms et **récupère-t-il** automatiquement sans blocage ?

## Conditions initiales

- Bus I²C actif (Host + ≥ 1 esclave) ; fixture capable de **forcer `SDA` bas**
  (MOSFET/relais) de façon commandée.
- Firmware de test avec watchdog de bus (voir
  [bus-stuck & retrait](../bus-stuck-and-transaction.md)).

## Instrumentation requise

- Analyseur logique / oscilloscope (lignes `SDA`/`SCL`).
- Fixture de forçage `SDA` bas commandée ; base de temps commune.

## Montage / fixture

Injection du défaut sur `SDA` en aval des pull-ups ; horodatage de l'instant de
forçage et de l'instant de récupération.

## Conditions d'alimentation

Voir la [matrice partagée](../electrical-risk-analysis.md) (§2). Le bus I²C est
alimenté par le **`3V3` logique Host régulé** ; **rail observé : `3V3`**. Variante
**VA uniquement**. **Coins batterie NON applicables** (la régulation masque `VBAT`)
— tester `3V3` à **±5 %**. USB présent et absent (sans effet attendu ; documenté).

## Fréquence et charge des bus

I²C à 100 kHz **et** 400 kHz **[H]** ; occupation représentative (trafic
périodique) ; documenter le taux.

## Seuils de réussite / échec chiffrés

Verdicts instrumentés : voir [définitions des événements](event-definitions.md)
(« blocage bus » = ligne basse > `[BL]` ms).

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Détection du bus-stuck | ≤ 50 ms | **[P]** |
| Récupération automatique | 0 blocage sur ≥ 100 essais | **[P]** |
| Effet de bord (corruption autres esclaves) | 0 (def. instrumentée) | **[P]** |

### Champs à finaliser au baselining (`[BL]`)

- **Durée** au-delà de laquelle une ligne basse est comptée comme « blocage » (ms).
- **Nombre max d'impulsions `SCL`** de récupération avant escalade (power-cycle).

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 · **`n_runs`** ≥ **100 forçages** (par fréquence I²C) ·
  **`n_campaigns`** ≥ 2 · **`n_cycles`** = n/a.
- **Total** = `n_dut` × 2 fréquences × 100 × `n_campaigns` (figé au baselining).
- **Répartition** : par fréquence (100/400 kHz) × DUT × campagne.
- **Ordre** : instants de forçage **randomisés** dans la trame (graine journalisée).
- **Repos** : non requis (essai non destructif) ; pause si dérive observée.
- **Reprise après échec** : blocage non récupérable → arrêt, journalisation,
  reprise après réinitialisation ; non rétroactif.
- **Méthode** : min/max/percentiles du délai de détection/récupération.

## Critères d'arrêt immédiat

- Récupération impossible (bus définitivement bloqué) répétée.
- Comportement erratique du contrôleur I²C Host (reset non prévu).

## Remise en état entre campagnes

Relâcher le forçage, vérifier l'intégrité des pull-ups et des esclaves,
réinitialiser le firmware de test, revérifier la synchronisation d'horodatage.

## Éléments susceptibles d'être détruits

Faible risque ; éventuellement la fixture de forçage en cas de mauvaise
limitation. Esclaves de test si sur-sollicités.

## Données brutes attendues

Traces logiques `SDA`/`SCL` avec marqueurs forçage/récupération, journal des
essais, mesures de délai ; SHA-256 par fichier au rapport.

## Sécurité opérateur

Faibles tensions ; précautions ESD standard sur le montage ouvert.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Modèle — Protocole de test (à renseigner AVANT l'essai)

> Ce gabarit est réutilisable. Il doit être **entièrement renseigné avant de commencer
> l'essai**. Un protocole figé avant mesure est la condition d'un résultat reproductible
> (voir [Cadre de validation & modèle de preuves](../validation-framework.md)).
>
> **Note : aucune formulation non mesurable (« fiable », « borné », « robuste ») —
> uniquement des nombres.**

## Identifiant du protocole

<!-- Identifiant unique et stable (ex. PROTO-XXXX). -->

## Lot & question(s) de décision visée(s)

<!-- Lot de rattachement et identifiant(s) de question de décision `DEC-<lot>-NNN`
     (ex. DEC-L2-001) que cet essai doit alimenter. NE PAS référencer un numéro
     d'ADR : l'ADR est créée APRÈS les mesures (voir validation-framework §2). -->

## Objectif

<!-- Question précise à laquelle l'essai répond. Une seule question mesurable. -->

## Configuration exacte du banc

<!-- Schéma de câblage, montage, connexions. -->

### Firmware de test + commit exact

<!-- Nom du firmware de test et **commit Git exact** (SHA) utilisé pour l'essai. -->

## Reproductibilité (à renseigner avant l'essai)

<!-- Renseigner chaque champ : ces éléments conditionnent le caractère
     reproductible du résultat (voir validation-framework §4). -->

- **Révision du DUT** (device under test) : <!-- ex. rev A du PCB/banc -->
- **Nombre d'échantillons `n`** : <!-- n ≥ 5 par défaut, à justifier -->
- **Nombre de campagnes indépendantes** : <!-- ≥ 2 pour un résultat reproductible -->
- **Opérateur** : <!-- qui exécute -->
- **Étalonnage des instruments** : <!-- date et référence d'étalonnage -->
- **Méthode statistique** : <!-- moyenne/écart-type/min-max/percentiles… -->
- **Gestion des valeurs aberrantes** : <!-- règle définie AVANT l'essai -->

## Plage de tension

<!-- Valeurs chiffrées (min / nominal / max) en V. -->

## Courant max / modèle de charge

<!-- Courant maximal en A et modèle de charge appliqué. -->

## Fréquence et charge des bus

<!-- Fréquences (Hz/kHz/MHz) et taux d'occupation des bus concernés. -->

## Nombre de cycles

<!-- Nombre de répétitions/cycles prévus. -->

## Durée

<!-- Durée totale de l'essai. -->

## Température (si pertinent)

<!-- Température ou plage de température (°C). Indiquer « non applicable » si sans objet. -->

## Seuils de réussite / échec chiffrés

<!-- Critères numériques de passage/échec. Aucun terme qualitatif. -->

## Incertitude / précision & équipement

<!-- Incertitude de mesure, précision attendue, et référence exacte des instruments. -->

## Données brutes à conserver (emplacement)

<!-- Nature des données brutes et emplacement de stockage (dossier du lot dans la PR). -->

## Sécurité opérateur

<!-- Consignes de sécurité pour l'exécution de l'essai. -->

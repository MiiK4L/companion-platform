<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Modèle — Rapport de mesure

> Ce gabarit consigne les résultats d'un essai mené selon un protocole figé. Il alimente la
> traçabilité ADR ↔ preuves définie dans le
> [Cadre de validation & modèle de preuves](../validation-framework.md).

## Identifiant du rapport

<!-- Identifiant unique et stable (ex. RAPP-XXXX). -->

## Protocole de référence (lien)

<!-- Lien relatif vers le protocole de test appliqué (ex. ./test-protocol-template.md). -->

## Date

<!-- Date de l'essai. -->

## Type de résultat

<!-- Cocher : exploratoire OU reproductible. Seul « reproductible » peut faire passer
     une décision Proposé → Accepté. -->

- [ ] Exploratoire
- [ ] Reproductible

## Banc utilisé

<!-- Description du banc réellement utilisé et de sa configuration. -->

## Reproductibilité & données

- **Révision du DUT** : <!-- rev exacte testée -->
- **Commit firmware exact** : <!-- SHA Git -->
- **Opérateur** : <!-- --> · **Étalonnage instruments** : <!-- date/réf -->
- **Nombre de campagnes indépendantes** : <!-- ≥ 2 pour reproductible -->
- **Nombre d'échantillons `n`** : <!-- --> · **Méthode statistique** : <!-- -->
- **Gestion des valeurs aberrantes** : <!-- règle appliquée -->
- **Hash SHA-256 des données brutes** : <!-- un hash par fichier -->

## Résultats

| Mesure | Seuil | n | Résultats par répétition | Verdict |
|---|---|---|---|---|
| <!-- grandeur --> | <!-- seuil chiffré --> | <!-- n --> | <!-- valeurs / campagnes --> | <!-- atteint / non atteint --> |

## Incertitude

<!-- Incertitude associée aux mesures rapportées. -->

## Écarts / anomalies

<!-- Écarts par rapport au protocole, anomalies observées. -->

## Lien vers les données brutes

<!-- Lien relatif vers les données brutes conservées (dossier du lot dans la PR). -->

## Verdict

<!-- Les seuils sont-ils atteints ? Conséquence :
     - décision passe Proposé → Accepté, ou
     - décision reste Proposé, ou
     - option Rejetée (avec trace). -->

## Question de décision alimentée (`DEC-*`)

<!-- Identifiant `DEC-<lot>-NNN` que ce rapport alimente. L'ADR correspondante
     est créée APRÈS les mesures, lorsqu'un arbitrage est possible ; elle
     référencera ce DEC et ce rapport (liens bidirectionnels). Ne pas exiger de
     numéro d'ADR ici. -->

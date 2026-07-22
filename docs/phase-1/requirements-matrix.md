<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Phase 1 — Matrice initiale des exigences

> **Principe accepté = cœur de calcul remplaçable ([ADR-0004](../adr/0004-coeur-de-calcul-socket.md)).**
> Le XIAO ESP32-S3 est le **candidat de référence de prototypage**, PAS une décision.
> Tout candidat technologique reste au statut **Proposé** tant que les lots de mesure
> (voir [plan de Phase 1](./plan.md)) n'ont pas tranché.

Ce document est un support de **cadrage**. Aucune valeur n'y est décrétée : les cibles
chiffrées sont marquées « à confirmer » et seront fixées par les lots correspondants.

## Besoins de la plateforme (indépendants du MCU)

Ces besoins caractérisent la plateforme elle-même ; ils doivent pouvoir être satisfaits
par n'importe quel cœur de calcul conforme au principe de remplaçabilité.

| Besoin | Valeur / tendance visée | Statut | À confirmer par |
| --- | --- | --- | --- |
| **GPIO** | Décompte des fonctions requises (voir budget GPIO ci-dessous) | Proposé | L1 |
| **Bus** | I²C (capteurs, expander éventuel), SPI (écran, CX-Bus), UART (debug/console) | Proposé | L1–L2 |
| **RAM / flash** | Suffisantes pour la charge de travail UI et le runtime de référence + état applicatif — à chiffrer | Proposé | L4, L8 |
| **USB** | USB-C ; CDC (console/flash) ; host éventuel à évaluer | Proposé | L1 |
| **Consommation** | Cible deep-sleep à fixer (non décrétée ici) | Proposé | L6 |
| **Radio** | Wi-Fi / BT présents (usage à cadrer, impact conso à mesurer) | Proposé | L5–L6 |
| **Dimensions / mécanique** | Objet de poche (contrainte de forme, pas de dimension figée) | Proposé | L1–L5 |
| **Maintenance / remplaçabilité** | Cœur remplaçable sans refonte du reste (cf. ADR-0004) | Accepté | — |
| **Disponibilité / pérennité** | Composants approvisionnables sur la durée de vie du projet | Proposé | tous lots |

### Budget GPIO (décompte des fonctions)

Le budget GPIO liste les fonctions à câbler ; il sert de base au dimensionnement du cœur
et au critère de rejet ci-dessous. Les quantités exactes sont **à confirmer en L1**.

| Fonction | Type de lien | Remarque |
| --- | --- | --- |
| Écran | SPI (+ CS, DC, RST, éventuel backlight) | Partage possible du bus SPI |
| Boutons | GPIO entrées (matrice ou lignes directes) | Nombre à confirmer |
| Buzzer | GPIO / PWM | — |
| Vibreur | GPIO (via driver) | — |
| IRQ accéléromètre | GPIO entrée (interruption) | — |
| IRQ module | GPIO entrée (interruption) | — |
| Enable alimentation | GPIO sortie | Contrôle rail module |
| Détection présence (module) | GPIO entrée | Lié au hot-plug (voir R5) |
| Bus CX-Bus | SPI ou dédié | Contention possible avec l'écran (voir R2) |

> Le total des fonctions ci-dessus, confronté aux GPIO réellement exposés par le candidat,
> conditionne le recours à un expander I²C, au partage du bus SPI, ou au déclenchement
> d'une carte custom (voir critères de rejet).

## Comparaison d'architectures de cœur

Comparaison **non conclusive** : les cases sont à renseigner par la mesure. Ne pas
interpréter la présence d'un candidat « de référence » comme une préférence arrêtée.

| Besoin clé | (A) XIAO ESP32-S3 — candidat de référence | (B) Autre module ESP32-S3 exposant plus de GPIO — candidat alternatif | (C) Carte custom autour de l'ESP32-S3 nu — Option C d'ADR-0004 |
| --- | --- | --- | --- |
| GPIO disponibles vs budget | à évaluer en L1 | à évaluer en L1 | à évaluer en L1 |
| Bus (I²C / SPI / UART) | à évaluer en L1 | à évaluer en L1 | à évaluer en L1 |
| RAM / flash | à évaluer en L4 | à évaluer en L4 | à évaluer en L4 |
| Deep-sleep / consommation | à évaluer en L5 | à évaluer en L5 | à évaluer en L5 |
| Dimensions / intégration poche | à évaluer en L1/L5 | à évaluer en L1/L5 | à évaluer en L1/L5 |
| Disponibilité / pérennité | à évaluer en L1/L5 | à évaluer en L1/L5 | à évaluer en L1/L5 |
| Effort d'intégration / maintenance | à évaluer en L1/L5 | à évaluer en L1/L5 | à évaluer en L1/L5 |

Légende : « à évaluer en L*x* » renvoie au lot du [plan de Phase 1](./plan.md) chargé de
produire la mesure correspondante.

## Critères déclencheurs d'arbitrage du cœur

L'atteinte d'un critère **ne rejette pas automatiquement** le candidat de référence et
**n'impose pas** de carte custom. Elle **déclenche un arbitrage documenté** comparant, avec
leur **coût global**, au moins les options suivantes :

- **autre module** (candidat B) ;
- **carte custom** (Option C d'ADR-0004) ;
- **réduction de périmètre** V1 ;
- **GPIO expander ou partage de bus** (aucun n'est imposé avant d'avoir comparé son coût
  global : composant, GPIO/IRQ consommés, complexité, conso, appro) ;
- **optimisation logicielle** (empreinte mémoire, deep-sleep).

Critères mesurables qui **déclenchent** cet arbitrage (dès qu'**au moins un** est atteint) :

1. **Budget GPIO non couvrable** par le candidat (le recours à un expander/partage de bus est
   une **option de l'arbitrage**, pas un préalable acquis).
2. **Deep-sleep système mesuré au-dessus de la cible d'autonomie** fixée en L6 (la cible n'est
   pas décrétée ici ; établie en L6, confrontée à la mesure L5).
3. **RAM / flash insuffisantes** pour la charge de travail UI et le runtime de référence + état.
4. **Indisponibilité ou pérennité insuffisante** du candidat sur la durée de vie du projet.

> **Effet de gouvernance :** l'arbitrage **remonte une décision structurante** et peut
> **rouvrir [ADR-0004](../adr/0004-coeur-de-calcul-socket.md)**. Toute bascule (module
> alternatif, carte custom, expander, partage de bus…) est **instruite, tracée et validée** —
> jamais un choix implicite.

## Renvois

- [ADR-0004 — Cœur de calcul remplaçable](../adr/0004-coeur-de-calcul-socket.md)
- [Plan de Phase 1](./plan.md) — Lot 1 (inventaire / budget GPIO) et Lot 5 (mesures conso / intégration)
- [Registre initial des risques](./risk-register.md)
- Budget GPIO (section ci-dessus)

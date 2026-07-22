<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# ADR-0008 : Architecture d'alimentation — power-gating et gestion d'énergie explicite

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideur** : Mainteneur du projet (MiiK4L)
- **Expertises consultées** : Expert alimentation, expert basse consommation, architecte électronique
- **Phase de roadmap** : 0 → 1
- **Domaines impactés** : hardware, firmware
- **Tags** : énergie, batterie, jauge, power-gating, autonomie

## 1. Contexte et problème

L'autonomie de plusieurs jours est un objectif majeur, avec un écran TFT
(poste de consommation important, ADR-0003), un connecteur de module qui peut
tirer du courant, et un besoin d'affichage « produit » du niveau de batterie.
Le comportement énergétique doit être maîtrisé de bout en bout, y compris en
deep sleep et lorsque des modules sont connectés.

## 2. Options étudiées

### Mesure du niveau de batterie
- **A. Diviseur de tension + ADC** — simple et gratuit, mais imprécis (courbe
  LiPo non linéaire), pas de suivi de coulombs.
- **B. Jauge de batterie dédiée (MAX17048 / LC709203F)** — pourcentage fiable,
  faible conso propre, comportement « produit ». Surcoût d'une puce I²C.

### Alimentation des modules CX-Bus
- **A. Rail toujours alimenté** — simple, mais un module fuit du courant même
  en veille.
- **B. Power-gating (load switch commandé)** — le module n'est alimenté que
  lorsqu'il est utilisé/détecté ; essentiel pour l'autonomie et le deep sleep.

## 3. Décision

> **Portée du statut.** Seul le *principe* ci-dessous est **Accepté**. Les éléments listés en §6 (Décisions différées) sont au statut **Proposé** et seront actés par de futures ADR une fois validés par prototype.

Principe de **gestion d'énergie explicite** accepté :
- **Recharge USB-C** de la batterie.
- **Power-gating des modules** CX-Bus : un module est mis hors tension en
  veille/deep sleep et n'est alimenté que lorsqu'il est utilisé/détecté.
- **Politique logicielle agressive** : extinction auto du rétroéclairage, deep
  sleep système, réveils sélectifs (voir
  [ADR-0009](0009-rtc-externe-et-persistance-etat.md)).

Les éléments spécifiques — **opportunité et modèle d'une jauge** de batterie
(candidats MAX17048 / LC709203F), **budget énergétique** chiffré, **load switch**
de power-gating, courant de charge, LDO, et **choix de batterie** — relèvent des
décisions différées (§6) : ils ne sont pas actés comme définitifs par la
présente ADR.

## 4. Raisons du choix

Ces **principes** sont ceux d'un produit, pas d'un prototype : aucun gaspillage
par les modules en veille (power-gating), deep sleep réellement bas, recharge
standard. Ils conditionnent l'objectif d'autonomie. Un niveau de batterie fiable
(jauge dédiée) est souhaitable mais reste **candidat** (§6).

## 5. Conséquences

### Positives
- Autonomie visée tenable malgré un écran couleur ; niveau de batterie crédible
  **si** une jauge est retenue.
- Les modules ne pénalisent pas la veille (power-gating).

### Négatives / compromis acceptés
- Quelques composants et GPIO supplémentaires (budget des 11 GPIO du XIAO à
  arbitrer — un GPIO expander pourra être nécessaire).

### Impacts futurs
- Un budget énergétique détaillé (par mode : actif, écran éteint, deep sleep,
  module actif) sera établi en Phase 1 et suivi comme métrique de qualité.

## 6. Décisions différées (statut : Proposé — à valider par prototype)

Éléments au statut **Proposé**, qui feront l'objet de futures ADR une fois
validés par prototypes/mesures :
- l'**opportunité et le modèle d'une jauge** de batterie dédiée (vs mesure ADC)
  et, le cas échéant, le **composant** exact ;
- le **budget énergétique** chiffré par mode (à établir par mesure sur
  prototype) — aucun chiffre d'autonomie n'est garanti à ce stade ;
- le **load switch** de power-gating, le courant de charge, la rampe de
  séquencement ;
- le **choix de batterie** ;
- la faisabilité de la recharge/gestion via le XIAO vs circuit dédié.

## 7. Liens
- [ADR-0003](0003-technologie-et-abstraction-ecran.md) · [ADR-0009](0009-rtc-externe-et-persistance-etat.md) · [ADR-0004](0004-coeur-de-calcul-socket.md)

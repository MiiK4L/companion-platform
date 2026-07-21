# ADR-0008 : Architecture d'alimentation — jauge + power-gating

- **Statut** : Accepté (principes) ; composants figés en Phase 1
- **Date** : 2026-07-21
- **Décideurs** : Expert alimentation, expert basse consommation, architecte électronique
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

Principes actés :
- **Jauge de batterie I²C dédiée** (candidats MAX17048 / LC709203F) pour un
  niveau de charge fiable.
- **Power-gating du connecteur CX-Bus** via un *load switch* commandé par GPIO :
  le module est mis hors tension en veille/deep sleep et séquencé à la détection.
- Gestion d'énergie logicielle agressive : extinction auto du rétroéclairage,
  deep sleep système, réveils sélectifs (voir [ADR-0009](0009-rtc-externe-et-persistance-etat.md)).
- Recharge **USB-C** (assurée par le XIAO en V1, à confirmer/optimiser en Phase 1).

Le choix final des composants (jauge, load switch, courant de charge, LDO)
est **figé en Phase 1** (sélection des composants) et validé en Phase 2/3.

## 4. Raisons du choix

Ces choix sont ceux d'un produit, pas d'un prototype : mesure fiable, aucun
gaspillage par les modules en veille, deep sleep réellement bas. Ils
conditionnent directement l'atteinte de l'objectif d'autonomie.

## 5. Conséquences

### Positives
- Autonomie tenable malgré l'écran couleur ; niveau de batterie crédible.
- Les modules ne pénalisent pas la veille.

### Négatives / compromis acceptés
- Quelques composants et GPIO supplémentaires (budget des 11 GPIO du XIAO à
  arbitrer — un GPIO expander pourra être nécessaire).

### Impacts futurs
- Un budget énergétique détaillé (par mode : actif, écran éteint, deep sleep,
  module actif) sera établi en Phase 1 et suivi comme métrique de qualité.

## 7. Réserves — à valider en Phase 1 (non figé)

> Ce qui est **acté** : les **principes** (recharge USB-C, power-gating des
> modules, gestion d'énergie logicielle agressive). Ce qui **reste à
> mesurer/choisir** :
> - l'**opportunité même** d'une jauge de batterie dédiée (vs mesure ADC) et,
>   le cas échéant, le **composant** exact ;
> - le **budget énergétique** par mode (à établir par mesure sur prototype) —
>   aucun chiffre d'autonomie n'est garanti à ce stade ;
> - le **load switch** de power-gating, le courant de charge, la rampe de
>   séquencement ;
> - la faisabilité de la recharge/gestion via le XIAO vs circuit dédié.

## 6. Liens
- [ADR-0003](0003-technologie-et-abstraction-ecran.md) · [ADR-0009](0009-rtc-externe-et-persistance-etat.md) · [ADR-0004](0004-coeur-de-calcul-socket.md)

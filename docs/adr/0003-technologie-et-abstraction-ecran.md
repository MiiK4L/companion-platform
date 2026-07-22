# ADR-0003 : Interface graphique abstraite (indépendante du contrôleur d'écran)

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideur** : Mainteneur du projet (MiiK4L)
- **Expertises consultées** : Expert UX/UI embarquée, architecte électronique, expert énergie
- **Phase de roadmap** : 0
- **Domaines impactés** : hardware, os, apps
- **Tags** : écran, ui, lvgl, énergie

## 1. Contexte et problème

L'écran est la décision qui pilote simultanément l'autonomie, l'expérience
utilisateur (un personnage « vivant » et animé), la mécanique du boîtier et le
coût. Deux objectifs du cahier des charges sont en tension directe :
« compagnon vivant, animé, moderne » et « autonomie de plusieurs jours ».

## 2. Options étudiées

### Option A — TFT IPS couleur
- **Avantages** : personnage expressif, animations fluides, jeux couleur,
  angles de vue larges, coût maîtrisé, large choix de contrôleurs.
- **Inconvénients** : le rétroéclairage est le premier poste de consommation ;
  autonomie dépendante d'une gestion d'énergie agressive.

### Option B — Sharp Memory LCD (monochrome)
- **Avantages** : autonomie exceptionnelle (µA, image maintenue sans
  rafraîchissement), lisible en plein soleil, esthétique « montre ».
- **Inconvénients** : personnage bien moins vivant, pas de couleur, jeux
  limités — s'éloigne de l'identité « compagnon ».

### Option C — OLED couleur
- **Avantages** : contraste et noirs superbes, très fin.
- **Inconvénients** : **burn-in** rédhibitoire pour un HUD/personnage affiché en
  permanence ; consommation proportionnelle aux pixels allumés ; durée de vie.

## 3. Décision

> **Portée du statut.** Seul le *principe* ci-dessous est **Accepté**. Les éléments listés en §6 (Décisions différées) sont au statut **Proposé** et seront actés par de futures ADR une fois validés par prototype.

Principe d'**interface graphique abstraite** accepté :
- abstraction **complète** du matériel d'affichage via un **moteur graphique**,
  de sorte qu'**aucune application ne dépend directement du contrôleur d'écran** ;
- interface fondée sur des **widgets et sprites**, pas sur des accès pixel ;
- le **contrôleur d'écran est interchangeable** sans modifier les applications ;
- **politique d'économie d'énergie agressive** de l'écran : extinction
  automatique du rétroéclairage, power-gating si possible, deep sleep système.

Les choix spécifiques — **famille TFT IPS couleur**, moteur **LVGL**,
**contrôleur** exact (ST7789 / GC9A01 / ILI9341 / autre), **dimensions /
résolution** et **budget RAM** — relèvent des décisions différées (§6) : ils ne
sont pas actés comme définitifs par la présente ADR.

## 4. Raisons du choix

L'**abstraction graphique** est le choix structurant : elle préserve l'identité
« compagnon vivant » quel que soit l'écran finalement retenu, et protège
l'approvisionnement (un contrôleur peut disparaître du marché). La famille
**TFT IPS couleur** est **pressentie** (seule à préserver couleur, animations et
jeux) mais reste à valider (§6) ; le risque d'autonomie est alors traité par la
gestion d'énergie plutôt que par un renoncement à la couleur.

## 5. Conséquences

### Positives
- UX riche, animée ; personnage comme interface centrale.
- Indépendance vis-à-vis d'un contrôleur donné (résilience composants).

### Négatives / compromis acceptés
- La gestion d'énergie de l'écran sera un chantier de premier plan.
- Le moteur graphique (**LVGL pressenti**) aura une empreinte RAM/flash à
  budgéter et à valider (Phase 1).

### Impacts futurs
- Le boîtier (Phase 4) devra intégrer une fenêtre/vitre et la gestion du
  rétroéclairage.
- La HAL doit exposer une interface « display » indépendante du contrôleur.

## 6. Décisions différées (statut : Proposé — à valider par prototype)

Éléments au statut **Proposé**, qui feront l'objet de futures ADR une fois
validés (prototype P2) :
- le choix de la famille **TFT IPS couleur** ;
- le choix du moteur graphique **LVGL** ;
- le **contrôleur** précis (ST7789 / GC9A01 / ILI9341 / autre) ;
- les **dimensions et la résolution** définitives ;
- le **budget RAM** (buffers, tampon(s) de rafraîchissement) et son impact sur
  la mémoire disponible ;
- la **consommation réelle** de l'écran par mode, mesurée sur prototype.

## 7. Liens
- [ADR-0007](0007-hal-et-companion-sdk.md) · [ADR-0008](0008-architecture-alimentation.md)

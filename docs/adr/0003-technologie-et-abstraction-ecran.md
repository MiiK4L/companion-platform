# ADR-0003 : Écran TFT IPS couleur abstrait par un moteur graphique

- **Statut** : Accepté (orientation) — la **famille** d'écran (TFT IPS couleur)
  et l'abstraction sont actées ; le **contrôleur exact**, les **dimensions /
  résolution** et le **budget RAM de LVGL** sont à valider en Phase 1.
- **Date** : 2026-07-21
- **Décideurs** : Expert UX/UI embarquée, architecte électronique, expert énergie
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

**TFT IPS couleur**, avec les exigences d'architecture suivantes :
- abstraction **complète** du matériel d'affichage via un **moteur graphique**
  (LVGL retenu comme candidat) ;
- **aucune application ne dépend directement du contrôleur LCD** ;
- interface fondée sur des **widgets et sprites**, pas sur des accès pixel ;
- le **contrôleur d'écran est interchangeable** (ST7789, GC9A01, ILI9341…) sans
  modifier les applications ;
- **politique d'économie d'énergie agressive** : extinction automatique du
  rétroéclairage, power-gating si possible, deep sleep système — de sorte que le
  choix d'un TFT n'impacte pas significativement l'autonomie en usage réel.

## 4. Raisons du choix

Le TFT couleur est le seul choix qui préserve l'identité « compagnon vivant » et
les jeux. Le risque (autonomie) est traité par l'architecture (abstraction +
gestion d'énergie) plutôt que par un renoncement à la couleur. L'abstraction
protège aussi l'approvisionnement (un contrôleur peut disparaître du marché).

## 5. Conséquences

### Positives
- UX riche, animée ; personnage comme interface centrale.
- Indépendance vis-à-vis d'un contrôleur donné (résilience composants).

### Négatives / compromis acceptés
- La gestion d'énergie de l'écran devient un chantier de premier plan.
- LVGL a une empreinte RAM/flash non négligeable à budgéter (Phase 1).

### Impacts futurs
- Le boîtier (Phase 5) doit intégrer une fenêtre/vitre et la gestion du
  rétroéclairage.
- La HAL doit exposer une interface « display » indépendante du contrôleur.

## 7. Réserves — à valider en Phase 1 (non figé)

> Ce qui est **acté** : écran **couleur TFT IPS**, abstraction par moteur
> graphique, indépendance des apps vis-à-vis du contrôleur, gestion d'énergie
> agressive. Ce qui **reste à mesurer/choisir** :
> - le **contrôleur** précis (ST7789 / GC9A01 / ILI9341 / autre) ;
> - les **dimensions et la résolution** définitives ;
> - le **budget RAM réel de LVGL** (buffers, tampon(s) de rafraîchissement) et
>   son impact sur la mémoire disponible ;
> - la **consommation réelle** de l'écran par mode, mesurée sur prototype.

## 6. Liens
- [ADR-0007](0007-hal-et-companion-sdk.md) · [ADR-0008](0008-architecture-alimentation.md)

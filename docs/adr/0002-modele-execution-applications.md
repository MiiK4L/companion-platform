# ADR-0002 : Modèle d'exécution des applications — hybride natif + script

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideur** : Mainteneur du projet (MiiK4L)
- **Expertises consultées** : Architecte logiciel embarqué, expert optimisation mémoire
- **Phase de roadmap** : 0
- **Domaines impactés** : os, apps, firmware
- **Tags** : runtime, apps, lua, wasm, extensibilité

## 1. Contexte et problème

Le cœur du projet est de « charger automatiquement les applications » quand un
module est détecté, et de « pouvoir ajouter facilement des apps et des jeux »
pendant des années. Or l'ESP32-S3 ne charge pas du code natif arbitraire à
l'exécution comme un OS de bureau. Le modèle d'exécution des apps est la
décision logicielle la plus lourde de conséquences : elle détermine si ajouter
une app impose de reflasher tout le firmware, et si des tiers peuvent
contribuer des apps sans toucher au cœur.

## 2. Options étudiées

### Option A — Tout natif (registre d'apps compilées)
- **Avantages** : performance maximale, simplicité, robustesse, empreinte
  mémoire minimale.
- **Inconvénients** : **ajouter une app = recompiler et reflasher** ; pas
  d'écosystème tiers réaliste ; contradiction avec « ajouter facilement ».

### Option B — Scripting complet via WASM (WAMR)
- **Avantages** : extensibilité maximale, apps installables sans reflash,
  sandboxing fort, langage-agnostique.
- **Inconvénients** : empreinte RAM/flash plus lourde, complexité d'intégration
  plus élevée dès la V1, maturité de l'outillage à valider sur ESP32-S3.

### Option C — Hybride : cœur natif + apps scriptées
- **Avantages** : performance là où elle compte (OS, drivers, temps réel),
  extensibilité là où elle a de la valeur (apps, jeux, widgets) ; apps
  installables sans reflasher ; ouverture aux contributions tierces.
- **Inconvénients** : nécessite d'intégrer et de maintenir un runtime de script.

### Choix du langage de script (dans l'option C) : Lua vs WASM
- **Lua** : très léger (RAM/flash modestes), intégration C simple et éprouvée
  en embarqué, idéal pour la V1.
- **WASM** : plus lourd mais plus universel et mieux sandboxé, pertinent à terme.

## 3. Décision

> **Portée du statut.** Seul le *principe* ci-dessous est **Accepté**. Les éléments listés en §6 (Décisions différées) sont au statut **Proposé** et seront actés par de futures ADR une fois validés par prototype.

Principe **hybride** accepté :
- Les **services critiques** (OS, noyau, drivers, HAL, gestion des modules,
  sécurité, alimentation, communications) restent **natifs**.
- Les **applications, jeux, widgets et extensions** s'exécutent dans un
  **runtime de script**, et sont conçus pour être **installables indépendamment
  du firmware**.
- L'architecture est bâtie autour d'une **abstraction de runtime** afin de
  pouvoir changer ou ajouter un moteur (Lua, WASM ou autre) plus tard **en
  limitant l'impact sur le reste de l'OS**.

Le choix du runtime concret de la V1 (**Lua** pressenti) et la **faisabilité de
l'installation dynamique** d'apps relèvent des décisions différées (§6) : ils ne
sont pas actés comme définitifs par la présente ADR.

## 4. Raisons du choix

L'hybride est le seul modèle qui satisfait simultanément la performance/robustesse
du cœur et la promesse d'extensibilité. Lua minimise le risque et l'empreinte en
V1 ; l'abstraction de runtime protège l'avenir sans surcoût immédiat — exactement
la logique « qualité et évolutivité avant rapidité ».

## 5. Conséquences

### Positives
- **Objectif visé** : ajouter une app/un jeu **sans reflasher** le firmware —
  **conditionné** à la validation de l'installation dynamique (P8, §6) ; non
  garanti à ce stade.
- Écosystème tiers possible ; le Tamagotchi devient une app comme une autre.

### Négatives / compromis acceptés
- Intégration et maintenance d'un runtime de script (Lua candidat) + d'une couche d'abstraction.
- Les apps scriptées sont plus lentes que du natif (acceptable pour de l'UI/jeux).

### Impacts futurs
- Le Companion SDK doit exposer une API stable, utilisable depuis C natif comme
  depuis Lua (voir [ADR-0007](0007-hal-et-companion-sdk.md)).
- Un futur backend WASM se branche derrière l'abstraction de runtime.

## 6. Décisions différées (statut : Proposé — à valider par prototype)

Éléments au statut **Proposé**, qui feront l'objet de futures ADR une fois
validés :
- le choix du runtime **Lua** comme moteur de script de la V1 (empreinte
  RAM/flash réelle, intégration) — à valider (prototype P8) ;
- la **faisabilité de l'installation dynamique** d'apps sur ESP32-S3
  (mécanisme, limites, sécurité) — à démontrer par prototype (P8) avant d'être
  considérée comme une fonctionnalité garantie.

## 7. Liens
- [ADR-0007](0007-hal-et-companion-sdk.md) · [ADR-0001](0001-framework-firmware-esp-idf.md)

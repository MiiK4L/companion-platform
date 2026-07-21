# ADR-0007 : Inversion des dépendances — HAL + Companion SDK

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideurs** : Architecte logiciel embarqué, architecte firmware
- **Phase de roadmap** : 0
- **Domaines impactés** : firmware, os, apps
- **Tags** : hal, sdk, portabilité, découplage, tests

## 1. Contexte et problème

Le projet doit rester vivant pendant des années et pouvoir, si nécessaire,
changer de MCU, de RTOS, de contrôleur d'écran ou de runtime de script **sans
réécrire les applications**. Si les apps et les services métier appelaient
directement ESP-IDF/FreeRTOS ou un contrôleur précis, tout changement bas niveau
casserait tout le haut niveau. Il faut inverser les dépendances.

## 2. Options étudiées

### Option A — Apps appelant directement ESP-IDF / bibliothèques matérielles
- **Avantages** : moins de couches, développement initial plus rapide.
- **Inconvénients** : couplage fort au silicium ; portage = réécriture ; tests
  impossibles hors cible ; contradiction avec les objectifs de pérennité.

### Option B — Une HAL seule
- **Avantages** : découple l'OS du matériel.
- **Inconvénients** : la HAL est de bas niveau ; l'exposer directement aux apps
  reste trop technique et instable pour un écosystème d'apps (natives + Lua).

### Option C — HAL (bas niveau) + Companion SDK (API applicative stable)
- **Avantages** : deux frontières nettes — la HAL abstrait le **matériel/RTOS**,
  le Companion SDK offre une **API applicative stable** (liable en natif ET en
  Lua). Portage = réimplémenter la HAL ; les apps ne bougent pas. Tests des apps
  et de l'OS sur PC via HAL mockée.
- **Inconvénients** : plus de couches à concevoir et maintenir.

## 3. Décision

Adopter le **principe d'inversion des dépendances matérielles** :

```
apps  →  Companion SDK  →  HAL  →  drivers  →  silicium (ESP-IDF/FreeRTOS)
```

- **Aucune couche ne saute une autre.**
- ESP-IDF/FreeRTOS ne sont connus que de la HAL, des drivers et du noyau.
- Les **applications et services métier ne dépendent QUE du Companion SDK**.
- La HAL possède une implémentation **cible** (ESP32-S3) et une implémentation
  **hôte (mock)** pour les tests sur PC.

## 4. Raisons du choix

C'est la traduction architecturale directe des exigences de portabilité et de
testabilité. Le surcoût de couches est un investissement qui protège l'ensemble
du haut niveau contre les changements bas niveau — le meilleur rapport
maintenabilité/évolutivité, cœur des priorités du projet.

## 5. Conséquences

### Positives
- Portabilité MCU/RTOS/écran/runtime sans toucher aux apps.
- Tests unitaires host (rapides, sans matériel) → CI de qualité (voir tests/).
- API applicative stable = écosystème d'apps possible.

### Négatives / compromis acceptés
- Conception et maintenance de deux frontières d'API (HAL + SDK).
- Discipline requise : toute violation de couche doit être bloquée en revue/CI.

### Impacts futurs
- Le Companion SDK devient le contrat public du projet : sa stabilité est
  versionnée (SemVer, voir [ADR-0012](0012-monorepo-et-versioning.md)).
- Un backend WASM (ADR-0002) et un nouveau contrôleur d'écran (ADR-0003) se
  branchent sous ces frontières sans impact sur les apps.

## 6. Liens
- [ADR-0001](0001-framework-firmware-esp-idf.md) · [ADR-0002](0002-modele-execution-applications.md) · [ADR-0003](0003-technologie-et-abstraction-ecran.md)

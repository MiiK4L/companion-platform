# ADR-0007 : Modèle de dépendances — ports & adaptateurs (Companion SDK)

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideur** : Mainteneur du projet (MiiK4L)
- **Expertises consultées** : Architecte logiciel embarqué, architecte firmware
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

### Option C — Ports abstraits + adaptateurs, avec Companion SDK en façade
- **Avantages** : deux frontières nettes — des **ports** abstraits (portables)
  côté matériel/RTOS, implémentés par des **adaptateurs** ; et le **Companion
  SDK**, API applicative stable utilisable depuis C natif comme depuis Lua.
  Un portage revient à écrire un nouvel adaptateur ; les tests tournent sur PC
  via des adaptateurs host (mocks).
- **Inconvénients** : plus d'éléments à concevoir et maintenir.

## 3. Décision

Adopter un modèle **ports / adaptateurs** (inversion des dépendances) :

```
apps → Companion SDK → services → ports abstraits ◄─ adaptateurs (ESP32-S3/ESP-IDF · host/mock)
                                                          └─ drivers · ESP-IDF · FreeRTOS
```

Règles (détaillées dans [dependency-inversion](../architecture/dependency-inversion.md)) :

1. Les apps ne dépendent que du **Companion SDK**.
2. Le Companion SDK est une **façade fournie par les services** ; il ne dépend
   pas directement des drivers.
3. Les **services** dépendent d'**interfaces abstraites (les ports)**, jamais
   d'ESP-IDF ni de pilotes concrets.
4. Les **adaptateurs et drivers** dépendent des interfaces qu'ils **implémentent**.
5. Le paquet des **ports reste portable** et ne connaît pas ESP-IDF.
6. Le **point de composition** assemble ports et implémentations au démarrage.
7. Une dépendance **peut contourner un niveau** s'il n'y a pas de raison
   fonctionnelle de le traverser ; ce qui est interdit, c'est qu'une abstraction
   dépende d'un détail concret.
8. La **portabilité est un objectif, pas une garantie absolue**.

Les ports ont des adaptateurs **cible** (ESP32-S3) et **host (mock)** pour les
tests sur PC.

## 4. Raisons du choix

C'est la traduction architecturale directe des exigences de portabilité et de
testabilité. Le surcoût de couches est un investissement qui protège l'ensemble
du haut niveau contre les changements bas niveau — le meilleur rapport
maintenabilité/évolutivité, cœur des priorités du projet.

## 5. Conséquences

### Positives
- Portabilité MCU/RTOS/écran/runtime **visée** : compatibilité applicative
  préservée dans les limites du contrat du SDK (objectif, non garanti).
- Tests unitaires host (rapides, sans matériel) → CI de qualité (voir tests/).
- API applicative stable = écosystème d'apps possible.

### Négatives / compromis acceptés
- Conception et maintenance de deux frontières (ports + SDK).
- Discipline requise : une abstraction qui dépend d'un détail concret
  (en-tête ESP-IDF, driver) doit être bloquée en revue/CI.

### Impacts futurs
- Le Companion SDK devient le contrat public du projet : sa stabilité est
  versionnée (SemVer, voir [ADR-0012](0012-monorepo-et-versioning.md)).
- Un backend WASM (ADR-0002) et un nouveau contrôleur d'écran (ADR-0003) se
  branchent derrière ces frontières ; l'impact sur les apps est visé nul dans
  les limites du contrat, mais dépend des capacités effectivement exposées.

## 6. Liens
- [ADR-0001](0001-framework-firmware-esp-idf.md) · [ADR-0002](0002-modele-execution-applications.md) · [ADR-0003](0003-technologie-et-abstraction-ecran.md)

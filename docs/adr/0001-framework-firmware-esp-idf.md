# ADR-0001 : Framework firmware — ESP-IDF comme fondation

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideurs** : Architecte firmware, expert ESP32, expert basse consommation
- **Phase de roadmap** : 0
- **Domaines impactés** : firmware, os
- **Tags** : firmware, rtos, esp32, fondation

## 1. Contexte et problème

Le firmware doit se comporter comme un petit système d'exploitation : gestion
d'énergie fine (deep sleep, réveils multiples), partitionnement flash, OTA
signé, Secure Boot, ordonnancement, pilotes bas niveau. Le choix du framework
de base conditionne le contrôle qu'on aura sur le matériel et la robustesse à
long terme. Le cahier des charges interdit « une dépendance à Arduino Uno » —
cela concerne la carte, pas nécessairement le framework logiciel, point qu'il
fallait clarifier.

## 2. Options étudiées

### Option A — ESP-IDF (FreeRTOS)
- **Avantages** : framework officiel Espressif, contrôle total (énergie,
  partitions, OTA, Secure Boot/Flash Encryption), modèle de composants propre,
  support long terme, professionnel, base de l'écosystème ESP32.
- **Inconvénients** : courbe d'apprentissage plus raide qu'Arduino, API plus
  verbeuse.

### Option B — Arduino-ESP32
- **Avantages** : ergonomie de développement, large écosystème de bibliothèques.
- **Inconvénients** : abstractions qui masquent le bas niveau (énergie,
  timings), moins de contrôle fin ; c'est de toute façon une surcouche
  d'ESP-IDF. Inadapté comme *fondation* d'un « vrai système ».

### Option C — Zephyr RTOS
- **Avantages** : le plus portable et le plus propre (HAL native, idéal en cas
  de changement de MCU), modèle de pilotes unifié.
- **Inconvénients** : support ESP32-S3 (Wi-Fi/BT) moins mature qu'ESP-IDF,
  courbe d'apprentissage la plus raide, risque sur des fonctionnalités clés.

## 3. Décision

**ESP-IDF (FreeRTOS)** est la fondation du firmware de la V1. Toutefois,
ESP-IDF et FreeRTOS sont **confinés aux adaptateurs cible et aux drivers** (et au
**point de composition** qui les assemble au démarrage), et **jamais exposés**
aux ports abstraits, aux services, au SDK ni aux applications, qui n'en dépendent
pas (modèle ports/adaptateurs — voir [ADR-0007](0007-hal-et-companion-sdk.md)).

## 4. Raisons du choix

ESP-IDF offre le meilleur contrôle bas niveau, indispensable aux objectifs
d'autonomie et de sécurité, avec le support le plus solide pour l'ESP32-S3.
Le risque de portabilité (le principal argument de Zephyr) est neutralisé par
la HAL et le Companion SDK : la portabilité est traitée par notre architecture,
pas déléguée au framework.

## 5. Conséquences

### Positives
- Accès complet à la gestion d'énergie, l'OTA, le Secure Boot.
- Écosystème et documentation matures.

### Négatives / compromis acceptés
- Ergonomie de développement inférieure à Arduino → compensée par le
  Companion SDK qui offrira une API confortable aux développeurs d'apps.

### Impacts futurs
- Un portage futur vers Zephyr ou un autre MCU reste possible en écrivant un
  nouvel **adaptateur cible** ; l'objectif est de préserver la compatibilité
  applicative dans les limites du contrat du Companion SDK, sans garantie
  absolue (voir ADR-0007).

## 7. Réserves — à valider ultérieurement (non figé)

> Ce qui est **acté** : **ESP-IDF/FreeRTOS** comme fondation, confinés sous la
> HAL. Ce qui **n'est pas un prérequis de prototypage** :
> - **Secure Boot**, **Flash Encryption** et **signature OTA** sont des cibles
>   de maturité « production », activées progressivement — **pas** exigées
>   pendant le prototypage (elles compliquent le flashage/debug). Leur activation
>   fera l'objet d'une décision dédiée le moment venu.

## 6. Liens
- [ADR-0007](0007-hal-et-companion-sdk.md) · [ADR-0002](0002-modele-execution-applications.md)

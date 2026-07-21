# Companion SDK — Surface d'API des applications

> Statut : **Phase 0 — Fondations**. L'API détaillée est marquée « ⏳ À définir ».

Le **Companion SDK** est **LA surface d'API stable** que voient les applications. C'est le
contrat entre les apps et la plateforme : une app — native ou Lua — ne connaît **rien d'autre**
que le SDK.

Références de décision : ADR-0001, ADR-0002, ADR-0007.

## Principe fondamental

> Aucune app **NE DÉPEND** d'ESP-IDF, de FreeRTOS ou de la HAL directement. Une app dépend
> **uniquement du Companion SDK.**

C'est ce qui **vise** à rendre MCU, RTOS, écran et runtime remplaçables en **préservant la
compatibilité applicative dans les limites du contrat et des capacités garanties par le SDK** —
un objectif, pas une garantie absolue. Le SDK est conçu pour rester **stable** dans le temps
même si les implémentations sous-jacentes évoluent ; il sert les apps **natives** (compilées
dans l'image) comme les apps **Lua** (chargées par le runtime — ADR-0002).

## Grands domaines d'API

| Domaine | Objet |
|---------|------|
| **Affichage / widgets** | Dessin, widgets et sprites via le moteur graphique (LVGL sous-jacent, ADR-0003) — sans exposer LVGL directement. |
| **Entrées** | Événements de boutons et d'interaction utilisateur. |
| **Stockage** | Persistance des données d'app (état, sauvegardes). |
| **Réseau** | Accès Wi-Fi / Bluetooth à la demande (via le service Connectivity). |
| **Tâches / timers** | Ordonnancement applicatif, temporisations, réveils programmés. |
| **Accès modules (capacités)** | Découverte et usage des modules CX-Bus **par capacité déclarée** (Manifest), sans accès matériel brut. |
| **Personnage / Companion** | Interaction avec le service Companion (état, présentation incarnée). |

## Règles de dépendance

- Le SDK s'appuie sur les **services** (et le kernel), jamais sur ESP-IDF/FreeRTOS/HAL en
  direct depuis le point de vue de l'app.
- Le SDK **expose** des capacités ; il **n'expose jamais** un handle matériel brut, un objet
  LVGL natif, ni une primitive FreeRTOS — cela briserait l'inversion des dépendances.
- L'accès aux modules se fait **par capacité** (déclarée dans le CX-Bus Manifest), jamais par
  adressage matériel direct depuis l'app.

> ⏳ **À définir — Phases ultérieures** : signatures d'API par domaine, bindings Lua, modèle
> de permissions/capacités, versionnement de l'API du SDK, garanties de stabilité.

## Licence

Firmware sous **Apache-2.0**. Documentation sous **CC-BY-4.0**.

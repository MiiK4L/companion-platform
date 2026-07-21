# Kernel — Noyau de la plateforme

> Statut : **Phase 0 — Fondations**. Les mécanismes détaillés sont marqués « ⏳ À définir ».

Le **kernel** (noyau) fournit les mécanismes centraux de la plateforme au-dessus de la HAL :
ordonnancement, gestion d'énergie, bus d'événements et gestion d'état. Il orchestre le
fonctionnement du système sans jamais exposer les détails du RTOS aux couches supérieures.

Références de décision : ADR-0007, ADR-0008, ADR-0009.

## Responsabilités

- **Ordonnancement** : coordination des tâches **au-dessus de FreeRTOS**. FreeRTOS n'est
  **jamais exposé aux apps** ni aux services applicatifs ; le kernel encapsule
  l'ordonnancement sous-jacent (accédé via la HAL) et présente un modèle de tâches propre.
- **Gestion d'énergie / deep sleep** : politiques de basse consommation, entrée/sortie de
  deep sleep, coordination des sources de réveil (bouton, RTC, mouvement — ADR-0008).
- **Bus d'événements** : diffusion d'événements système entre services (entrées, réveils,
  insertion/retrait de module, changements d'état).
- **Gestion d'état & persistance** : sauvegarde/restauration de l'état système et applicatif
  en mémoire non volatile, support du calcul du delta temps au réveil (ADR-0009).

## Règles de dépendance

- Le kernel dépend **uniquement de la HAL** (et d'utilitaires internes). Il **NE DOIT PAS**
  inclure ESP-IDF ni appeler FreeRTOS directement (ADR-0007) : tout passe par la HAL.
- Le kernel **NE DOIT PAS** dépendre des services, du SDK ou des apps (dépendances
  descendantes uniquement).
- Les services s'appuient sur les mécanismes du kernel ; les apps n'y accèdent **jamais**
  directement — seulement via le Companion SDK.

> ⏳ **À définir — Phases ultérieures** : modèle de tâches exposé, API du bus d'événements,
> format de persistance d'état, politiques d'énergie et seuils de deep sleep.

## Licence

Firmware sous **Apache-2.0**. Documentation sous **CC-BY-4.0**.

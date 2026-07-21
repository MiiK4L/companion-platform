# HAL — Hardware Abstraction Layer

> Statut : **Phase 0 — Fondations**. Les interfaces détaillées sont marquées « ⏳ À définir ».

La **HAL** (couche d'abstraction matérielle) définit **quoi** le matériel doit savoir faire,
sous forme d'interfaces stables, indépendamment de **comment** c'est réalisé sur un MCU donné.
Elle est le point de découplage entre le matériel et le reste du firmware.

Références de décision : ADR-0001, ADR-0007.

## Responsabilités

La HAL expose des interfaces abstraites pour :

- **Écran** : primitives de sortie graphique (surface de rendu pour LVGL, ADR-0003).
- **Entrées** : boutons et événements d'entrée.
- **Bus** : I²C, SPI, UART (transport brut, arbitrage des bus partagés).
- **Énergie** : modes basse consommation, deep sleep, sources de réveil, power-gating.
- **Timers** : temporisations, horloge monotone.
- **Stockage** : accès mémoire non volatile (flash/NVS abstraite).

## Règles de dépendance

- La HAL est, **avec les drivers**, la **seule couche autorisée à connaître ESP-IDF/FreeRTOS**
  (ADR-0007). Elle traduit les besoins abstraits en primitives fournies par les drivers.
- La HAL **NE DOIT PAS** dépendre des couches supérieures (kernel, services, SDK, apps).
- Les couches supérieures dépendent des **interfaces** de la HAL, jamais d'une implémentation
  matérielle concrète.
- La HAL définit les interfaces ; les **drivers** les implémentent pour un composant donné.

## Testabilité (implémentation mockée)

La HAL rend possible les **tests sur machine hôte** (host) : en fournissant une
implémentation **mockée** des interfaces, le kernel, les services et le SDK peuvent être
compilés et testés sans matériel réel ni ESP-IDF. C'est un objectif structurant de la couche.

> ⏳ **À définir — Phases ultérieures** : signatures exactes des interfaces (écran, bus,
> énergie, stockage…), conventions d'erreur, modèle de configuration matérielle, banc de
> mocks host.

## Licence

Firmware sous **Apache-2.0**. Documentation sous **CC-BY-4.0**.

# Companion Platform

**Companion Platform** est une plateforme électronique de poche **open source** et **modulaire**, conçue pour durer des années plutôt que pour suivre une mode passagère. Construite autour du microcontrôleur **Seeed XIAO ESP32-S3** — socketé, donc remplaçable — elle est pensée comme un petit ordinateur de poche capable d'accueillir de nombreuses applications au fil du temps : un Tamagotchi pour commencer, puis des outils réseau, un homelab miniature, de l'électronique, des capteurs, des jeux, et bien d'autres usages imaginés par la communauté.

Ce projet fait le pari inverse de l'obsolescence programmée : chaque choix technique — matériel, firmware, logiciel, documentation — est évalué d'abord sur sa **robustesse**, sa **maintenabilité** et sa **capacité à évoluer**, jamais sur la rapidité de mise en œuvre.

## Quatre décisions structurantes

L'architecture de Companion Platform repose sur quelques décisions verrouillées, documentées sous forme d'[ADR](adr/README.md) :

1. **Un runtime d'applications hybride, natif et Lua** ([ADR-0002](adr/README.md)) — les applications critiques ou performantes peuvent être écrites en natif, tandis que les applications communautaires, plus légères à distribuer et à sandboxer, s'écrivent en Lua.
2. **Un écran TFT IPS couleur, entièrement abstrait par LVGL** ([ADR-0003](adr/README.md)) — les applications dessinent leur interface via LVGL, sans jamais dépendre du contrôleur d'écran réellement présent sur la carte.
3. **Un standard d'extension matériel unifié, le CX-Bus** *(Companion eXpansion Bus)* ([ADR-0005](adr/README.md) / [ADR-0006](adr/README.md)) — chaque module d'extension (capteur, connectivité, affichage...) s'identifie et se déclare de façon standardisée, via une EEPROM embarquée.
4. **Un framework ESP-IDF encapsulé derrière un HAL et un Companion SDK obligatoires** ([ADR-0001](adr/README.md) / [ADR-0007](adr/README.md)) — aucune application ne dépend jamais directement d'ESP-IDF ou de FreeRTOS.

## Le principe directeur : inverser les dépendances matérielles

Companion Platform applique systématiquement un principe d'**inversion des dépendances matérielles** : les couches de haut niveau ne dépendent jamais directement des couches de bas niveau.

```
apps → Companion SDK → HAL → drivers → silicium
```

Une application ne connaît que le Companion SDK. Le SDK ne connaît que le HAL (Hardware Abstraction Layer). Le HAL seul dialogue avec les drivers, qui seuls touchent au silicium. Ce principe garantit qu'une évolution matérielle — un nouvel écran, un nouveau capteur, voire un nouveau microcontrôleur — n'oblige jamais à réécrire les applications qui tournent dessus.

Pour aller plus loin, consultez la page [Inversion des dépendances](architecture/dependency-inversion.md).

## Pour aller plus loin

- [Vue d'ensemble de l'architecture](architecture/overview.md) — comment le matériel, le firmware et les applications s'articulent.
- [Décisions d'architecture (ADR)](adr/README.md) — l'historique complet des choix structurants du projet.
- [Roadmap](roadmap.md) — les phases de développement prévues.
- [Guides](guides/contributing-code.md) — pour contribuer au code, développer une application ou un module, ou flasher votre carte.

## Licences

Companion Platform est un projet de référence, publié sous plusieurs licences selon la nature du contenu : **Apache-2.0** pour le firmware, l'OS, le SDK et les outils ; **CERN-OHL-S-2.0** pour le matériel ; **CC-BY-4.0** pour la documentation. L'identité visuelle du projet reste réservée. Voir [ADR-0010](adr/README.md) pour le détail des licences retenues.

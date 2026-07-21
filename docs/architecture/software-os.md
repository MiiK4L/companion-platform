# Architecture logicielle / OS

Le firmware est conçu comme un **petit système d'exploitation** : le Tamagotchi
n'est jamais dans le noyau, c'est une application. Décisions liées :
[ADR-0001](../adr/0001-framework-firmware-esp-idf.md) (ESP-IDF),
[ADR-0002](../adr/0002-modele-execution-applications.md) (runtime),
[ADR-0007](../adr/0007-hal-et-companion-sdk.md) (HAL/SDK).

## Couches

```
Bootloader (ESP-IDF, Secure Boot + OTA signé — cible)
   │
Noyau (kernel) : ordonnancement (au-dessus de FreeRTOS, jamais exposé),
   │             gestion d'énergie & deep sleep, bus d'événements,
   │             gestion d'état & persistance (NVS / LittleFS)
   │
HAL : abstraction matérielle (voir inversion des dépendances)
   │
Drivers : implémentations concrètes
   │
Services : Module Manager · App Manager · UI (LVGL) · Power ·
   │        Storage · Connectivity (Wi-Fi/BT) · Companion
   │
Companion SDK : API stable pour les apps (natif + Lua)
   │
Applications & Jeux (Lua) — Tamagotchi, réseau, homelab, électronique, capteurs…
```

Détail des responsabilités dans les READMEs de couches :
[`firmware/`](https://github.com/MiiK4L/companion-platform/blob/main/firmware/README.md), [`hal`](https://github.com/MiiK4L/companion-platform/blob/main/firmware/hal/README.md),
[`drivers`](https://github.com/MiiK4L/companion-platform/blob/main/firmware/drivers/README.md),
[`kernel`](https://github.com/MiiK4L/companion-platform/blob/main/firmware/kernel/README.md),
[`services`](https://github.com/MiiK4L/companion-platform/blob/main/firmware/services/README.md),
[`companion-sdk`](https://github.com/MiiK4L/companion-platform/blob/main/firmware/companion-sdk/README.md).

## Services clés

- **Module Manager** — détecte l'insertion/retrait d'un module CX-Bus, lit et
  valide le **CX-Bus Manifest** (EEPROM), charge les drivers, publie les
  capacités et déclenche l'apparition des apps associées.
- **App Manager** — cycle de vie des applications (installation, lancement,
  suspension, sauvegarde), exécution via le **runtime Lua**, derrière une
  **abstraction de runtime** ouverte à WASM plus tard.
- **UI** — moteur graphique (LVGL) : widgets, sprites, animations, transitions ;
  aucune app ne touche le pixel ni le contrôleur d'écran ([ADR-0003](../adr/0003-technologie-et-abstraction-ecran.md)).
- **Power** — deep sleep, extinction auto de l'écran, réveils (bouton, alarme
  RTC, mouvement de l'accéléromètre), power-gating des modules ([ADR-0008](../adr/0008-architecture-alimentation.md)).
- **Storage** — état/réglages en NVS, données/sauvegardes en LittleFS.
- **Connectivity** — Wi-Fi / Bluetooth, exposés aux apps via le SDK.
- **Companion** — service **transverse** : traduit un état système en
  expression du personnage. C'est ce qui unifie l'expérience (« CPU 80 % » →
  personnage fatigué). N'importe quelle app peut piloter le personnage.

## Modèle d'application

Une application est un paquet **installable indépendamment du firmware**
([ADR-0002](../adr/0002-modele-execution-applications.md)), décrit par un
manifeste (nom, version, auteur, capacités matérielles requises, apps/écrans
exposés). L'App Manager l'active si ses exigences sont satisfaites — par exemple
lorsqu'un module CX-Bus fournissant la capacité requise est présent.

## Persistance & évolution hors tension

Le système privilégie un **deep sleep réel** : au réveil, il lit le RTC externe
et applique l'évolution correspondant au **Δt** écoulé (le Tamagotchi
« vieillit » sans tâche de fond permanente). Voir
[ADR-0009](../adr/0009-rtc-externe-et-persistance-etat.md).

## Sécurité (périmètre)

Secure Boot et Flash Encryption (fonctions ESP-IDF), OTA **signé**, et à terme
vérification de l'intégrité des apps installées. Détail : [`SECURITY.md`](https://github.com/MiiK4L/companion-platform/blob/main/SECURITY.md).

# `hardware/` — Matériel officiel Companion Platform

Ce dossier contient la conception matérielle **officielle** de la carte
mère (mainboard) Companion Platform, basée sur le module **Seeed XIAO
ESP32-S3** : schémas, PCB, nomenclature (BOM) et fichiers de fabrication
(Gerbers).

## Organisation

Chaque révision de carte vit dans son propre sous-dossier versionné :

```
hardware/
  mainboard-v1/
    kicad/          # projet KiCad (schémas, PCB)
    production/     # Gerbers, BOM, fichiers de fabrication export
```

## Ce qui va ici

- Les projets **KiCad** (schémas, empreintes, PCB) de la carte mère.
- Les nomenclatures (BOM) et fichiers d'export pour la fabrication
  (Gerbers, fichiers de perçage, fichiers d'assemblage).
- La documentation de conception propre à une révision matérielle
  (choix de composants, contraintes mécaniques, notes de test).

## Ce qui ne va pas ici

- **Aucun capteur ni périphérique applicatif** n'a sa place sur la carte
  mère : la mainboard expose uniquement le socle (alimentation, ESP32-S3,
  écran TFT/LVGL, connecteur(s) CX-Bus). Tout capteur, actionneur ou
  extension applicative est un **module CX-Bus** — voir `modules/`.
- Le boîtier et ses sources CAO : voir `case/`.
- Le firmware qui pilote cette carte : voir `firmware/`.

## Licence

Ce dossier est publié sous **CERN-OHL-S-2.0** (CERN Open Hardware Licence
Version 2 — Strongly Reciprocal), conformément à la stratégie de licences
du projet. Voir [`LICENSING.md`](../LICENSING.md) et
[`LICENSES/CERN-OHL-S-2.0.txt`](../LICENSES/CERN-OHL-S-2.0.txt).

## Phase de roadmap

`mainboard-v1` constitue la première révision matérielle de référence,
suffisante pour valider le socle (ESP32-S3, écran, CX-Bus). Les révisions
suivantes (optimisation de consommation, révisions de connecteur CX-Bus,
variantes) seront ajoutées en sous-dossiers `mainboard-v2/`, etc., au fur
et à mesure des phases ultérieures de la roadmap. Consultez
`docs/roadmap.md` pour le calendrier détaillé.

# `modules/` — Modules CX-Bus

Ce dossier contient les **modules d'extension** de la plateforme,
connectables via le **CX-Bus (Companion eXpansion Bus)**, le standard
d'extension matérielle de Companion Platform (voir ADR-0005/0006 et
`standards/cx-bus/`). C'est ici que vivent les capteurs, actionneurs et
extensions applicatives que la mainboard elle-même n'embarque pas.

## Organisation

Chaque module vit dans son propre sous-dossier, nommé d'après le module :

```
modules/
  <nom-du-module>/
    hardware/     # schémas, PCB, BOM du module (KiCad)
    firmware/     # firmware/driver spécifique au module, au-dessus de la HAL
    docs/          # documentation du module (fiche technique, manifest)
```

## Ce qui va ici

- Un dossier par module, respectant le **standard CX-Bus** : mécanique de
  connecteur, identification via EEPROM (manifest CX-Bus), protocole de
  communication.
- Pour chaque module : sa documentation, ses sources KiCad et son
  firmware/driver associé, afin qu'un module soit auto-suffisant et
  reproductible.

## Ce qui ne va pas ici

- Le matériel de la carte mère elle-même : voir `hardware/`.
- Les apps Lua qui *utilisent* les capacités exposées par un module : voir
  `apps/` (une app consomme un module via le Companion SDK, elle n'en fait
  pas partie).
- Un module qui ne respecte pas le standard CX-Bus (mécanique, EEPROM
  d'identification) n'est pas un module CX-Bus au sens de ce dossier.

## Licence

Le matériel de chaque module est publié sous **CERN-OHL-S-2.0**, et le
firmware/driver associé sous **Apache-2.0**, conformément à la stratégie
générale du projet — voir [`LICENSING.md`](../LICENSING.md). Les modules
**tiers** peuvent être documentés ou référencés ici tout en conservant
leur propre licence, à condition de la déclarer explicitement (en-tête
SPDX, fichier `LICENSE` local).

## Phase de roadmap

Ce dossier reste vide tant que le standard CX-Bus (`standards/cx-bus/`)
et la première révision de mainboard exposant un connecteur CX-Bus ne
sont pas stabilisés. Les premiers modules de référence (par exemple un
module capteur simple) seront ajoutés une fois cette base posée, en
phase intermédiaire de la roadmap. Consultez `docs/roadmap.md` et
`docs/guides/module-development.md` pour le détail.

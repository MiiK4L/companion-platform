# `case/` — Boîtier Companion Platform

Ce dossier contient la conception du **boîtier imprimable en 3D** de
Companion Platform : sources CAO paramétriques et fichiers STL prêts à
imprimer.

## Organisation

```
case/
  <révision>/
    freecad/   ou   fusion/    # sources CAO paramétriques
    stl/                        # exports STL prêts à l'impression
```

(La structure précise par outil CAO — FreeCAD et/ou Fusion 360 — se
stabilisera à mesure que la conception avance ; ce README sera mis à jour
en conséquence.)

## Ce qui va ici

- Les sources CAO paramétriques du boîtier (**FreeCAD** et/ou
  **Fusion 360**), afin que le boîtier reste modifiable et pas seulement
  consultable.
- Les exports **STL** correspondants, prêts à trancher pour impression
  3D.
- Toute note de conception nécessaire à la co-conception avec le PCB :
  emplacements de vis, découpes pour connecteurs, tolérances d'ajustement
  avec `hardware/`.

## Ce qui ne va pas ici

- Les sources du PCB lui-même : voir `hardware/`.
- Les modules CX-Bus et leur propre boîtier/mécanique éventuelle (si un
  module nécessite une extension de boîtier, cette extension vit avec le
  module concerné dans `modules/`).

## Point d'attention : co-conception avec le PCB

Le boîtier est **co-conçu avec le PCB** de la mainboard : toute
modification du placement des connecteurs, de l'écran ou des points de
fixation sur `hardware/mainboard-v*/` doit être répercutée ici, et
inversement. Une PR qui modifie l'un sans considérer l'autre est
incomplète.

## Licence

Ce dossier est publié sous **CERN-OHL-S-2.0**, au même titre que le reste
du matériel officiel — voir [`LICENSING.md`](../LICENSING.md) et
[`LICENSES/CERN-OHL-S-2.0.txt`](../LICENSES/CERN-OHL-S-2.0.txt).

## Phase de roadmap

Ce dossier se remplit une fois la première révision de mainboard
(`hardware/mainboard-v1/`) suffisamment stable pour figer les
emplacements mécaniques critiques (connecteurs, écran, boutons). Consultez
`docs/roadmap.md` pour la phase concernée.

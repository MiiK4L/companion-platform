<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Standard CX-Bus

Le **CX-Bus** (*Companion eXpansion Bus*) est le standard d'extension matérielle de Companion Platform. Il définit comment un module d'extension (capteur, écran, connectivité, etc.) se déclare, s'identifie et communique avec la carte principale, via un **mécanisme d'identification standardisé** (le support d'identification est un mécanisme **candidat** : une **EEPROM I²C** embarquée est l'option pressentie, voir ADR-0005 et ADR-0006).

Cette page est une **passerelle volontairement minimale** : la spécification complète, versionnée et détaillée du standard CX-Bus vit dans le dépôt, en dehors de l'arborescence `docs/`, afin de rester proche des sources techniques (schémas, format du Manifest, exemples de modules) qu'elle décrit.

Vous trouverez la spécification complète dans le dossier :

```
standards/cx-bus/
```

## À consulter également

- [Vue d'ensemble de l'architecture](../architecture/overview.md)
- [Inversion des dépendances matérielles](../architecture/dependency-inversion.md)
- [Décisions d'architecture (ADR)](../adr/README.md) — en particulier ADR-0005 et ADR-0006
- [Développer un module CX-Bus](../guides/module-development.md)

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# App Tamagotchi — Application de référence

> Statut : **Phase 0 — Fondations**. La conception détaillée est marquée « ⏳ À définir —
> Phase 9 ».

Le **Tamagotchi** est l'**application de référence** de la Companion Platform : la première app
d'un catalogue destiné à s'étoffer. Elle sert de démonstrateur du modèle applicatif.

Références de décision : ADR-0002, ADR-0009.

## Nature de l'app

- **App scriptée indépendante du firmware.** Le Tamagotchi est une app scriptée (langage
  **Lua** candidat en V1) ; il ne fait **jamais** partie du noyau ni des services. Il vit
  dans `apps/`.
- **Consomme uniquement le Companion SDK.** L'app n'accède ni à ESP-IDF, ni à FreeRTOS, ni à la
  HAL, ni aux services en direct — seulement au **Companion SDK** (voir
  [`firmware/companion-sdk/`](../../firmware/companion-sdk/README.md)).
- **Distincte du service Companion.** Le personnage-système (service Companion) est transverse ;
  le Tamagotchi est une app à part entière qui a sa propre logique de créature.

## Attributs du personnage

Le personnage du Tamagotchi porte notamment les attributs suivants (modèle à préciser en
Phase 9) :

- Faim
- Énergie
- Santé
- Humeur
- Propreté
- Expérience
- Niveau
- Âge
- Inventaire
- Objets
- Évolutions

## Évolution hors tension

Le Tamagotchi **évolue même lorsque l'appareil est éteint** — c'est un principe acquis. Au
réveil, l'app calcule le **delta de temps écoulé** à partir de la base de temps et de l'état
persistant, puis met à jour les attributs en conséquence (ADR-0009). La base de temps s'appuie
sur un **RTC externe candidat** (PCF8563 pressenti). Ce mécanisme repose sur les services de
temps et de persistance exposés par le SDK ; l'app ne pilote pas le RTC directement.

## Règles de dépendance

- L'app dépend **exclusivement** du Companion SDK. Aucune dépendance vers le firmware bas
  niveau.
- Toute fonctionnalité matérielle (affichage, entrées, stockage, temps) est obtenue via le SDK.

> ⏳ **À définir — Phase 9** : modèle de données complet des attributs, courbes d'évolution,
> arbres d'évolution, économie d'objets/inventaire, boucle de jeu, intégration avec des
> modules CX-Bus, assets graphiques.

## Licence

Firmware et apps officiels sous **Apache-2.0**. Documentation sous **CC-BY-4.0**.

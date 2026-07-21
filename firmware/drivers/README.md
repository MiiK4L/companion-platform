# `drivers/` — Adaptateurs cible & drivers concrets

> Statut : **Phase 0 — Fondations**. La liste et les détails d'implémentation sont marqués
> « ⏳ À définir » selon le choix final des composants.

Ce dossier contient les **adaptateurs cible** et les **drivers concrets** : les implémentations
des **ports** ([`hal/`](../hal/README.md)) pour la cible **ESP32-S3 / ESP-IDF** et pour les
composants matériels. C'est ici que vit la connaissance d'un contrôleur d'écran, d'un RTC ou
d'une jauge donnés. Voir le modèle ports/adaptateurs
([dependency-inversion](../../docs/architecture/dependency-inversion.md)).

Références de décision : ADR-0003, ADR-0004, ADR-0009.

## Contenu

Adaptateurs/drivers implémentant les ports pour les composants candidats (choix final en
Phase 1, et distinct des composants de **prototype**) :

- **Contrôleurs d'écran** : ST7789 / GC9A01 / ILI9341 (candidats, ADR-0003).
- **RTC** : PCF8563 (candidat, ADR-0009).
- **Accéléromètre** : détection de mouvement / réveil.
- **Jauge de batterie** : MAX17048 / LC709203F (candidats).
- **GPIO expander I²C** : PCA9555 / TCA9535 (candidats).
- **EEPROM CX-Bus** : lecture du CX-Bus Manifest des modules (mécanisme à valider, ADR-0005).

## Règles de dépendance

- Les adaptateurs/drivers **dépendent des ports qu'ils implémentent** (`hal/`) — c'est le sens
  correct de l'inversion : le détail dépend de l'abstraction (règle 4 du modèle).
- Ils sont, **avec ESP-IDF/FreeRTOS**, la partie du firmware autorisée à connaître le silicium
  (ADR-0007).
- Ils **NE SONT PAS** appelés directement par les services, le SDK ou les apps : ces derniers
  passent par les **ports**. Le **point de composition** (dans `kernel/`) choisit l'adaptateur
  concret au démarrage et l'injecte derrière le port.
- Remplacer un composant (ex. un contrôleur d'écran) se fait en **ajoutant/changeant un
  adaptateur**, sans toucher aux ports ni aux couches supérieures — dans les limites du contrat
  du port (un changement peut néanmoins impliquer des différences électriques/mécaniques/de
  performance, non couvertes par le logiciel).

> ⏳ **À définir — Phase 1+** : composants de production retenus, matrice adaptateur ↔ port,
> gestion des variantes d'écran, configuration au démarrage.

## Licence

Firmware sous **Apache-2.0**. Documentation sous **CC-BY-4.0**.

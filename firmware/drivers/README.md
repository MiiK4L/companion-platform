# Drivers — Pilotes matériels concrets

> Statut : **Phase 0 — Fondations**. La liste et les détails d'implémentation sont marqués
> « ⏳ À définir » selon le choix final des composants.

Les **drivers** sont les pilotes concrets des composants matériels. Chaque driver
**implémente une ou plusieurs interfaces de la HAL** pour un composant précis. C'est ici que
vit la connaissance d'un contrôleur d'écran, d'un RTC ou d'une jauge donnés.

Références de décision : ADR-0003, ADR-0004, ADR-0009.

## Responsabilités

Fournir des implémentations d'interfaces HAL pour les composants de la carte mère et des
modules CX-Bus. Composants candidats (choix final en Phase 1) :

- **Contrôleurs d'écran** : ST7789 / GC9A01 / ILI9341 (interchangeables sous LVGL, ADR-0003).
- **RTC** : PCF8563 (temps réel basse consommation, base du delta au réveil — ADR-0009).
- **Accéléromètre** : détection de mouvement / réveil.
- **Jauge de batterie** : MAX17048 / LC709203F.
- **GPIO expander I²C** : PCA9555 / TCA9535 (extension du budget GPIO, boutons).
- **EEPROM CX-Bus** : lecture du CX-Bus Manifest des modules (I²C).

## Règles de dépendance

- Les drivers dépendent de la **HAL** (dont ils implémentent les interfaces) et de la
  fondation (**ESP-IDF/FreeRTOS**), qu'ils sont — avec la HAL — les seuls autorisés à
  connaître (ADR-0007).
- Un driver **NE DOIT PAS** être appelé directement par le kernel, les services, le SDK ou
  les apps : ces couches passent toujours par l'**interface HAL** correspondante.
- Remplacer un composant (ex. un contrôleur d'écran par un autre) se fait en **ajoutant/
  changeant un driver**, sans toucher aux couches supérieures — c'est le bénéfice de
  l'inversion des dépendances.

> ⏳ **À définir — Phase 1+** : liste définitive des composants, matrice driver ↔ interface
> HAL, gestion des variantes de contrôleur d'écran, stratégie de configuration au démarrage.

## Licence

Firmware sous **Apache-2.0**. Documentation sous **CC-BY-4.0**.

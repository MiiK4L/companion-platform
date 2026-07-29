<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# Board `xiao_esp32s3` — rôle HÔTE du banc (squelette)

Rôle : **hôte** du banc (maître SPI + orchestrateur de profil + lecture des
compteurs). **Squelette documenté** : réutilise le cœur portable ; seule la
HAL/BSP ESP32-S3 (`hal/esp32/`) est à implémenter localement (ESP-IDF).

> **Statut : squelette — non compilé ni flashé ici.** Fourni pour préserver la
> **portabilité** (le cœur reste indépendant de la cible). L'implémentation
> fonctionnelle sera complétée au besoin, en local. Aucun contenu L1.

Changer de cible ne touche que `hal/<cible>/` + `boards/<carte>/` ; le cœur
portable (`portable/`) est inchangé.

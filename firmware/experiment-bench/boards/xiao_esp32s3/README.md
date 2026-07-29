<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# Board `xiao_esp32s3` — rôle HÔTE du banc

Rôle : **hôte** (maître SPI + orchestrateur de scénario + lecture des compteurs).
La board application (`main.c`) ne contient **aucune logique** : elle câble la
HAL hôte (`hal/esp32/`, qui implémente le port `bench_spi_master_t` et lit
l'IRQ) au **moteur portable** (`portable/engine/`), puis exécute le **scénario
partagé** avec l'esclave.

## Statut réel

**Contrat et squelette fournis** ; **implémentation matérielle NON réalisée/
validée** ici : `hal/esp32/` ne contient que des **stubs** (`TODO`, sans
ESP-IDF). Le câblage `main.c` est **compilé** contre l'API portable en CI
(anti-dérive), mais **non compilé avec ESP-IDF, ni flashé, ni exécuté**.

## Portabilité

Changer de cible ne touche que `hal/<cible>/` + `boards/<carte>/` ; le cœur
portable (`portable/`) est inchangé. Aucun contenu de campagne L1.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# Board `rp2040_reference` — rôle ESCLAVE (simulateur de module CX-Bus)

Rôle : **esclave** du banc — un simulateur de module CX-Bus **contrôlé** (propre
CS, trafic répétable, fréquence/tailles configurables, IRQ vers l'hôte, contrôle
d'intégrité par CRC). La board application (`main.c`) ne contient **aucune
logique** : elle câble les primitives de la HAL (`hal/rp2040/`) au **port
ISpiSlave** fourni par le **moteur portable** (`portable/engine/`), lequel décode
les trames, vérifie le CRC, met à jour les compteurs et lève l'IRQ.

## Statut réel

**RP2040 = cible candidate de référence.** **Contrat et squelette d'intégration
fournis** ; **implémentation matérielle NON réalisée et NON validée** ici :

- `hal/rp2040/` ne contient que des **stubs** (`TODO`), sans Pico SDK ni PIO ;
- le câblage `main.c` est **compilé** contre l'API portable en CI (anti-dérive),
  mais **non compilé avec le Pico SDK, ni flashé, ni exécuté sur matériel**.

Les prérequis avant toute mesure (compilation Pico SDK, PIO SPI-esclave, boucle
locale/hôte, IRQ sortante vérifiée, trames CRC réellement échangées) sont listés
dans [`docs/target-comparison.md`](../../docs/target-comparison.md).

## Portabilité

Changer de cible ne touche que `hal/<cible>/` + `boards/<carte>/` ; le cœur
portable (`portable/`) est inchangé. Aucun contenu de campagne L1 ici.

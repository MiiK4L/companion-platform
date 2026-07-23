<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2A — Définitions instrumentées des événements (partagé)

> **Statut : Brouillon.** Référentiel **commun** aux
> [protocoles](README.md) : chaque événement compté comme erreur doit être
> **observable et mesurable**. Les valeurs `[BL]` sont **finalisées au
> baselining** (voir le [cycle de vie](README.md)). Aucune valeur mesurée `[M]`.

Un critère « 0 X » n'a de sens que si **X est défini de façon instrumentée**. Le
tableau ci-dessous fait foi ; chaque protocole peut préciser des seuils propres,
sans contredire ces définitions.

## Événements et leur définition instrumentée

| Événement | Définition instrumentée | Observable / capteur | Seuil |
|-----------|-------------------------|----------------------|-------|
| **Reset Host** | Redémarrage non sollicité **ou** cause de reset anormale **ou** heartbeat manquant | Compteur de boot persistant + **cause de reset** (registre) + heartbeat périodique (GPIO/log) | 0 événement |
| **Corruption écran** | Divergence du contenu affiché **ou** erreur de transaction d'affichage | **CRC du framebuffer** vs image de référence, **et** compteur d'erreurs de transaction SPI écran | 0 hors spec |
| **Blocage bus (stuck)** | Ligne `SDA`/`SCL` maintenue basse au-delà d'une durée | Analyseur logique + horodatage ; durée mesurée | > **[BL]** ms = blocage |
| **Latch-up** | Courant d'alim > limite pendant une durée | Sonde de courant sur le rail concerné + fenêtre temporelle | I > **[BL]** pendant > **[BL]** µs |
| **Réveil parasite** (module `VMOD` coupé) | Tension rail module, **ou** courant, **ou** événement logiciel dépassant une limite | Tension `VMOD` résiduelle + courant de fuite + indicateur firmware module | V/I > **[BL]** |
| **État connu** (reprise) | Liste **précise** des états matériels et logiciels attendus | Voir §« État connu » ci-dessous | 100 % conforme |

## État « connu » (reprise après incident)

Un retour à l'« état connu » **doit** vérifier **tous** les points suivants (liste
à figer au baselining, valeurs `[BL]`) :

- **Matériel** : `VMOD` = coupé ; lignes de bus = haute-Z ; pas de courant de
  fuite > `[BL]` ; température nominale.
- **Logiciel** : contrôleurs I²C/SPI Host réinitialisés et **non bloqués** ;
  pilotes en état « idle » ; pas de transaction pendante ; heartbeat actif ;
  journal cohérent (pas d'erreur non traitée).

## Stockage & corrélation des compteurs

- **Où** : compteurs firmware (boot, resets, erreurs de transaction, CRC écran)
  journalisés via UART/log persistant, horodatés sur une **base de temps commune**
  aux instruments (oscilloscope + analyseur logique).
- **Corrélation** : chaque événement firmware porte un **timestamp** aligné sur un
  **signal de synchro** (marqueur GPIO capturé simultanément par l'oscilloscope et
  l'analyseur logique) afin de recouper compteur ↔ trace analogique/logique.
- **Données brutes** : captures + journaux conservés dans le dossier du lot ;
  **hash SHA-256 par fichier** consigné au rapport.

## Renvois

- [Index des protocoles & cycle de vie](README.md)
- [Bus-stuck & retrait en transaction](../bus-stuck-and-transaction.md)
- [Cadre de validation](../../validation-framework.md)

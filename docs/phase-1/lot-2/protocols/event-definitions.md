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
| **Corruption logique de l'affichage** | Framebuffer **ou** transactions d'affichage incorrects | **CRC du framebuffer** + compteur d'erreurs de transaction SPI écran | 0 |
| **Anomalie de sortie affichée** | Contenu **visible** ≠ référence : écran figé, noir, reset, perte de synchro | **≥ 1 méthode de vérification de sortie** (voir §« Anomalie de sortie ») — le **CRC framebuffer seul ne suffit pas** | 0 |
| **Blocage bus (stuck)** | Ligne `SDA`/`SCL` maintenue basse au-delà d'une durée | Analyseur logique + horodatage ; durée mesurée | > **[BL]** ms = blocage |
| **Surcourant persistant / conduction anormale** | Courant d'alim > limite pendant une durée (**directement observable**) | Sonde de courant sur le rail concerné + fenêtre temporelle | I > **[BL]** pendant > **[BL]** µs |
| **Latch-up (suspecté)** | Conduction anormale qui **persiste après suppression du stimulus** et **n'est levée que par une coupure d'alimentation** | Sonde de courant + séquence « stimulus off → observation → power-cycle » | conduction persistante levée **uniquement** par power-cycle |
| **Réveil parasite** (module `VMOD` coupé) | Tension rail module, **ou** courant, **ou** événement logiciel dépassant une limite | Tension `VMOD` résiduelle + courant de fuite + indicateur firmware module | V/I > **[BL]** |
| **État connu** (reprise) | Liste **précise** des états matériels et logiciels attendus | Voir §« État connu » ci-dessous | 100 % conforme |

## Anomalie de sortie affichée — méthodes de vérification (baseline)

Un **CRC de framebuffer correct** et un **compteur d'erreurs SPI nul** peuvent
coexister avec une **image réellement erronée** (contrôleur ou dalle perturbés
par un transitoire électrique). Détecter une **anomalie de sortie** exige donc
d'observer la **sortie visible**, pas seulement l'état interne. Au baselining,
retenir **au moins une** des méthodes suivantes (choix justifié) :

- **Capture caméra** synchronisée, comparée à une **image de référence** ;
- **Retour d'état du contrôleur** d'affichage (registres de statut), si disponible ;
- **Mire périodique** avec contrôle externe (photodiode / caméra / capteur) ;
- **Autre mécanisme justifié** (documenté au protocole).

> Interdit : conclure « corruption/glitch écran = 0 » sur la seule base du **CRC
> framebuffer** + compteur SPI. La corruption **logique** et l'anomalie de
> **sortie** sont deux événements distincts, comptés séparément.

## Surcourant persistant vs latch-up (à ne pas confondre)

- **Surcourant persistant / conduction anormale** est le critère **directement
  observable** (courant > seuil pendant une durée) et sert de **déclencheur**
  (arrêt immédiat, comptage).
- **Latch-up** n'est **retenu (suspecté)** que si la conduction anormale
  **persiste après suppression du stimulus** et **ne disparaît qu'après une
  coupure d'alimentation**. Un simple dépassement de courant **ne prouve pas** un
  latch-up.
- Procédure d'attribution : sur surcourant, **couper le stimulus** ; si la
  conduction persiste et n'est levée **que** par power-cycle → **latch-up
  suspecté** (à confirmer, hors périmètre documentaire).

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

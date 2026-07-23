<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 1 — Budget GPIO / bus (allocation de référence, *Proposé*)

> **Statut : Proposé — question `DEC-L1-001`.** L'allocation ci-dessous est une
> **référence de conception établie sur documentation** ; elle **reste Proposé**
> jusqu'à sa **validation sur banc de bring-up en L2** (aucune ADR à ce stade).
> Les numéros de GPIO exacts sont **à confirmer sur la fiche officielle Seeed**
> de la révision utilisée (analyse documentaire).

## 1. Ressource : broches exposées par le candidat de référence (XIAO ESP32-S3)

Le XIAO ESP32-S3 expose **11 broches d'E/S** (D0–D10) sur ses bords, plus
alimentation (5V, 3V3, GND) et pads batterie (B+/B-). Rôles de bus **par
défaut** (convention Seeed — numéros GPIO à confirmer) :

| Broche | Rôle de bus par défaut | Remarque |
|--------|------------------------|----------|
| D0–D3 | GPIO polyvalents (ADC) | 4 lignes libres |
| D4 | I²C `SDA` | bus lent partagé |
| D5 | I²C `SCL` | bus lent partagé |
| D6 | UART `TX` | debug/console |
| D7 | UART `RX` | debug/console |
| D8 | SPI `SCK` | bus rapide |
| D9 | SPI `MISO` | bus rapide |
| D10 | SPI `MOSI` | bus rapide |

Atouts du candidat (à confirmer/mesurer plus tard) : USB-C natif, Wi-Fi/BT,
8 Mo flash + 8 Mo PSRAM (utile pour le moteur graphique — à valider en L4),
charge LiPo embarquée. **Le deep-sleep réel reste à mesurer (L5).**

## 2. Demande : signaux à câbler (issus de la [matrice des exigences](../requirements-matrix.md))

Écran SPI (SCK, MOSI, MISO + **CS, DC, RST**, éventuel **backlight PWM**) ·
boutons (n à confirmer) · buzzer · vibreur · IRQ accéléromètre · IRQ module ·
enable alim module · détection présence module · **bus CX-Bus** (I²C + SPI
partagés + IRQ) · support d'identification (I²C).

## 3. Confrontation : le direct ne tient pas

En câblage **direct**, I²C (2) + SPI (3) + UART (2) consomment déjà 7 des 11
broches ; il ne reste que **D0–D3 (4 lignes)** pour : écran DC/CS/RST/backlight
(≈ 4) **et** boutons, buzzer, vibreur, 2× IRQ, enable, détection, CS module…
→ **impossible en direct**. *(Ce constat tient quels que soient les numéros GPIO
exacts.)*

## 4. Allocation de référence proposée (*Proposé*, à valider en L2)

- **I²C (D4/D5)** — bus partagé : RTC, accéléromètre, jauge, **support
  d'identification** du module, **et un GPIO expander I²C** (candidat) portant
  boutons, enable, détection, et lignes auxiliaires.
- **SPI (D8/D9/D10)** — **partagé écran + CX-Bus**, chip-selects distincts (via
  l'expander si nécessaire) ; arbitrage/contention à valider (**R2**, L2/L4).
- **UART (D6/D7)** — console/debug (optionnellement réaffectable).
- **D0–D3** — réservées : DC/RST écran, backlight (PWM), une **IRQ** directe
  (latence), au choix selon le résultat L2.
- **Boutons** : via expander **ou** échelle de boutons sur **une entrée ADC**
  (option sans puce, à comparer — coût global évalué en arbitrage, pas imposé).

**Conséquence** : un **GPIO expander I²C** (ou une échelle ADC pour les boutons)
est un **candidat fortement pressenti** pour tenir le budget — **sans être
imposé** : son coût global (composant, IRQ, latence, conso) est comparé lors de
l'arbitrage (voir [déclencheurs d'arbitrage](../requirements-matrix.md)).

## 5. Ce que ce lot NE fait pas

- Ne **fige** pas le brochage ni l'expander (validation banc = **L2**).
- Ne **mesure** rien (contention SPI, latences, conso = L2/L4/L5).
- Ne crée **aucune ADR** : `DEC-L1-001` reste **Ouvert**.

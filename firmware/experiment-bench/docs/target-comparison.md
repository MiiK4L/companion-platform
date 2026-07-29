<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# Comparaison de cible — MCU esclave (simulateur de module CX-Bus)

> **Choix de banc expérimental, réversible. PAS une ADR, PAS une décision
> produit.** Le cœur portable est **indépendant** de la cible ; migrer ne
> réécrit que `hal/<cible>/` + `boards/<carte>/`.

## Critères objectifs

| Critère | RP2040 | ESP32-S3 (2ᵉ exemplaire) |
|--------|--------|--------------------------|
| Esclave SPI déterministe | **PIO** dédié, très adapté | possible, moins naturel |
| Déterminisme temporel | élevé (pas de Wi-Fi/BT) | perturbable (radio) |
| Génération de profils | simple (cœur portable) | idem |
| Gestion des IRQ | fine (PIO/GPIO) | correcte |
| Outils de développement | Pico SDK matûr | ESP-IDF matûr |
| Simplicité du banc | rôle **distinct** du DUT | risque de confusion hôte/esclave |
| Coût / disponibilité | faible / large | faible / large |
| Maintenabilité | 2ᵉ toolchain à gérer | 1 seule toolchain |
| Impact build | +1 toolchain (Pico SDK) | aucune (ESP-IDF déjà présent) |

## Cible retenue : **RP2040** — statut réel

**RP2040 = cible candidate de référence.** Le **contrat** (ports maître/esclave,
IRQ directionnelle) et le **squelette d'intégration** (HAL + board application)
sont fournis. **L'implémentation matérielle n'est PAS réalisée et n'est PAS
validée** ici : les primitives de `hal/rp2040/` sont des **stubs** (`TODO`), non
compilées avec le Pico SDK ni flashées.

Justification du choix : le **PIO** permet un esclave SPI déterministe de grande
précision, sans perturbation radio ; un cœur dédié distinct du DUT ESP32-S3
évite de brouiller les rôles. Le coût (2ᵉ toolchain) est **assumé** et cantonné à
`hal/rp2040` + `boards/rp2040_reference`.

**Réversibilité** : si les essais montrent qu'une autre plateforme convient
mieux, seul le couple `hal/` + `boards/` change — décision revue **sur critères
techniques**, jamais gravée dans l'architecture ni transformée en ADR.

## Prérequis AVANT toute mesure (gate de B3)

B3 ne pourra **pas** prétendre démarrer des mesures avant d'avoir, en local et de
façon vérifiée :

1. une **compilation Pico SDK** réussie du board esclave ;
2. un **PIO SPI-esclave** implémenté (réception/émission déterministes) ;
3. un **test de boucle** locale ou avec l'hôte (trafic aller-retour) ;
4. l'**IRQ sortante** vérifiée (assertion/relâchement observés) ;
5. des **trames CRC réellement échangées** sur le lien matériel.

Tant que ces cinq points ne sont pas satisfaits et documentés (`build_manifest`),
aucune donnée `[M]` ne peut être produite.

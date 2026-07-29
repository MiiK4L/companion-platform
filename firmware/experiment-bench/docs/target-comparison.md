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

## Cible retenue : **RP2040**

Justification : le **PIO** permet un **esclave SPI déterministe** de grande
précision, sans perturbation radio ; un **cœur dédié** distinct du DUT ESP32-S3
évite de brouiller les rôles. Le coût (2ᵉ toolchain) est **assumé** et cantonné à
`hal/rp2040` + `boards/rp2040_reference`.

**Réversibilité** : si les essais montrent qu'une autre plateforme convient
mieux, seul le couple `hal/` + `boards/` change — décision revue **sur critères
techniques**, jamais gravée dans l'architecture ni transformée en ADR.

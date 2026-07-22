<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Standard CX-Bus — Companion eXpansion Bus

> Statut : **Phase 0 — Fondations** · Version courante du standard : **CX-Bus 1.0 (BROUILLON)**

Le **CX-Bus** (Companion eXpansion Bus) est le standard matériel et logique d'extension
de la **Companion Platform**. Il définit la manière dont des modules d'extension
interchangeables se connectent, s'identifient et communiquent avec la carte mère.

> ⚠️ **Précision de nomenclature.** Le projet s'appelle **Companion Platform**.
> **CX-Bus** désigne *uniquement* le standard d'extension, jamais le projet dans son ensemble.

## Philosophie : « console à cartouches »

La Companion Platform est conçue comme une console de poche durable dont les capacités
s'étendent par des **cartouches** matérielles. Le CX-Bus s'inspire directement du modèle
éprouvé des consoles à cartouches : un socle stable (le **Host**) et un catalogue ouvert
de modules interchangeables (les **Modules**), chacun capable de se décrire lui-même et
d'exposer une ou plusieurs applications.

Objectifs directeurs :

- **Interchangeabilité** : un module se branche et se retire sans reconfiguration manuelle.
- **Auto-description** : chaque module porte son propre descripteur d'identification
  (le **CX-Bus Manifest**), sur le modèle de la spec EEPROM des HAT Raspberry Pi.
- **Ouverture** : un tiers doit pouvoir concevoir un module conforme sans autorisation
  centrale, en s'appuyant sur cette spécification publique.
- **Durabilité** : compatibilité ascendante préservée dans le temps (voir ci-dessous).

Ces choix découlent des décisions verrouillées **ADR-0005** et **ADR-0006**.

## Nomenclature versionnée

| Terme | Définition |
|-------|-----------|
| **CX-Bus 1.0** | Version du *standard* (spécification). Versionnée en SemVer. |
| **CX-Bus Module** | La cartouche : carte d'extension physique conforme au standard. |
| **CX-Bus Host** | La carte mère qui expose le connecteur et pilote le bus. |
| **CX-Bus Manifest** | Le descripteur d'identification porté par le Module via son support d'identification (mécanisme normatif à définir en Phase 1 ; EEPROM I²C = option privilégiée à évaluer). |
| **CX-Bus SDK** | La surface logicielle permettant d'écrire le firmware d'un Module et de l'intégrer côté Host. |

## Versionnement du standard

Le standard CX-Bus suit le **versionnement sémantique (SemVer)** : `MAJEUR.MINEUR.CORRECTIF`.

- **MAJEUR** : rupture de compatibilité (brochage, mécanique ou protocole incompatibles).
- **MINEUR** : ajout rétrocompatible (nouvelle capacité optionnelle, nouveau champ Manifest facultatif).
- **CORRECTIF** : clarification rédactionnelle sans impact technique.

Chaque version publiée du standard est marquée par un **tag Git** préfixé `spec-`
(ex. `spec-1.0.0`). Le préfixe distingue les versions de la *spécification* des
versions du firmware ou du matériel.

### Compatibilité ascendante

Un objectif central du CX-Bus est la **compatibilité ascendante** : un Module conçu
pour une version antérieure du standard doit rester utilisable sur un Host plus récent
partageant la même version MAJEURE. Les règles détaillées figurent au chapitre 10 de la
spécification.

## Index des documents

| Document | Objet |
|----------|-------|
| [`SPEC-CX-Bus-1.0.md`](./SPEC-CX-Bus-1.0.md) | Spécification technique du standard (BROUILLON). |
| [`cx-bus-manifest.md`](./cx-bus-manifest.md) | Spécification du descripteur d'identification (Manifest porté par le support d'identification). |

## Licence

La documentation du standard CX-Bus est publiée sous **CC-BY-4.0**.
Les implémentations matérielles officielles (`hardware/`, `modules/`, `case/`) relèvent de
**CERN-OHL-S-2.0** ; le firmware relève d'**Apache-2.0**. Un auteur tiers reste libre de
la licence de son propre module (voir `modules/TEMPLATE-module/`).

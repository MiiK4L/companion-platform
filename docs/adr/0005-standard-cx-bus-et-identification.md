# ADR-0005 : Standard d'extension CX-Bus et identification par EEPROM

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideur** : Mainteneur du projet (MiiK4L)
- **Expertises consultées** : Architecte système, architecte électronique, expert protocoles
- **Phase de roadmap** : 0 (spécification détaillée en Phase 1)
- **Domaines impactés** : hardware, cx-bus, os, modules
- **Tags** : cx-bus, modularité, identification, eeprom, hot-plug

## 1. Contexte et problème

La modularité « type console » est la partie la plus importante du projet : un
connecteur unique, une interface identique pour tous les modules, une détection
et une identification automatiques, puis le chargement des apps correspondantes.
Il faut un **standard matériel et protocolaire** stable, capable d'accueillir
des modules très divers (GPS, CO₂, réseau, électronique…) pendant des années,
sans que la carte mère embarque aucun capteur applicatif.

## 2. Options étudiées

### Mécanisme d'identification
#### Option A — EEPROM I²C par module (descripteur normalisé)
- **Avantages** : éprouvé (spec EEPROM des HAT Raspberry Pi comme antériorité),
  coût dérisoire, lecture simple sur le bus I²C partagé, descripteur riche
  (nom, version, auteur, capacités, apps exposées).
- **Inconvénients** : quelques octets de flash et une puce à provisionner.

#### Option B — Résistances de détection / codage analogique (ID par ADC)
- **Avantages** : ultra simple, pas de puce.
- **Inconvénients** : nombre d'identifiants très limité, aucune métadonnée
  riche, pas d'auteur/version/capacités. Insuffisant.

#### Option C — Négociation logicielle sur le bus (interrogation active)
- **Avantages** : flexible.
- **Inconvénients** : nécessite un MCU dans chaque module ; surcoût et
  complexité pour des modules passifs simples.

## 3. Décision

> **Portée du statut.** Seul le *principe* ci-dessous est **Accepté**. Les éléments listés en §6 (Décisions différées) sont au statut **Proposé** et seront actés par de futures ADR une fois validés par prototype.

Création du standard **CX-Bus** (voir [ADR-0006](0006-nommage-cx-bus.md) pour le
nom). Principe accepté :
- des **modules auto-identifiés** : chaque module est capable de se décrire lui-même ;
- une **interface d'extension unique** : un connecteur unique et une interface
  identique pour tous les modules ;
- support de type C (modules à MCU) possible par-dessus, sans l'imposer aux
  modules passifs.

Le mécanisme d'auto-identification pressenti est une **EEPROM I²C** par module
contenant un descripteur normalisé, le **CX-Bus Manifest** (magic, version de
protocole, vendor/product ID, version matérielle, nom, auteur, UUID, capacités,
brochage requis, liste des apps exposées, CRC). Ce mécanisme, le **format
binaire** du Manifest et la **séquence de hot-plug** détaillée ne sont **pas**
actés comme définitifs par la présente ADR : ils relèvent des décisions
différées (§6).

Le format binaire exact, le brochage, le connecteur physique et le format
mécanique sont spécifiés dans
[`standards/cx-bus/SPEC-CX-Bus-1.0.md`](../../standards/cx-bus/SPEC-CX-Bus-1.0.md).

## 4. Raisons du choix

L'EEPROM I²C offre le meilleur rapport richesse/coût/robustesse, permet des
modules passifs bon marché comme des modules actifs, et s'appuie sur une
antériorité industrielle validée (HAT RPi). C'est la base la plus pérenne pour
un écosystème de modules tiers.

## 5. Conséquences

### Positives
- Écosystème de modules extensible et auto-descriptif.
- Découplage total carte mère / capteurs applicatifs.

### Négatives / compromis acceptés
- Le bus I²C/SPI est partagé entre la carte mère et le module : arbitrage et
  budget d'adresses/broches à spécifier (contrainte des 11 GPIO du XIAO).
- Provisionnement d'EEPROM à outiller (voir `tools/`).

### Impacts futurs
- Le standard CX-Bus est versionné indépendamment (tags `spec-`), avec des
  règles de compatibilité ascendante à définir en Phase 1.
- Toute application « module » (GPS, CO₂, réseau…) s'appuie sur ce mécanisme.

## 6. Décisions différées (statut : Proposé — à valider par prototype)

Éléments au statut **Proposé**, qui feront l'objet de futures ADR une fois
validés expérimentalement — notamment tant que les **contraintes électriques**
(sûreté, isolation, back-powering) ne sont pas démontrées (cf. SPEC §7 et le lot
sûreté électrique de Phase 1) :
- l'**identification par EEPROM I²C** (CX-Bus Manifest) comme mécanisme
  définitif, et le **format binaire** du Manifest ;
- la **stratégie de hot-plug** détaillée (détection présence, alimentation
  module, lecture EEPROM, validation Manifest, chargement drivers, apparition
  des apps ; séquencement, debounce) ;
- la **famille de connecteur** physique (card-edge/board-to-board/FPC/pogo) ;
- le **brochage** définitif et les niveaux logiques ;
- le **budget de courant** par module et le power-gating ;
- le **plan d'adressage I²C** et l'arbitrage du SPI partagé.

## 7. Liens
- [ADR-0006](0006-nommage-cx-bus.md) · [ADR-0004](0004-coeur-de-calcul-socket.md) · [SPEC CX-Bus 1.0](../../standards/cx-bus/SPEC-CX-Bus-1.0.md)

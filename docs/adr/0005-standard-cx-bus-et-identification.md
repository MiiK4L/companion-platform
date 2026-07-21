# ADR-0005 : Standard d'extension CX-Bus et identification par EEPROM

- **Statut** : Accepté (principes) — le mécanisme d'**identification par EEPROM**
  et le **connecteur unique** sont actés ; **connecteur physique, brochage,
  budget de courant et stratégie de hot-plug** sont spécifiés/validés en Phase 1
  (cf. marqueurs ⏳ de la SPEC).
- **Date** : 2026-07-21
- **Décideurs** : Architecte système, architecte électronique, expert protocoles
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

Création du standard **CX-Bus** (voir [ADR-0006](0006-nommage-cx-bus.md) pour le
nom). Principes actés en Phase 0 :
- **connecteur unique** ; interface identique pour tous les modules ;
- chaque module embarque une **EEPROM I²C** contenant un descripteur normalisé,
  le **CX-Bus Manifest** (magic, version de protocole, vendor/product ID,
  version matérielle, nom, auteur, UUID, capacités, brochage requis, liste des
  apps exposées, CRC) ;
- séquence au démarrage / hot-plug : *détection présence → alimentation module →
  lecture EEPROM → validation Manifest → chargement drivers → apparition des
  apps* ;
- support de type C (modules à MCU) possible par-dessus, sans l'imposer aux
  modules passifs.

Le format binaire exact, le brochage, le connecteur physique et le format
mécanique sont **spécifiés en Phase 1** dans
[`standards/cx-bus/SPEC-CX-Bus-1.0.md`](https://github.com/MiiK4L/companion-platform/blob/main/standards/cx-bus/SPEC-CX-Bus-1.0.md).

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

## 7. Réserves — à valider en Phase 1 (non figé)

> Ce qui est **acté** : identification par **EEPROM I²C** (CX-Bus Manifest),
> connecteur **unique** et interface identique pour tous les modules. Ce qui
> **reste à trancher** (tous marqués ⏳ dans la SPEC) :
> - la **famille de connecteur** physique (card-edge/board-to-board/FPC/pogo) ;
> - le **brochage** définitif et les niveaux logiques ;
> - le **budget de courant** par module et le power-gating ;
> - la **stratégie de hot-plug** exacte (détection, séquencement, debounce) ;
> - le **plan d'adressage I²C** et l'arbitrage du SPI partagé.

## 6. Liens
- [ADR-0006](0006-nommage-cx-bus.md) · [ADR-0004](0004-coeur-de-calcul-socket.md) · [SPEC CX-Bus 1.0](https://github.com/MiiK4L/companion-platform/blob/main/standards/cx-bus/SPEC-CX-Bus-1.0.md)

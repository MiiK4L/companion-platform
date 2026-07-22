<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Architecture — vue d'ensemble

Ce document présente la vision d'architecture de **Companion Platform**. Il est
volontairement de haut niveau : les détails vivent dans les documents dédiés
([matériel](hardware.md), [logiciel/OS](software-os.md),
[inversion des dépendances](dependency-inversion.md), [glossaire](glossary.md))
et dans les [décisions d'architecture (ADR)](../adr/README.md).

## 1. Ce que nous construisons

Nous ne construisons pas « un Tamagotchi ». Nous construisons une **plateforme
électronique de poche** — matérielle et logicielle — capable d'accueillir des
dizaines d'applications pendant des années. Le Tamagotchi est la **première
application de démonstration**, pas le produit.

Objectifs directeurs (par ordre de priorité) :

> **qualité > robustesse > maintenabilité > évolutivité > documentation**,
> jamais la rapidité de développement.

## 2. Les quatre décisions fondatrices

L'architecture repose sur quatre choix structurants, chacun tracé par une ADR :

| Domaine | Décision | ADR |
|---------|----------|-----|
| Exécution des apps | Runtime **hybride extensible** (cœur natif + apps scriptées) derrière une abstraction de runtime — **Lua** candidat en V1, WASM futur | [0002](../adr/0002-modele-execution-applications.md) |
| Affichage | Écran couleur abstrait par un moteur graphique — **TFT IPS** et **LVGL** candidats | [0003](../adr/0003-technologie-et-abstraction-ecran.md) |
| Modularité | Standard d'extension **CX-Bus** + identification (mécanisme candidat, **EEPROM I²C** pressentie) | [0005](../adr/0005-standard-cx-bus-et-identification.md) / [0006](../adr/0006-nommage-cx-bus.md) |
| Fondation firmware | **ESP-IDF** confiné sous une **HAL + Companion SDK** | [0001](../adr/0001-framework-firmware-esp-idf.md) / [0007](../adr/0007-hal-et-companion-sdk.md) |

## 3. Le principe qui tient tout ensemble

Un modèle **ports / adaptateurs** (inversion des dépendances) :

```
apps → Companion SDK → services → ports abstraits ◄─ adaptateurs (ESP32-S3/ESP-IDF · host/mock)
                                                          └─ drivers · ESP-IDF · FreeRTOS · silicium
```

Les **ports** (interfaces) sont portables et ne dépendent d'aucune
implémentation ; les **adaptateurs** concrets dépendent des ports qu'ils
implémentent. Les applications ne connaissent que le **Companion SDK** (façade
fournie par les services). L'objectif : **préserver la compatibilité applicative
dans les limites du contrat garanti par le SDK** lorsqu'une pièce bas niveau
change (silicium, RTOS, écran, runtime) — un objectif de conception, pas une
garantie absolue. Voir [le détail](dependency-inversion.md).

## 4. Vue matérielle (résumé)

Une carte mère unique — le **CX-Bus Host** — ne contient que l'infrastructure
générique : cœur de calcul (XIAO ESP32-S3 **remplaçable**, non soudé définitivement — socketage pressenti à valider, [ADR-0004](../adr/0004-coeur-de-calcul-socket.md)),
alimentation/charge USB-C, écran, entrées (boutons/buzzer/vibreur), base de temps,
accéléromètre, et **un connecteur d'extension unique**. **Aucun capteur
applicatif n'est soudé** : tout capteur passe par un **module CX-Bus**. Détails :
[architecture matérielle](hardware.md).

## 5. Vue logicielle (résumé)

Le firmware se comporte comme un petit système d'exploitation, structuré en
**ports / adaptateurs** (apps → Companion SDK → services → ports ← adaptateurs →
silicium). Le **Module Manager** détecte et identifie les modules CX-Bus,
l'**App Manager** gère le cycle de vie des applications (runtime de script, Lua candidat en V1), et le
service **Companion** transforme n'importe quel état système en expression du
personnage. Détails : [architecture logicielle](software-os.md).

## 6. Modularité « type console »

Insérer un module CX-Bus déclenche : *détection → identification (lecture du
CX-Bus Manifest) → validation → **publication des capacités*** par le Module
Manager. Ensuite, l'**App Manager** peut **proposer** les apps compatibles selon
la **politique de confiance** — **aucun code n'est installé ou lancé sur la
seule foi du Manifest**. Ainsi un module GPS peut faire proposer une app GPS ; un
module CO₂ une app « qualité de l'air ». Le personnage sert d'interface : plutôt
que « CPU 80 % », il paraît fatigué ; plutôt que « serveur injoignable », il
paraît inquiet.

## 7. Où lire la suite

- [Architecture matérielle](hardware.md)
- [Architecture logicielle / OS](software-os.md)
- [Inversion des dépendances](dependency-inversion.md)
- [Glossaire](glossary.md)
- [Standard CX-Bus](../../standards/cx-bus/README.md)
- [Décisions d'architecture (ADR)](../adr/README.md)
- [Feuille de route](../roadmap.md)

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Spécification CX-Bus 1.0

> **Statut : BROUILLON / DRAFT** — Phase 0 (Fondations).
> Ce document fige l'**intention**, la **structure** et les **critères de décision** du standard.
> Les valeurs électriques, mécaniques et le brochage définitifs sont marqués « ⏳ À définir »
> avec la phase de projet où ils seront tranchés. Aucune valeur figurée ici n'est normative
> tant qu'elle porte ce marqueur.
>
> Le **gel en 1.0.0 n'interviendra qu'après satisfaction des critères de sortie** (prototypes
> de sûreté électrique, connecteur, hot-plug, endurance — voir la roadmap). D'ici là, le
> document évolue en versions **0.x** puis **1.0.0-rc**.

Références de décision : ADR-0005, ADR-0006, ADR-0008.

---

## 1. Portée et objectifs

Cette spécification définit le standard **CX-Bus** (Companion eXpansion Bus) permettant à
des **Modules** d'extension de s'interconnecter avec un **Host** (carte mère) de la
Companion Platform.

Le document couvre :

- le format mécanique et le connecteur de l'interface ;
- le brochage et les rails d'alimentation ;
- la détection de présence et le hot-plug ;
- l'identification des modules (renvoi au CX-Bus Manifest) ;
- le protocole de communication et l'arbitrage des bus partagés ;
- les règles de compatibilité entre versions ;
- les critères de conformité « CX-Bus compatible ».

**Objectifs.** Interchangeabilité, auto-description, ouverture aux tiers, durabilité et
compatibilité ascendante. **Hors périmètre :** la logique applicative des modules, l'API
du Companion SDK côté apps, et le détail des schémas électriques du Host (traités dans
`hardware/`).

**Topologie V1 : 1 Host = 1 slot CX-Bus = 1 Module simultané.** Le **chaînage**, les **hubs**
et les **Host multi-slots** sont **hors périmètre V1** (sauf décision contraire ultérieure).
En conséquence, les seules collisions à traiter en V1 sont : (a) le **module contre les
périphériques internes du Host** ; (b) **plusieurs périphériques présents sur un même module**.
Cela simplifie fortement le plan d'adressage (§9).

## 2. Terminologie

| Terme | Définition |
|-------|-----------|
| **Host** (CX-Bus Host) | Carte mère exposant le connecteur CX-Bus et pilotant le bus. Maître des bus partagés. |
| **Module** (CX-Bus Module) | Carte d'extension amovible (« cartouche ») conforme au standard. |
| **Manifest** (CX-Bus Manifest) | Descripteur d'identification normalisé porté par le Module via un **support d'identification** (mécanisme normatif à définir en Phase 1 ; EEPROM I²C = option privilégiée à évaluer). Voir `cx-bus-manifest.md`. |
| **SDK** (CX-Bus SDK) | Ensemble logiciel d'intégration d'un Module (firmware Module + intégration Host). |
| **Rail** | Ligne d'alimentation continue exposée sur le connecteur. Le rail de référence est `3V3` commuté ; `VBAT` reste un **candidat** conditionné à analyse de sécurité (voir §6). |
| **Capacité** | Fonctionnalité déclarée par un Module dans son Manifest (ex. capteur, actionneur, stockage). |

Les mots-clés « DOIT », « NE DOIT PAS », « DEVRAIT », « PEUT » sont à interpréter au sens
usuel des documents de spécification (obligation, recommandation, option).

## 3. Format mécanique ⏳

**Intention.** Définir un format physique de Module robuste, insérable/retirable à la main,
détrompé (impossible à insérer à l'envers) et retenu mécaniquement en usage nomade.

**Critères de décision :**

- Encombrement compatible avec un objet « de poche » (contrainte de boîtier, cf. `case/`).
- Détrompage/polarisation empêchant toute insertion inversée ou décalée.
- Rétention (clip, glissière, vis, friction) adaptée à un usage mobile.
- Facilité de fabrication pour un tiers (tolérances, procédés standards).
- Protection ESD et mécanique des contacts.

> ⏳ **À définir — Phase 2/3** : dimensions hors-tout, épaisseur, position du détrompeur,
> mécanisme de rétention exact, tolérances mécaniques. Dépend du choix de connecteur (§4)
> et du boîtier.

## 4. Connecteur ⏳

**Intention.** Sélectionner la famille de connecteur de l'interface CX-Bus. Ce choix
conditionne le brochage (§5), la mécanique (§3) et la stratégie de hot-plug (§7).

**Table d'analyse comparative (candidats) :**

| Critère | Board-to-board | Card-edge (cartouche) | FPC / flex | Pogo pins |
|---------|----------------|-----------------------|------------|-----------|
| Robustesse / nb d'insertions | Moyen | Élevé | Faible | Élevé |
| Nb de contacts disponibles | Élevé | Moyen-élevé | Élevé | Faible-moyen |
| Encombrement | Compact | Moyen | Très compact | Variable |
| Coût (module tiers) | Moyen | Faible (PCB nu) | Moyen | Élevé (côté Host) |
| Détrompage natif | Selon modèle | À concevoir | Faible | À concevoir |
| Aptitude hot-plug | Moyenne | Bonne (séquençable) | Faible | Bonne |

**Recommandation préliminaire.** Le format **card-edge** (contacts directement en bord de
PCB du Module, comme une cartouche de console) est privilégié pour son coût nul côté Module
et sa robustesse, sous réserve de valider le détrompage et le séquencement d'alimentation.
Décision à **trancher en Phase 1**.

> ⏳ **À définir — Phase 1** : famille de connecteur retenue, nombre de contacts, pas
> (pitch), référence(s) fabricant, ordre de contact (broches d'alimentation/masse
> établies avant les signaux pour le hot-plug).

## 5. Brochage ⏳

**Intention.** Organiser les signaux du connecteur par catégories fonctionnelles. Le
brochage exact (numéro de broche ↔ signal) est figé en Phase 1 après choix du connecteur.

**Catégories de signaux :**

| Catégorie | Signaux | Rôle |
|-----------|---------|------|
| Alimentation | `3V3`, `GND` (+ `VBAT` **candidat**) | Rail de référence `3V3` commuté et masses ; `VBAT` conditionné à analyse de sécurité (voir §6). |
| I²C | `SDA`, `SCL` | Bus lent partagé : support d'identification (EEPROM I²C **candidate**), capteurs lents, expander éventuel. |
| SPI | `SCK`, `MOSI`, `MISO`, `CSn` module | Bus rapide partagé avec l'écran (chip-select dédié module). |
| UART | `TX`, `RX` | Liaison série optionnelle pour modules le nécessitant. |
| GPIO / IRQ | `IRQ`, GPIO polyvalent(s) | Interruption module → Host, usage général. |
| Détection de présence | `PRSNT` / `DETECT` | Indique la présence physique d'un module (voir §7). |
| *(pas de broche d'enable côté module)* | — | Le slot reçoit un **rail commuté** par le Host. L'enable du load switch est un **signal interne au Host** et n'est **pas** exposé au module (voir §6). |

**Contrainte structurante.** Le MCU (Seeed XIAO ESP32-S3, remplaçable — ADR-0004) n'expose que
**11 GPIO**. Le brochage CX-Bus doit s'inscrire dans le budget GPIO global du Host, ce qui
impose le partage du SPI avec l'écran ; selon le budget retenu, un **GPIO expander I²C** côté
Host **pourra** être nécessaire (candidat à évaluer en Phase 1, voir `hardware/mainboard-v1/`).

> ⏳ **À définir — Phase 1** : table de brochage complète (broche ↔ signal), niveaux
> logiques, présence/absence de l'UART et du nombre de GPIO exposés, affectation du
> chip-select module sur le SPI partagé.

## 6. Alimentation ⏳

**Intention.** Définir les rails, le budget de courant par module, le power-gating et le
séquencement d'alimentation. Référence : ADR-0008.

**Principes :**

- **Rail régulé `3V3`** : candidat principal, **commuté** vers le slot par le Host.
- **`VBAT` (batterie brute) : candidat à ÉVALUER, PAS un rail acquis.** Exposer directement la
  batterie sur un connecteur tiers introduit des risques (court-circuit, décharge profonde,
  incompatibilité de tension). Sa présence sur le connecteur est **conditionnée à une analyse de
  sécurité** (Phase 1) ; à défaut, seul `3V3` commuté est exposé.
- **Power-gating** : l'alimentation du slot DOIT pouvoir être coupée par le Host via un **load
  switch** dont l'enable est un **signal interne au Host** (le module ne reçoit qu'un rail
  commuté — voir §5).
- **Séquencement** : les contacts d'alimentation/masse DOIVENT être établis avant les signaux ;
  la mise sous tension n'intervient qu'après stabilisation, dans le cadre des exigences de
  sûreté du §7.
- **Budget de courant** : un plafond par module DOIT être défini (protection du régulateur et de
  l'alimentation du Host).

**Critères de décision :** courant max soutenable par le Host, impact sur l'autonomie,
comportement en sous-tension batterie, protection contre court-circuit/surintensité d'un module
tiers, sûreté d'une éventuelle exposition de `VBAT`.

> ⏳ **À définir — Phase 1/2** : rails réellement exposés (et sort de `VBAT`), tensions, budget de
> courant nominal/crête par module, load switch, rampe d'inrush et de séquencement.

## 7. Détection de présence, hot-plug & sûreté électrique ⏳

> ⚠️ **La Phase 0 ne définit PAS de « séquence sûre ».** Un connecteur mécanique et électrique
> hot-plug partageant des bus (I²C, SPI) avec l'écran et les périphériques internes du Host pose
> des risques réels qui **doivent être étudiés et démontrés par prototype en Phase 1** avant
> tout gel. Cette section fixe les **exigences de sûreté à satisfaire**, pas une solution acquise.

**Risques identifiés à couvrir** (un module non alimenté peut déjà être raccordé aux lignes de
signaux) : back-powering par les diodes de protection ; lignes maintenues à l'état bas ;
corruption du bus I²C partagé ; perturbation de l'écran sur le SPI partagé ; courant d'appel non
contrôlé ; court-circuit ou panne d'un module tiers affectant le Host ; ESD à l'insertion.

**Exigences à étudier et à valider par prototype (Phase 1) :**

- **bus isolé ou commuté** vers le slot (l'accès du module aux bus n'est activé qu'après
  identification/validation) ;
- **état haute impédance** des signaux du côté module tant qu'il est hors tension ;
- **résistances série** et **protections ESD** sur les lignes exposées ;
- **protection contre surintensité et court-circuit** du rail alimentant le slot ;
- **limitation du courant d'appel** et **rampe d'alimentation** (inrush) ;
- **décharge contrôlée** du rail après coupure ;
- comportement défini si un **périphérique bloque SDA ou SCL** (bus stuck) ;
- **ordre réel des contacts** selon le connecteur retenu (§4) ;
- comportement lors d'un **retrait pendant une transaction** ;
- **masses établies en premier / rompues en dernier** (selon le sens d'insertion) ;
- **niveaux logiques et tolérance** des signaux lorsque `VBAT` ou `3V3` sont présents.

**Détection de présence.** Une ligne dédiée (`PRSNT`/`DETECT`) signalera la présence physique
d'un module (broche plus courte électriquement, ou lecture via l'expander I²C du Host). Le
mécanisme exact, les temporisations de debounce et les garanties de non-corruption du bus
partagé sont **⏳ à définir — Phase 1/2**, conditionnés aux résultats des prototypes de sûreté.

## 8. Identification — CX-Bus Manifest ⏳

Chaque Module porte un **CX-Bus Manifest** décrivant son identité et ses capacités. Le format
s'inspire de la spécification EEPROM des HAT Raspberry Pi (antériorité). **L'EEPROM I²C est le
mécanisme candidat, non figé** : il n'est pas acté tant que la lecture du Manifest ne peut pas
se faire **sans alimenter tout le matériel non fiable du module** (contrainte de sûreté, §7).

**Options à étudier (Phase 1) pour lire le Manifest en sécurité :**

- EEPROM sur un **petit rail d'identification séparé et limité en courant** ;
- alimentation complète **préchargée et limitée** avant validation ;
- **isolation des autres périphériques** du module pendant l'identification ;
- **contrôleur ou mécanisme alternatif** d'identification.

La structure logique des champs, le modèle de confiance et un exemple illustratif non normatif
sont dans le document dédié : [`cx-bus-manifest.md`](./cx-bus-manifest.md).

## 9. Protocole de communication

**Intention.** Définir les rôles et l'arbitrage sur les bus partagés.

**Rôles.** Le **Host est maître** des bus (I²C et SPI). Un **Module est esclave/périphérique**
et NE DOIT PAS initier de transaction perturbant un bus partagé sans y être invité par le Host.

**Arbitrage des bus partagés :**

- **SPI partagé écran + CX-Bus** : sélection exclusive par chip-select. Le Host garantit
  qu'un seul périphérique (écran ou module) est sélectionné à un instant donné et gère les
  sections critiques (ADR-0003 pour l'écran).
- **I²C partagé** : adressage par adresse esclave. Le support d'identification (EEPROM I²C
  candidate) et les périphériques du module DOIVENT présenter des adresses ne créant pas de
  collision sur le bus.

**Adressage I²C.** Le Manifest déclare les adresses I²C utilisées par le module. Compte tenu de
la **topologie V1 (un seul module**, §1), la stratégie de gestion des collisions se limite à
éviter les conflits entre **le module et les périphériques internes du Host** (adresses
réservées, ou isolation/expander côté Host) et entre **périphériques d'un même module**. Le
multi-module (chaînage/hub) est hors périmètre V1.

> ⏳ **À définir — Phase 1** : plan d'adressage I²C réservé, protocole applicatif au-dessus
> du transport (le cas échéant), débits SPI/I²C garantis, gestion des interruptions `IRQ`.

## 10. Règles de compatibilité

**Intention.** Garantir la durabilité du parc de modules dans le temps.

- **Compatibilité ascendante (module ancien sur Host récent).** Un Module conforme à une
  version antérieure DOIT rester fonctionnel sur un Host d'une version MAJEURE identique.
  Le Host lit `version du protocole CX-Bus requis` dans le Manifest et refuse proprement un
  module trop récent plutôt que d'échouer silencieusement.
- **Compatibilité descendante (module récent sur Host ancien).** Non garantie entre versions
  MINEURES si le module exige une capacité absente du Host ; le Host DOIT le signaler.
- **Rupture MAJEURE.** Un changement incompatible (brochage/mécanique/protocole) incrémente
  la version MAJEURE ; le détrompage mécanique DEVRAIT empêcher l'appariement physique de
  générations incompatibles.

> ⏳ **À définir — Phase 1** : matrice de compatibilité version standard ↔ version Manifest,
> comportement précis du Host au refus.

## 11. Conformité & certification « CX-Bus compatible »

**Intention.** Définir les critères permettant de déclarer un module tiers **« CX-Bus
compatible »**.

**Critères de conformité (préliminaires) :**

1. Respect du format mécanique et du connecteur (§3, §4).
2. Respect du brochage et des rails (§5, §6), sans dépassement du budget de courant.
3. Présence d'un **support d'identification** portant un **CX-Bus Manifest** valide et lisible
   (§8, `cx-bus-manifest.md`) — mécanisme normatif à figer en Phase 1 (EEPROM I²C = option
   privilégiée à évaluer).
4. Respect des rôles et de l'arbitrage des bus partagés (§9) — aucun blocage du bus.
5. Comportement sûr à l'insertion/retrait à chaud (§7).
6. Déclaration correcte de la version de protocole CX-Bus requise.

> ⏳ **À définir — Phase 1+** : procédure de test de conformité, banc de validation,
> gestion de la marque/label « CX-Bus compatible », auto-certification vs vérification.

## 12. Historique des révisions

| Version | Date | Statut | Notes |
|---------|------|--------|-------|
| 0.1.0-draft | 2026-07 | BROUILLON | Création de la structure de spécification (Phase 0). Aucune valeur normative figée. |

---

*Document publié sous CC-BY-4.0. Voir le [README du standard](./README.md).*

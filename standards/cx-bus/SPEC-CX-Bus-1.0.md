# Spécification CX-Bus 1.0

> **Statut : BROUILLON / DRAFT** — Phase 0 (Fondations).
> Ce document fige l'**intention**, la **structure** et les **critères de décision** du standard.
> Les valeurs électriques, mécaniques et le brochage définitifs sont marqués « ⏳ À définir »
> avec la phase de projet où ils seront tranchés. Aucune valeur figurée ici n'est normative
> tant qu'elle porte ce marqueur.

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

## 2. Terminologie

| Terme | Définition |
|-------|-----------|
| **Host** (CX-Bus Host) | Carte mère exposant le connecteur CX-Bus et pilotant le bus. Maître des bus partagés. |
| **Module** (CX-Bus Module) | Carte d'extension amovible (« cartouche ») conforme au standard. |
| **Manifest** (CX-Bus Manifest) | Descripteur d'identification normalisé stocké dans l'EEPROM I²C du Module. Voir `cx-bus-manifest.md`. |
| **SDK** (CX-Bus SDK) | Ensemble logiciel d'intégration d'un Module (firmware Module + intégration Host). |
| **Rail** | Ligne d'alimentation continue disponible sur le connecteur (VBAT, 3V3, GND…). |
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
| Alimentation | `VBAT`, `3V3`, `GND` | Rails d'alimentation et masses (voir §6). |
| I²C | `SDA`, `SCL` | Bus lent partagé : EEPROM Manifest, capteurs lents, expander. |
| SPI | `SCK`, `MOSI`, `MISO`, `CSn` module | Bus rapide partagé avec l'écran (chip-select dédié module). |
| UART | `TX`, `RX` | Liaison série optionnelle pour modules le nécessitant. |
| GPIO / IRQ | `IRQ`, GPIO polyvalent(s) | Interruption module → Host, usage général. |
| Détection de présence | `PRSNT` / `DETECT` | Indique la présence physique d'un module (voir §7). |
| Enable | `MOD_EN` | Power-gating : activation/coupure de l'alimentation du module (voir §6). |

**Contrainte structurante.** Le MCU (Seeed XIAO ESP32-S3, socketé — ADR-0004) n'expose que
**11 GPIO**. Le brochage CX-Bus doit s'inscrire dans le budget GPIO global du Host, ce qui
impose le partage du SPI avec l'écran et le recours à un GPIO expander I²C côté Host (voir
`hardware/mainboard-v1/`).

> ⏳ **À définir — Phase 1** : table de brochage complète (broche ↔ signal), niveaux
> logiques, présence/absence de l'UART et du nombre de GPIO exposés, affectation du
> chip-select module sur le SPI partagé.

## 6. Alimentation ⏳

**Intention.** Définir les rails, le budget de courant par module, le power-gating et le
séquencement d'alimentation. Référence : ADR-0008.

**Principes :**

- Rails candidats : `VBAT` (batterie brute) et `3V3` (régulé). À confirmer en §5.
- **Power-gating** : l'alimentation du connecteur module DOIT être coupable par le Host via
  `MOD_EN`, afin de préserver l'autonomie lorsqu'aucun module n'est actif.
- **Séquencement** : à l'insertion, les contacts d'alimentation/masse DOIVENT être établis
  avant les signaux ; l'activation logique (`MOD_EN`) intervient après stabilisation.
- **Budget de courant** : un plafond par module DOIT être défini pour protéger la jauge de
  batterie et le régulateur du Host.

**Critères de décision :** courant max soutenable par le régulateur Host, impact sur
l'autonomie, comportement en sous-tension batterie, protection contre les courts-circuits
d'un module tiers.

> ⏳ **À définir — Phase 1/2** : tensions exactes des rails, budget de courant nominal et
> crête par module, composant de power-gating (load switch), rampe de séquencement.

## 7. Détection de présence & hot-plug

**Intention.** Permettre au Host de détecter en sécurité l'insertion et le retrait d'un
module, y compris à chaud.

**Mécanisme de détection.** Une ligne dédiée (`PRSNT`/`DETECT`) signale la présence physique
d'un module. Le principe retenu est une broche de détection plus courte électriquement (ou
lue via l'expander I²C côté Host), établie en dernier à l'insertion et rompue en premier au
retrait, ce qui borne la fenêtre d'activité des signaux.

**Séquence d'insertion sûre (principe) :**

1. Contacts d'alimentation et de masse établis.
2. Contacts de signaux établis.
3. Ligne de présence active → le Host détecte le module.
4. Le Host applique `MOD_EN` (mise sous tension via power-gating).
5. Le Host lit le **CX-Bus Manifest** en EEPROM I²C et identifie le module (§8).

**Séquence de retrait sûre (principe) :**

1. Perte du signal de présence → le Host coupe `MOD_EN`.
2. Le Host libère les ressources logicielles associées au module.
3. Rupture physique des signaux puis de l'alimentation.

> ⏳ **À définir — Phase 1/2** : implémentation exacte de la détection (broche courte vs
> expander), temporisations de debounce, garanties de non-corruption du bus partagé pendant
> l'insertion.

## 8. Identification — CX-Bus Manifest

Chaque Module DOIT embarquer un **CX-Bus Manifest** stocké dans une EEPROM I²C, décrivant
son identité et ses capacités. Le format s'inspire de la spécification EEPROM des HAT
Raspberry Pi (antériorité).

La structure logique des champs, les règles de lecture et un exemple illustratif non
normatif sont définis dans le document dédié : [`cx-bus-manifest.md`](./cx-bus-manifest.md).

## 9. Protocole de communication

**Intention.** Définir les rôles et l'arbitrage sur les bus partagés.

**Rôles.** Le **Host est maître** des bus (I²C et SPI). Un **Module est esclave/périphérique**
et NE DOIT PAS initier de transaction perturbant un bus partagé sans y être invité par le Host.

**Arbitrage des bus partagés :**

- **SPI partagé écran + CX-Bus** : sélection exclusive par chip-select. Le Host garantit
  qu'un seul périphérique (écran ou module) est sélectionné à un instant donné et gère les
  sections critiques (ADR-0003 pour l'écran).
- **I²C partagé** : adressage par adresse esclave. L'EEPROM Manifest et les périphériques du
  module DOIVENT présenter des adresses ne créant pas de collision sur le bus.

**Adressage I²C.** Le Manifest déclare les adresses I²C utilisées par le module. Une
stratégie de gestion des collisions (adresses réservées, ou multiplexage/expander côté Host)
est nécessaire pour autoriser plusieurs modules ou éviter les conflits avec les
périphériques du Host.

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
3. Présence d'un **CX-Bus Manifest** valide et lisible en EEPROM I²C (§8, `cx-bus-manifest.md`).
4. Respect des rôles et de l'arbitrage des bus partagés (§9) — aucun blocage du bus.
5. Comportement sûr à l'insertion/retrait à chaud (§7).
6. Déclaration correcte de la version de protocole CX-Bus requise.

> ⏳ **À définir — Phase 1+** : procédure de test de conformité, banc de validation,
> gestion de la marque/label « CX-Bus compatible », auto-certification vs vérification.

## 12. Historique des révisions

| Version | Date | Statut | Notes |
|---------|------|--------|-------|
| 1.0.0-draft | 2026-07 | BROUILLON | Création de la structure de spécification (Phase 0). Aucune valeur normative figée. |

---

*Document publié sous CC-BY-4.0. Voir le [README du standard](./README.md).*

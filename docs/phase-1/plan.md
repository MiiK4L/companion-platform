<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Phase 1 — Plan consolidé (spécification CX-Bus, composants, budgets)

> **Statut : planification.** Ce document cadre l'exécution de la Phase 1. Il ne
> fige aucun composant : tout choix technologique reste **Proposé** tant qu'un
> résultat **reproductible** (voir [cadre de validation](validation-framework.md))
> n'atteint pas les seuils chiffrés de son protocole.

## Principes acceptés vs technologies candidates

**Principes acceptés** (issus de la Phase 0, ne se rejouent pas sauf preuve
contraire d'un prototype) : modèle **ports/adaptateurs** · runtime **hybride
extensible** · **interface graphique abstraite** · **modules auto-identifiés +
connecteur unique (CX-Bus)** · **cœur de calcul remplaçable** · **power-gating +
recharge USB-C + gestion d'énergie** · **base de temps + persistance + Δt** ·
ESP-IDF **confiné aux adaptateurs** · topologie **V1 = 1 slot** · **modèle de
confiance** (le Module Manager publie des capacités ; l'App Manager décide) ·
pas de `VBAT` sur connecteur sans analyse de sécurité.

**Technologies candidates** (objet des mesures de la Phase 1, statut *Proposé*) :
cœur **XIAO ESP32-S3** *(candidat de référence de prototypage)* · **Lua** +
installation dynamique · **TFT IPS + LVGL** + contrôleur/format · mécanisme
d'identification **EEPROM I²C** *(baseline expérimentale)* + format Manifest +
hot-plug · **jauge** + batterie + power-path · **RTC externe / PCF8563** ·
**GPIO expander** · famille de **connecteur** + brochage + exposition `VBAT`.

## Méthode : rien n'est figé sans preuve reproductible

Chaque lot matériel suit un **protocole de test renseigné *avant* l'essai**
([modèle](templates/test-protocol-template.md)) et produit un **rapport de
mesure** ([modèle](templates/measurement-report-template.md)). Une décision ne
passe de **Proposé** à **Accepté** que via un résultat **reproductible**
atteignant ses **seuils chiffrés** (voir [validation-framework](validation-framework.md)).
Les seuils indiqués ci-dessous sont **proposés, à confirmer** dans chaque protocole.

---

## Lot 1 — Matrice des exigences & budgets (GPIO/bus/énergie/mécanique/coût)

- **Objectif** : établir les contraintes qui gouvernent tous les lots, **en
  partant des besoins de la plateforme** (indépendants du MCU) avant de les
  confronter au XIAO et à ≥ 1 architecture alternative.
- **Questions** : quels besoins réels en GPIO/bus/RAM/flash/USB/conso/radio/
  dimensions/maintenance ? le XIAO (11 GPIO) les couvre-t-il, et à quel prix
  (expander, SPI partagé) ? quelle architecture alternative comparer ? quel
  scénario d'usage sert de référence énergétique ?
- **Hypothèses à ne pas figer** : « le XIAO est le cœur » ; « 11 GPIO suffisent » ;
  « un expander suffira » ; toute cible d'autonomie chiffrée.
- **Prototypes/mesures** : aucun matériel — **DOC/SIM** (tableurs, datasheets).
- **Composants candidats** : cœur — XIAO ESP32-S3 (référence) vs autre module
  ESP32-S3 vs carte custom (ESP32-S3 nu). Voir [matrice d'exigences](requirements-matrix.md).
- **Critères de réussite/échec** : matrice GPIO **sans conflit**, budgets
  exprimés **avec incertitudes**, scénario d'usage **défini**. Échec = périmètre
  V1 non logeable dans les contraintes → arbitrage (réduction, expander, pivot cœur).
- **Dépendances** : amont de tous ; affiné par L2/L4/L5/L6.
- **Risques** : R1 (GPIO), R9 (mécanique).
- **Livrables** : [requirements-matrix.md](requirements-matrix.md), budget GPIO/bus,
  budget énergie v0, enveloppe mécanique v0, budget coût v0, comparaison d'architectures cœur + **critères de rejet du XIAO**.
- **Proposé→Accepté possible** : l'allocation GPIO/bus **de référence** est établie ici mais **reste Proposé** (révisable) tant que le banc de bring-up (L2) ne l'a pas validée ; scénario d'usage.
- **Effort** : 2–4 j · **DOC/SIM**.

## Lot 2 — Sûreté électrique & prototype CX-Bus

- **Objectif** : démontrer par la mesure les **exigences de sûreté** avant tout gel du connecteur/hot-plug.
- **Questions** : isolation/commutation du bus vers le slot ? haute-Z module non alimenté ? inrush/rampe ? court-circuit/surintensité ? bus tenu bas (stuck) ? retrait en transaction ? ordre des contacts selon connecteur ?
- **Hypothèses à ne pas figer** : « séquence sûre » esquissée ; exposition de `VBAT` ; « un load switch suffit » ; card-edge.
- **Prototypes/mesures** : **HW** — banc breadboard/petit PCB : load switch + isolation de bus, mesures à l'oscillo et à l'alim de labo (limitation de courant).
- **Composants candidats** : load switch TPS22918 / AP22913 / P-MOSFET ; isolation I²C TCA9517 / PCA9306 ; ESD PESD série.
- **Critères de réussite (chiffrés, proposés)** : inrush pic ≤ 2× courant établi + cap absolu à fixer, rampe ≥ 1 ms ; court-circuit franc maintenu 10 s × 100 : Host **0 reset/0 dégât**, reprise à la levée ; bus-stuck détecté ≤ 50 ms, récupération auto, 0 blocage sur ≥ 100 essais ; **≥ 500 cycles** hot-plug avec écran + trafic I²C → **0 corruption / 0 glitch hors spec** ; retrait en transaction × 200 → 0 crash. Coins de tension : rails ±5 % + batterie basse (≈ 3,0 V). **Échec** = corruption reproductible ou risque Host → revoir topologie/connecteur.
- **ESD (encadré)** : **analyse des chemins ESD**, **sélection/dimensionnement des protections**, **essais exploratoires uniquement** avec procédure/équipement adaptés, **aucune affirmation de conformité normative** (préconformité/certification hors périmètre Phase 1).
- **Connecteur (endurance chiffrée, proposé)** : **≥ 1000 cycles** d'insertion ; **résistance de contact mesurée /100 cycles, dérive < 50 mΩ** ; inspection mécanique ; 0 intermittence > 1 µs.
- **Dépendances** : ← L1 ; ↔ L6 ; → L3 ; → gel connecteur (L9).
- **Risques** : R2 (SPI partagé), R5 (hot-plug), back-powering, collisions I²C, endurance.
- **Livrables** : rapport de sûreté électrique + schéma chemin de puissance/isolation + procédures & résultats + reco famille de connecteur.
- **Proposé→Accepté possible** : exigences de sûreté ; famille de connecteur ; principe d'isolation/commutation ; exposition (ou non) de `VBAT`.
- **Effort** : 5–8 j · **HW** (cœur de la phase).

## Lot 3 — Mécanisme d'identification des modules (neutre)

- **Objectif** : d'abord **comparer les architectures d'identification** ; ensuite seulement prototyper une ou deux options ; l'EEPROM I²C est une **baseline expérimentale, pas une présélection**.
- **Étape 1 — matrice d'exigences & comparaison** (avant tout prototype) : passif vs actif ; lecture **Host-powered vs Module-powered** ; capacité ; identité unique (EUI) ; provisioning ; protection en écriture ; détection de corruption ; **évolution vers l'authenticité** crypto ; comportement en panne périphérique ; coût/surface.
- **Hypothèses à ne pas figer** : EEPROM I²C comme solution ; format Manifest esquissé ; « un CRC suffit » (≠ authenticité).
- **Prototypes/mesures** : **HW+CODE** — prototyper **≥ 2 options crédibles** (EEPROM baseline + une alternative, p.ex. µC d'ID actif ou 1-Wire) : lecture sans alimenter le reste du module, parse + CRC au boot, provisioning (script).
- **Composants candidats (au même niveau)** : 24AA02E64 (EUI) / AT24C64 / 1-Wire DS28E07 / µC d'ID (ATtiny/RP2040).
- **Critères de réussite (proposés)** : identification fiable **sans** exposer les autres périphériques ; format Manifest lu/validé (CRC) ; 0 collision d'adresse (topologie 1 slot) ; provisioning reproductible. **Échec** = pas d'isolation possible → l'option reste *Proposé*, alternative documentée.
- **Dépendances** : ← L2 (rail d'ID/sûreté), ← L1 (adressage).
- **Risques** : R6 (lecture), collisions I²C, provisioning.
- **Livrables** : matrice de comparaison d'architectures d'ID + rapport de mesure + format Manifest candidat + outil de provisioning (prototype).
- **Proposé→Accepté possible** : mécanisme d'identification + format Manifest **après** comparaison + preuve de lecture sûre.
- **Effort** : 3–5 j · **HW+CODE**.

## Lot 4 — Écran & moteur graphique

- **Objectif** : mesurer la famille d'écran + le moteur graphique contre les budgets RAM/énergie/fluidité, derrière l'abstraction déjà actée.
- **Questions** : contrôleur/format/résolution ? budget mémoire du moteur graphique candidat (buffers) sur ESP32-S3 ? FPS ? conso rétroéclairage par niveau ? SPI partagé viable (cf. L2) ?
- **Hypothèses à ne pas figer** : TFT IPS ; LVGL ; ST7789 ; une résolution.
- **Prototypes/mesures** : **HW** — 1–2 panneaux candidats via l'abstraction ; mesures RAM/FPS/conso (backlight PWM) ; extinction auto + reprise.
- **Composants candidats** : ST7789 / GC9A01 / ILI9341 ; LVGL (+ pilote LovyanGFX/TFT_eSPI) ; Sharp Memory LCD (mono, repli si autonomie).
- **Critères de réussite (proposés)** : budget mémoire du moteur graphique candidat **dans le budget L1** ; animations **≈ 30 fps** UI ; conso écran mesurée compatible de la cible d'autonomie (L6). **Échec** = hors budget → réduire résolution/buffer partiel ou reconsidérer la famille.
- **Dépendances** : ← L1 ; ↔ L2 (SPI) ; ↔ L6 (conso).
- **Risques** : R2, RAM/PSRAM, R4 (autonomie).
- **Livrables** : rapport écran (RAM/FPS/conso) + reco contrôleur+format.
- **Proposé→Accepté possible** : contrôleur+format ; LVGL ; budget RAM/écran.
- **Effort** : 4–6 j · **HW** (+ CODE via L8).

## Lot 5 — Cœur de calcul remplaçable & stratégie de socketage

- **Objectif** : valider **comment** rendre le cœur remplaçable (principe acté ; mise en œuvre non), et **mesurer le deep-sleep réel**.
- **Questions** : socket 2,54 mm fiable en usage nomade ? impact épaisseur/boîtier ? alternative (embases de précision, castellations soudées) ? deep-sleep du cœur monté ?
- **Hypothèses à ne pas figer** : embases 2,54 mm ; « +2–3 mm » ; « le deep-sleep du XIAO atteint la cible ».
- **Prototypes/mesures** : **HW** — cœur socketé vs soudé ; courant deep-sleep (µA-mètre PPK2/Joulescope) ; cycles d'insertion + profil de vibration avec surveillance de continuité ; relevé d'épaisseur.
- **Composants candidats** : embases 2,54 mm / embases de précision (tulipe) / castellations soudées (repli).
- **Critères de réussite (proposés)** : deep-sleep système **< seuil dérivé de la cible d'autonomie (L6)** ; résistance de contact < seuil à fixer ; ≥ N insertions sans dégradation ; 0 intermittence sous vibration. **Échec** = deep-sleep > cible → **décision structurante** (peut rouvrir ADR-0004, carte custom).
- **Dépendances** : ← L1 ; ↔ L6 ; → boîtier (Phase 4).
- **Risques** : **R3** (deep-sleep XIAO — peut rouvrir le choix du cœur), contact.
- **Livrables** : rapport cœur/socket (conso + méca).
- **Proposé→Accepté possible** : stratégie de socketage **ou** décision motivée d'alternative.
- **Effort** : 3–5 j · **HW**.

## Lot 6 — Batterie, USB-C, power-path, power-gating & jauge

- **Objectif** : concevoir et **mesurer** la chaîne d'alimentation, et établir un **budget énergétique mesuré** (cible d'autonomie provisoire, révisable).
- **Questions** : charge via XIAO ou circuit dédié à power-path ? régulation 3V3 et courant dispo ? jauge nécessaire ou ADC ? load switch ? chimie/capacité/format batterie ?
- **Hypothèses à ne pas figer** : présence d'une jauge ; MAX17048 ; « plusieurs jours » ; recharge via XIAO suffisante ; `VBAT` exposé.
- **Prototypes/mesures** : **HW** — chaîne charge+régulation+power-gating ; conso par mode ; décharge réelle vs jauge ; fuite module en veille.
- **Composants candidats** : charge MCP73831 / BQ24074 / BQ25185 / TP4056 ; jauge MAX17048 / LC709203F / BQ27441 / ADC ; load switch TPS22918 / AP22913 ; régulation TLV757P / TPS62840 ; batterie LiPo 3,7 V (capacités) / LiFePO4 (documenté).
- **Critères de réussite (proposés)** : conso par mode (VBAT 4,2/3,7/3,4/3,0 V, moyenne ≥ T min) ; **erreur jauge < ±5 %** vs référence coulomb sur décharge complète ; **fuite module coupé < 10 µA** ; charge complète avec **ΔT thermique < seuil**. **Échec** = autonomie très inférieure → arbitrage écran/batterie/mode.
- **Dépendances** : ← L1 ; ↔ L2 (load switch/inrush) ; ↔ L4 (conso écran) ; ↔ L5 (deep-sleep).
- **Risques** : R4 (autonomie), dispo jauge, courant régulateur.
- **Livrables** : rapport architecture d'alim + **budget énergie v1 (mesuré)** + reco batterie + cible d'autonomie provisoire.
- **Proposé→Accepté possible** : chaîne d'alim ; présence/choix jauge ; load switch ; batterie ; budget/cible d'autonomie.
- **Effort** : 5–8 j · **HW**.

## Lot 7 — RTC & persistance du temps

- **Objectif** : valider la base de temps fiable + Δt au réveil (principe acté ; RTC externe non).
- **Questions** : RTC interne suffisant ou externe nécessaire ? modèle ? dérive/conso réelles ? réveil alarme + mouvement ? persistance NVS/LittleFS robuste ?
- **Hypothèses à ne pas figer** : nécessité d'un RTC externe ; PCF8563 ; une puce I²C de plus.
- **Prototypes/mesures** : **HW+CODE** — cycles power-off/réveil ; dérive vs référence ; conso RTC ; Δt ; reprise d'état après coupures brutales.
- **Composants candidats** : PCF8563 / RV-3028-C7 / DS3231 / RTC interne (repli).
- **Critères de réussite (proposés)** : dérive **< seuil (ppm) du modèle** sur **≥ 72 h** ; conso RTC **< 1 µA** ; Δt correct sur **100 cycles** deep-sleep (erreur 0) ; reprise d'état sur **100 coupures brutales** (0 corruption, vérif checksum). **Échec** = dérive/conso inacceptables → autre modèle ou RTC interne + resync.
- **Dépendances** : ← L1 (I²C/IRQ) ; ↔ L6 ; ↔ L8 (abstraction horloge/stockage).
- **Risques** : budget GPIO (IRQ réveil), dérive.
- **Livrables** : rapport base de temps (dérive/conso) + reco RTC.
- **Proposé→Accepté possible** : opportunité + modèle RTC ; modèle de persistance.
- **Effort** : 2–4 j · **HW+CODE**.

## Lot 8 — Prototype logiciel minimal ports/adaptateurs

- **Objectif** : matérialiser l'architecture acceptée en **squelette testable sur PC**, sans figer les composants.
- **Rôles à matérialiser** (pas une arborescence historique) : **ports** (display, input, clock, storage, bus, power, scheduler) ; **adaptateurs host** (mocks) ; **adaptateurs cible ESP-IDF** ; **services** (Module/App Manager, UI, Power, Storage, Connectivity, Companion) ; **Companion SDK** (façade) ; **composition root**. Les **drivers** sont des **détails internes des adaptateurs**. Le nommage des dossiers est une **conséquence**, pas un préalable ; **`HAL` ne redevient pas le centre**.
- **Questions** : les ports sont-ils définissables proprement ? le composition root fonctionne-t-il (injection cible vs host) ? faisabilité du **runtime scripté candidat** embarqué + chargement d'app depuis LittleFS (RAM/flash) ? le Companion SDK est-il **utilisable et exposable proprement en C et depuis le runtime scripté candidat** ?
- **Hypothèses à ne pas figer** : Lua comme runtime ; installation dynamique « sans reflash » ; signatures d'API définitives.
- **Prototypes/mesures** : **CODE** (host) + **HW** (POC) — squelette ports + adaptateurs host + tests host en CI ; POC ESP-IDF (boot → composition root → 1 port réel) ; P8 : runtime scripté candidat embarqué, **RAM/flash par app chiffrées**, chargement d'une app depuis LittleFS.
- **Composants candidats (logiciels)** : runtime Lua vs WASM (WAMR, alternative documentée) ; test host Unity/CMock ou GoogleTest.
- **Critères de réussite (proposés)** : squelette **compile** ; **tests host verts en CI** ; **graphe de dépendances vérifié** (0 `#include` ESP-IDF/FreeRTOS/driver dans ports/services/SDK) ; composition root démontré ; RAM/flash par app du runtime scripté candidat **chiffrées** ; faisabilité (ou limites) de l'installation dynamique **documentée**. **Échec** = install dynamique infaisable dans le budget → recadrer la promesse (bundle OTA), garder l'abstraction de runtime.
- **Dépendances** : ← L1 (ports/bus) ; démarre **tôt et en parallèle** (partie host sans matériel).
- **Risques** : R7 (install dynamique), RAM.
- **Livrables** : squelette ports/adaptateurs/services/SDK + composition root + mocks ; tests host ; rapport de faisabilité runtime.
- **Proposé→Accepté possible** : Lua comme runtime V1 ; modèle réaliste d'installation d'apps ; premières signatures de ports.
- **Effort** : 5–8 j · **CODE** (majorité) **+ HW** (POC).

## Lot 9 — Synthèse, ADR & release candidate de la SPEC

- **Objectif** : consolider les preuves, faire passer en **Accepté** uniquement ce qui est démontré, rédiger les ADR 0013+, produire une **BOM V1 candidate**, et promouvoir la SPEC **au maximum en `spec-v1.0.0-rc.1`**.
- **Prototypes/mesures** : aucun — **DOC**.
- **Critères** : chaque décision *Accepté* adossée à un rapport reproductible ; **`spec-v1.0.0` NON promue** en Phase 1.
- **Sortie SPEC** : `0.x` stabilisée, ou **`spec-v1.0.0-rc.1`** si tous les critères de sortie sont satisfaits. Le passage à `1.0.0` (hors Phase 1) exigera une **carte Host intégrée**, **plusieurs types de modules**, une **campagne d'interopérabilité** et la **résolution des retours d'implémentation**.
- **Livrables** : ADR 0013+ (Acceptées ou maintenues *Proposé*) ; synthèse Phase 1 ; **BOM V1 candidate** ; matrice de compatibilité ; tag `spec-v1.0.0-rc.1` si mérité.
- **Dépendances** : ← tous.
- **Effort** : 3–5 j · **DOC**.

---

Voir aussi : [ordre d'exécution & critères de sortie](execution.md) ·
[matrice des exigences](requirements-matrix.md) ·
[registre des risques](risk-register.md) ·
[sourcing & BOM](sourcing-and-bom.md) ·
[cadre de validation](validation-framework.md) ·
[ADR prévisionnelles 0013+](adr-plan.md).

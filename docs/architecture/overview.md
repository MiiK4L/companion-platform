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
| Exécution des apps | Hybride **natif + Lua**, abstraction de runtime (WASM futur) | [0002](../adr/0002-modele-execution-applications.md) |
| Affichage | **TFT IPS couleur** abstrait par un moteur graphique (LVGL) | [0003](../adr/0003-technologie-et-abstraction-ecran.md) |
| Modularité | Standard d'extension **CX-Bus** + identification par EEPROM | [0005](../adr/0005-standard-cx-bus-et-identification.md) / [0006](../adr/0006-nommage-cx-bus.md) |
| Fondation firmware | **ESP-IDF** confiné sous une **HAL + Companion SDK** | [0001](../adr/0001-framework-firmware-esp-idf.md) / [0007](../adr/0007-hal-et-companion-sdk.md) |

## 3. Le principe qui tient tout ensemble

**Inversion des dépendances matérielles** :

```
apps  →  Companion SDK  →  HAL  →  drivers  →  silicium (ESP-IDF / FreeRTOS)
```

Aucune couche ne saute une autre. Les applications ne connaissent que le
**Companion SDK** ; elles ignorent tout de l'ESP32, du RTOS, du contrôleur
d'écran ou du runtime. Conséquence directe : **MCU, RTOS, écran et runtime sont
tous remplaçables sans réécrire les apps**. C'est notre assurance de longévité.
Voir [le détail](dependency-inversion.md).

## 4. Vue matérielle (résumé)

Une carte mère unique — le **CX-Bus Host** — ne contient que l'infrastructure
générique : cœur de calcul (XIAO ESP32-S3 **socketé**, [ADR-0004](../adr/0004-coeur-de-calcul-socket.md)),
alimentation/charge USB-C, écran, entrées (boutons/buzzer/vibreur), RTC,
accéléromètre, et **un connecteur d'extension unique**. **Aucun capteur
applicatif n'est soudé** : tout capteur passe par un **module CX-Bus**. Détails :
[architecture matérielle](hardware.md).

## 5. Vue logicielle (résumé)

Le firmware se comporte comme un petit système d'exploitation, organisé en
couches (bootloader → noyau → HAL → drivers → services → apps). Le
**Module Manager** détecte et identifie les modules CX-Bus, l'**App Manager**
gère le cycle de vie des applications (runtime Lua), et le service **Companion**
transforme n'importe quel état système en expression du personnage. Détails :
[architecture logicielle](software-os.md).

## 6. Modularité « type console »

Insérer un module CX-Bus déclenche : *détection → alimentation → lecture du
CX-Bus Manifest (EEPROM) → validation → chargement des drivers → apparition des
applications*. Un module GPS fait apparaître une app GPS ; un module CO₂ une app
« qualité de l'air ». Le personnage sert d'interface : plutôt que « CPU 80 % »,
il paraît fatigué ; plutôt que « serveur injoignable », il paraît inquiet.

## 7. Où lire la suite

- [Architecture matérielle](hardware.md)
- [Architecture logicielle / OS](software-os.md)
- [Inversion des dépendances](dependency-inversion.md)
- [Glossaire](glossary.md)
- [Standard CX-Bus](https://github.com/MiiK4L/companion-platform/blob/main/standards/cx-bus/README.md)
- [Décisions d'architecture (ADR)](../adr/README.md)
- [Feuille de route](../roadmap.md)

# ADR-0009 : Base de temps fiable et persistance d'état (évolution par Δt)

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideur** : Mainteneur du projet (MiiK4L)
- **Expertises consultées** : Architecte firmware, expert basse consommation
- **Phase de roadmap** : 0 → 1
- **Domaines impactés** : hardware, firmware, apps
- **Tags** : rtc, persistance, deep-sleep, tamagotchi

## 1. Contexte et problème

Le Tamagotchi doit « continuer à évoluer même lorsque l'appareil est éteint ».
Cela impose deux capacités : conserver l'heure de façon fiable à travers les
phases de deep sleep / extinction, et persister l'état des applications. Le RTC
interne de l'ESP32 dérive et n'est pas garanti à travers toutes les coupures ;
il faut une base de temps fiable et peu gourmande.

## 2. Options étudiées

### Base de temps
- **A. RTC interne de l'ESP32-S3** — gratuit, mais dérive notable, fiabilité
  limitée à travers les coupures d'alimentation.
- **B. RTC externe basse consommation (PCF8563)** — précis, très faible conso,
  maintien par la batterie, réveil programmé (alarme). Surcoût d'une puce I²C.
- **C. RTC de haute précision (DS3231)** — très précis (TCXO) mais conso plus
  élevée et coût supérieur ; surdimensionné pour l'usage.

### Persistance d'état
- **A. NVS (clé-valeur ESP-IDF)** — robuste pour l'état/les réglages.
- **B. Système de fichiers (LittleFS)** — pour les données/sauvegardes/assets.
- (Les deux sont complémentaires.)

## 3. Décision

> **Portée du statut.** Seul le *principe* ci-dessous est **Accepté**. Les éléments listés en §6 (Décisions différées) sont au statut **Proposé** et seront actés par de futures ADR une fois validés par prototype.

Principe accepté : disposer d'une **base de temps fiable** associée à une
**persistance d'état**, avec une **évolution par Δt au réveil** plutôt que par
un fonctionnement continu.
- L'évolution « hors tension » est calculée par **différence de temps au
  réveil** (Δt entre dernier arrêt et réveil), et non par une tâche de fond
  permanente : on lit l'horloge au réveil, on applique l'évolution
  correspondante.
- Persistance : **NVS** pour l'état/réglages, **LittleFS** pour données et
  sauvegardes d'apps.

Les éléments spécifiques — la **nécessité d'un RTC externe** (vs RTC interne),
le **modèle PCF8563**, la précision et la consommation — relèvent des décisions
différées (§6) : ils ne sont pas actés comme définitifs par la présente ADR.

## 4. Raisons du choix

Le calcul par Δt est le modèle le plus économe : l'appareil dort réellement,
sans tâche de fond, et « rattrape » l'évolution au réveil. Une base de temps
externe basse consommation (**PCF8563 candidat**) est pressentie pour la
fiabilité et la précision, à valider (§6). NVS + LittleFS sont les mécanismes
éprouvés de l'écosystème.

## 5. Conséquences

### Positives
- Deep sleep réel (autonomie) tout en gardant un compagnon « vivant ».
- État des apps robuste aux coupures.

### Négatives / compromis acceptés
- **Si** un RTC externe est retenu (§6) : une puce I²C et une ligne
  d'interruption de réveil supplémentaires (budget GPIO/adresses I²C à arbitrer,
  ADR-0004/0008) — non acté tant que le RTC externe reste proposé.

### Impacts futurs
- La HAL doit exposer une abstraction « horloge/alarme » et « stockage
  persistant » indépendantes du composant.
- Les apps conçoivent leur logique d'évolution en fonction de Δt (contrat SDK).

## 6. Décisions différées (statut : Proposé — à valider par prototype)

Éléments au statut **Proposé**, qui feront l'objet de futures ADR une fois
validés (prototype P4) :
- l'**opportunité et la nécessité** d'un RTC externe (vs RTC interne) et le
  **modèle exact** (PCF8563 proposé) ;
- la **précision réelle** et la **consommation** mesurées ;
- le partage du bus I²C et le budget de broches (interruption de réveil) ;
- la robustesse de la reprise d'état après coupure (tests).

## 7. Liens
- [ADR-0008](0008-architecture-alimentation.md) · [ADR-0007](0007-hal-et-companion-sdk.md) · [apps/tamagotchi](../../apps/tamagotchi/README.md)

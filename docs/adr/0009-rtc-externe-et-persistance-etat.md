# ADR-0009 : RTC externe et persistance d'état hors tension

- **Statut** : Accepté (principes) ; composant figé en Phase 1
- **Date** : 2026-07-21
- **Décideurs** : Architecte firmware, expert basse consommation
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

- **RTC externe basse consommation** (candidat **PCF8563**) comme base de temps
  fiable, maintenu et capable de réveiller le système (alarme).
- L'évolution « hors tension » est calculée par **différence de temps au
  réveil** (Δt entre dernier arrêt et réveil), et non par un fonctionnement
  continu : on lit le RTC au réveil, on applique l'évolution correspondante.
- Persistance : **NVS** pour l'état/réglages, **LittleFS** pour données et
  sauvegardes d'apps.
- Le composant RTC exact est **figé en Phase 1**.

## 4. Raisons du choix

Le calcul par Δt est le modèle le plus économe : l'appareil dort réellement,
sans tâche de fond, et « rattrape » l'évolution au réveil. Le PCF8563 offre le
meilleur compromis précision/conso/coût pour cet usage. NVS + LittleFS sont les
mécanismes éprouvés de l'écosystème.

## 5. Conséquences

### Positives
- Deep sleep réel (autonomie) tout en gardant un compagnon « vivant ».
- État des apps robuste aux coupures.

### Négatives / compromis acceptés
- Une puce I²C et une ligne d'interruption de réveil supplémentaires (budget
  GPIO/adresses I²C à arbitrer, ADR-0004/0008).

### Impacts futurs
- La HAL doit exposer une abstraction « horloge/alarme » et « stockage
  persistant » indépendantes du composant.
- Les apps conçoivent leur logique d'évolution en fonction de Δt (contrat SDK).

## 7. Réserves — à valider en Phase 1 (non figé)

> Ce qui est **acté** : le **principe** d'un RTC externe basse consommation +
> persistance, et le modèle d'évolution par **Δt au réveil**. Ce qui **reste à
> choisir/prouver** :
> - l'**opportunité** d'un RTC externe (vs RTC interne) et le **modèle exact**
>   (PCF8563 proposé, non figé) ;
> - la **précision réelle** et la **consommation** mesurées ;
> - le partage du bus I²C et le budget de broches (interruption de réveil) ;
> - la robustesse de la reprise d'état après coupure (tests).

## 6. Liens
- [ADR-0008](0008-architecture-alimentation.md) · [ADR-0007](0007-hal-et-companion-sdk.md) · [apps/tamagotchi](https://github.com/MiiK4L/companion-platform/blob/main/apps/tamagotchi/README.md)

# ADR-0004 : Cœur de calcul XIAO ESP32-S3 socketé et remplaçable

- **Statut** : Accepté (orientation) — le principe d'un cœur **remplaçable** est
  acté ; le **socketage** concret (fiabilité mécanique/électrique du contact,
  impact épaisseur) est à confirmer par prototype/mesure en Phase 1.
- **Date** : 2026-07-21
- **Décideurs** : Architecte système, expert PCB, architecte électronique
- **Phase de roadmap** : 0
- **Domaines impactés** : hardware
- **Tags** : mcu, xiao, esp32-s3, modularité, réparabilité

## 1. Contexte et problème

Le cahier des charges impose une carte principale « compatible pendant toute la
durée de vie du projet » et veut éviter des générations incompatibles. Le
module de calcul (Seeed XIAO ESP32-S3) est à la fois le composant le plus
susceptible d'évoluer (nouvelle révision, montée en gamme, obsolescence) et le
plus exposé aux pannes (USB, batterie). Faut-il le souder ou le rendre
remplaçable ?

## 2. Options étudiées

### Option A — XIAO soudé sur la carte mère
- **Avantages** : encombrement minimal, robustesse mécanique, coût connecteur nul.
- **Inconvénients** : panne du module = carte morte ; impossible de faire évoluer
  le cœur sans refaire la carte ; verrouille la plateforme sur une révision.

### Option B — XIAO socketé (sur connecteurs)
- **Avantages** : cœur **remplaçable** (panne, montée en gamme, futur module
  compatible) ; la promesse « compatible des années » devient tenable ; le cœur
  devient lui-même un élément interchangeable, cohérent avec l'esprit « console ».
- **Inconvénients** : +2–3 mm d'épaisseur, coût de deux embases, fiabilité du
  contact à soigner.

### Option C — Concevoir une carte custom autour du chip ESP32-S3 nu
- **Avantages** : contrôle total, optimisation énergie/encombrement.
- **Inconvénients** : effort de conception RF/USB/alimentation considérable,
  incompatible avec l'objectif d'une V1 raisonnable ; à envisager seulement plus
  tard si le projet le justifie.

## 3. Décision

Le cœur de calcul **XIAO ESP32-S3 est socketé** (embases femelles standard
2,54 mm sur la carte mère « CX-Bus Host »), donc **remplaçable sans dessouder**.

## 4. Raisons du choix

C'est l'option qui rend crédible la longévité et la réparabilité, pour un
surcoût minime. Elle transforme un risque (obsolescence/panne du cœur) en simple
opération de remplacement, et laisse la porte ouverte à une future carte custom
(Option C) sans bloquer la V1.

## 5. Conséquences

### Positives
- Réparabilité et évolutivité du cœur ; V1 pérenne.

### Négatives / compromis acceptés
- Légère hausse d'épaisseur (impact boîtier, Phase 5) et de coût.
- Contact socket à valider mécaniquement (vibrations, insertions).

### Impacts futurs
- Le brochage carte mère ↔ XIAO devient un mini-standard interne à documenter.
- Contrainte forte à traiter : le XIAO n'expose que **11 GPIO** — voir la
  stratégie de bus dans [ADR-0005](0005-standard-cx-bus-et-identification.md) et
  la carte mère.

## 7. Réserves — à valider en Phase 1 (non figé)

> Ce qui est **acté** : l'**exigence** d'un cœur de calcul remplaçable (non
> soudé définitivement) pour la longévité. Ce qui **reste à prouver** :
> - la fiabilité du **contact socketé** en usage nomade (vibrations, insertions,
>   résistance de contact) ;
> - l'**impact réel sur l'épaisseur** et le boîtier ;
> - l'alternative éventuelle (Option C : carte custom) reste ouverte si le
>   prototype révèle un problème.

## 6. Liens
- [ADR-0005](0005-standard-cx-bus-et-identification.md) · [ADR-0008](0008-architecture-alimentation.md)

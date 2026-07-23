<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2B — Protocoles d'essai (connecteur & endurance)

> **Statut : Brouillon (rédigés, en attente de revue de baselining).** Protocoles
> **génériques**, **indépendants de la famille** ; **aucune campagne exécutée**,
> **aucune [M]**. Seuils `[P]` / `[BL]` (voir [convention](../README.md)).

## Structure à deux niveaux

1. **Protocole générique** (ce dossier) — commun à toutes les familles ; fige la
   **méthode** (comptage, vitesses, mesure 4 fils, critères…) indépendamment du
   connecteur.
2. **Annexe de fixture & d'adaptation par famille** — **créée seulement lorsqu'une
   famille précise entre en essai** (montage, porte-DUT, guidage, câblage 4 fils
   propres à la famille). **Non créée dans cette PR** (aucune famille retenue).

## Cycle de vie (identique au Lot 2A)

```
Brouillon → Revu → Baseline d'essai → Exécuté → Rapporté
```

Le passage en **Baseline d'essai** exige : tous les `[BL]` levés **et** l'**annexe
de fixture de la famille testée** disponible. **Fiche de baseline** requise
(commit du protocole + annexe, date, approbateur, campagne autorisée, `[BL]`
levés, hash firmware/fixture) — même modèle qu'en
[Lot 2A](../../lot-2/protocols/README.md). **Aucune campagne tant qu'un `[BL]`
subsiste** ; toute modif post-baseline = **écart tracé, non rétroactif**.

## Conventions communes (à respecter par tout protocole générique)

Le protocole générique **précise déjà** :

- **Méthode de comptage des cycles** : 1 cycle = 1 insertion complète + 1 retrait
  complet (engagement/désengagement vérifiés) ; compteur mécanique + journal.
- **Vitesse d'insertion / de retrait** : imposée et bornée (`[BL]` mm/s), constante.
- **Alignement / désalignement tolérés** : offsets axiaux/angulaires appliqués et
  documentés (`[BL]`), incluant des cas **désalignés** représentatifs.
- **Charge électrique pendant les cycles** : courant `[BL]` appliqué sur les
  contacts pendant le cyclage (représentatif de `VMOD`), pour révéler l'usure.
- **Mesure 4 fils (Kelvin)** : points de mesure et méthode définis ; source de
  courant + mesure de tension séparées ; **par contact** identifié.
- **Température** : ambiante contrôlée (`[BL]` °C) ; option coins thermiques `[BL]`.
- **Nettoyage** : **autorisé/interdit explicitement** ; si autorisé, agent et
  procédure figés (sinon l'usure/oxydation n'est pas comparable).
- **Critère de défaillance** : voir [définitions des événements](event-definitions.md).
- **Règle après intermittence** : voir event-definitions (comptage, poursuite ou
  arrêt selon sévérité) — figée avant essai.
- **Traçabilité** : **DUT** et **contact mesuré** identifiés (numérotation stable),
  journalisés avec chaque relevé et donnée brute (SHA-256 au rapport).

## Protocoles génériques

| Protocole | Identifiant | État | Rapport futur | `DEC-*` |
|-----------|-------------|------|---------------|---------|
| [Endurance d'insertion](endurance-insertion.md) | PROTO-L2B-ENDUR | Brouillon | RAPP-L2B-ENDUR | `DEC-L2-002` |
| [Résistance de contact (4 fils)](contact-resistance.md) | PROTO-L2B-RCONTACT | Brouillon | RAPP-L2B-RCONTACT | `DEC-L2-002` |
| [Intermittence](intermittence.md) | PROTO-L2B-INTERM | Brouillon | RAPP-L2B-INTERM | `DEC-L2-002` |
| [Force d'insertion/extraction](insertion-extraction-force.md) | PROTO-L2B-FORCE | Brouillon | RAPP-L2B-FORCE | `DEC-L2-002` |
| [Inspection mécanique](mechanical-inspection.md) | PROTO-L2B-INSPECT | Brouillon | RAPP-L2B-INSPECT | `DEC-L2-002` |

> Le [modèle générique de rapport](../../templates/measurement-report-template.md)
> sert de base ; **aucun rapport vide n'est versionné**.

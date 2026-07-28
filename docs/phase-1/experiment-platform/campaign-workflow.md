<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Cycle d'une campagne expérimentale

> **Statut : Proposé.** Décrit le **flux** d'une campagne, du cadrage à
> l'archivage. **Renvoie** aux protocoles des lots (**autorité**) sans les
> dupliquer ; **ne décrit aucune mesure ni résultat**.

## Étapes

```text
1. Définition ──▶ 2. campaign_definition_id ──▶ 3. Acquisition ──▶ 4. Analyse
   (protocole,        (déterministe)              (driver réel        (schéma,
    DUT, config,                                   OU simulé)          incertitude)
    paramètres)                                        │
                                                       ▼
6. Archivage ◀── 5. Rapport + manifeste + SHA-256 ◀────┘
   (results/…)      (extension du template)
        │
        ▼
7. Reproduction (rejeu : mêmes entrées → mêmes artefacts, hors volatils)
```

| Étape | Entrée | Sortie | Autorité |
|-------|--------|--------|----------|
| 1. Définition | Protocole du lot + config + DUT + paramètres | Définition de campagne | [protocole du lot](../lot-2/protocols/README.md) |
| 2. Identifiant | Définition canonique | `campaign_definition_id` (déterministe) | [data-model](measurement-data-model.md) |
| 3. Acquisition | Config d'acquisition | Séries (CSV) + `run_id` unique | [reference-bench](reference-bench.md) |
| 4. Analyse | Séries | Grandeurs, incertitude, **schéma validé** | [validation-framework §4](../validation-framework.md) |
| 5. Rapport | Manifeste + métadonnées | `report.md` + `manifest.json` + hachages | [modèle de rapport](../templates/measurement-report-template.md) |
| 6. Archivage | Run complet | `results/<def_id>/<run_id>/` auto-suffisant | [data-model](measurement-data-model.md) |
| 7. Reproduction | Mêmes entrées | Artefacts identiques (hors volatils) | [architecture](architecture.md) |

## Réel vs simulé

- **Réel** (driver d'instrument) : produit des données `nature = measured`,
  éligibles au statut `[M]` et à alimenter une ADR **si** les conditions du
  [cadre §3](../validation-framework.md) sont réunies (reproductible,
  `n_campaigns ≥ 2`, alternatives comparées…).
- **Simulé** (driver de simulation) : produit des données `nature = simulated`.

> **Garde-fou.** **Une campagne simulée valide uniquement l'infrastructure
> d'outillage. Elle ne produit aucune donnée `[M]`, n'alimente aucune ADR et ne
> peut satisfaire aucun critère expérimental.** L'outil **interdit** de marquer
> `measured` un artefact `simulated`.

## Traçabilité obligatoire

Chaque run consigne les métadonnées de [reference-bench §5](reference-bench.md) :
instrument réel (marque/modèle/firmware), configuration (sondes/calibre/paramètres),
firmware testé, révision matérielle du DUT, date/heure, conditions.

## Reproductibilité (rappel)

Un résultat **reproductible** (au sens du [cadre §4](../validation-framework.md))
exige `n_campaigns ≥ 2` **indépendantes** — notion **méthodologique** qui dépasse
la seule reproductibilité **logicielle** vérifiée par l'outil (mêmes entrées →
mêmes artefacts). Le socle **outille** la seconde ; il ne **remplace pas** la
première.

## Renvois

- [Vue d'ensemble](README.md) · [Architecture](architecture.md) · [Modèle de données](measurement-data-model.md)
- [Cadre de validation](../validation-framework.md)

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Cycle d'une campagne expérimentale

> **Statut : Proposé.** Décrit le **flux** d'une campagne et le **cycle de vie de
> la preuve**. **Renvoie** aux protocoles des lots (**autorité**) sans les
> dupliquer ; **ne décrit aucune mesure ni résultat**.

## Description / implémentation / résultat

Séparation explicite (inspirée des bonnes pratiques d'ingénierie de test),
reliée par identifiants et hashes — **jamais** fondue dans un seul manifeste :

```text
Description du test        -> protocole baseliné : question, setup, seuils, verdict_rule
  (campaign-definition)        lisible et contrôlable humainement

Implémentation du test     -> firmware, script, fixture, adaptateurs, versions
  (execution-context +           peut évoluer (build_manifest = identification exacte)
   build_manifest)

Résultat du test           -> données, analyse, verdict, rapport
  (manifest, series,             immuable et traçable
   analysis, verdict, report)
```

## Étapes

```text
1. Definition ─▶ 2. campaign_definition_id ─▶ 3. Acquisition ─▶ 4. Analyse
   (question,        (deterministe)              (reel: RAW        (schema,
    verdict_rule)                                 / simule: S)      incertitude)
                                                       │
6. Archivage ◀── 5. Rapport + manifeste + verdict ◀────┘
   (results/…,       + SHA-256 (definition, contexte, series)
    autosuffisant)
        │
        ▼
7. Revue ─▶ 8. Promotion contrôlée ─▶ [M]   (uniquement pour une acquisition RÉELLE)
   (REVIEWED)   (verifie baseline, metadonnees, integrite, analyse, verdict decisif)
```

## Cycle de vie de la preuve

| État | Origine | Passage suivant |
|------|---------|-----------------|
| `RAW` | acquisition **réelle** | `mark_reviewed` → `REVIEWED` |
| `REVIEWED` | revue faite | `promote_to_measured` → `M` (si conditions réunies) |
| `M` | promotion contrôlée | preuve mesurée qualifiée |
| `S` | acquisition **simulée** | **terminal** (jamais `M`) |

**`promote_to_measured` vérifie, au minimum :** acquisition réelle · statut
`REVIEWED` · **baseline approuvée** · métadonnées obligatoires complètes ·
**artefacts intègres** (SHA-256) · **analyse exécutée** · **verdict décisif**
(`PASS`/`FAIL`). À défaut, la promotion est **refusée** (garde-fou).

## Verdict (états et sémantique)

`PASS` critères satisfaits · `FAIL` non satisfaits · `INCONCLUSIVE` données
insuffisantes/ambiguës · `INVALID` campagne invalide (banc/protocole/DUT/exécution)
· `NOT_RUN` définie mais non exécutée. **Un résultat ambigu n'est pas converti
artificiellement en succès ou échec.**

## Réel vs simulé

- **Réel** : `acquisition_nature = measured` → `RAW`. Éligible à `[M]` **après
  promotion contrôlée** et sous les conditions du
  [cadre §3](../validation-framework.md) (reproductible, `n_campaigns ≥ 2`,
  alternatives comparées…).
- **Simulé** : `acquisition_nature = simulated` → `S`.

> **Garde-fou.** **Une campagne simulée valide uniquement l'infrastructure
> d'outillage. Elle ne produit aucune donnée `[M]`, n'alimente aucune ADR et ne
> peut satisfaire aucun critère expérimental.** Son verdict est un **verdict
> d'outillage**.

## Traçabilité obligatoire (campagne réelle)

Chaque run réel consigne, via `execution-context.json`, les métadonnées de
[reference-bench §5](reference-bench.md) : instrument réel (marque/modèle/firmware),
configuration (sonde/calibre/bande passante/paramètres), **build_manifest**, DUT +
révision, fixture, conditions, étalonnage. L'exécution **refuse de finaliser** un
run réel si un champ applicable manque (`N/A` explicite sinon).

## Reproductibilité

Un résultat **reproductible** (au sens du [cadre §4](../validation-framework.md))
exige `n_campaigns ≥ 2` **indépendantes** — notion **méthodologique** distincte de
la reproductibilité **logicielle** vérifiée par l'outil (mêmes entrées → mêmes
artefacts, hors volatils). Le socle **outille** la seconde, ne **remplace pas** la
première.

## Renvois

- [Vue d'ensemble](README.md) · [Architecture](architecture.md) · [Modèle de données](measurement-data-model.md)
- [Cadre de validation](../validation-framework.md)

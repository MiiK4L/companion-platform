# ADR-0000 : Adopter les ADR pour tracer les décisions

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideurs** : Mainteneur (Mickaël), bureau d'études
- **Phase de roadmap** : 0
- **Domaines impactés** : docs, tous
- **Tags** : gouvernance, traçabilité, méthode

## 1. Contexte et problème

Le projet est conçu pour durer plusieurs années et pour pouvoir être repris par
n'importe quel ingénieur ou IA à partir du seul dépôt. Sans trace écrite des
décisions, le *pourquoi* se perd : on refait les mêmes débats, on casse des
choix structurants par ignorance de leur raison d'être, et la dette de
compréhension s'accumule. Il faut un mécanisme léger, versionné avec le code,
relu en revue, et durable.

## 2. Options étudiées

### Option A — ADR en Markdown dans le dépôt (format MADR)
- **Avantages** : versionné avec le code, relu en Pull Request, lisible hors
  ligne, indexable, repris par toute IA/ingénieur, standard reconnu.
- **Inconvénients** : discipline nécessaire pour les tenir à jour.

### Option B — Wiki / page Notion / documentation externe
- **Avantages** : édition confortable.
- **Inconvénients** : désynchronisé du code, non relu en PR, non versionné avec
  les changements qu'il justifie, dépendance à un service tiers.

### Option C — Pas de traçabilité formelle (commentaires de commit)
- **Avantages** : zéro cérémonie.
- **Inconvénients** : les raisons se diluent dans l'historique, illisibles à
  grande échelle. Inacceptable pour un projet pluriannuel.

## 3. Décision

Toute décision d'architecture importante est enregistrée sous forme d'**ADR**
en Markdown dans `docs/adr/`, au format **MADR** défini par
[`template.md`](template.md). Une ADR acceptée est **immuable** : on la remplace
par une nouvelle ADR plutôt que de la réécrire.

## 4. Raisons du choix

C'est l'option qui sert le mieux la priorité « documentation comme
fonctionnalité » et l'objectif de reprise par un tiers. Le coût (rédiger une
page) est négligeable devant la valeur (mémoire technique pérenne).

## 5. Conséquences

### Positives
- Chaque choix structurant est traçable et justifié.
- Onboarding accéléré ; cohérence des décisions dans le temps.

### Négatives / compromis acceptés
- Léger surcoût rédactionnel à chaque décision importante.

### Impacts futurs
- Toutes les décisions des phases 1–15 suivront ce processus.

## 6. Liens
- [Registre des ADR](README.md) · [Template](template.md)

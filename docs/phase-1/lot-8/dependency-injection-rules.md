<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 8 — Règles d'injection de dépendances (ownership, durée de vie, composition root)

> **Statut : Ouvert / Proposé.** Règles **explicites** régissant le câblage du
> squelette. Code de référence : `firmware/host-skeleton/composition/`.

## 1. Composition root — le seul endroit qui connaît le concret

- La **composition root** (`composition/composition_root.c`) est **l'unique** lieu
  qui référence des **implémentations concrètes** (ici : mocks host).
- Elle **construit** les adaptateurs, puis **injecte leurs ports** dans les
  services. Aucun service, port ou modèle ne référence la composition.
- Une composition **host** (mocks) et une composition **cible** (adaptateurs
  ESP-IDF, **hors périmètre** de ce lot) partagent les **mêmes** services/ports.

## 2. Ownership (possession)

- La composition root **possède** les adaptateurs (ils sont **membres** de la
  structure de composition — `HostComposition`).
- Un service **ne possède pas** les contextes `self` des ports qu'il reçoit : il
  en détient une **copie de l'interface** (petite : `self` + pointeurs), pas la
  ressource.
- **Aucune allocation dynamique** dans le squelette : tout est en **stockage
  automatique** (membres de `HostComposition`), ce qui rend l'ownership trivial et
  vérifiable.

## 3. Durée de vie (lifetime)

- La durée de vie des **adaptateurs** (possédés par la composition) doit
  **couvrir** celle des **services** qui utilisent leurs ports.
- Concrètement : tant que `HostComposition c` est vivant, `c.app_manager` peut
  utiliser `c.storage`/`c.runtime`/`c.log` en toute sûreté.
- **Interdit** : passer un port dont le `self` pointe sur une variable locale qui
  sort de portée avant le service (règle documentée, non applicable au squelette
  qui n'en crée pas).

## 4. Sens des dépendances (rappel)

```
composition root ──▶ services ──▶ ports ◀── adaptateurs ──▶ (plateforme)
        (connaît le concret)   (abstrait)     (concret)
```

- Flèche = « dépend de ». Les **services** dépendent des **ports** (abstrait) ;
  les **adaptateurs** implémentent les ports. La composition root câble le tout.
- Vérifié mécaniquement : [règle de dépendances](dependency-rules.md).

## Alimente

- Démonstration que l'architecture est **implémentable et testable** ; règles
  **réutilisables** pour la composition **cible** (ultérieure).

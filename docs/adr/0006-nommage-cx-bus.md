<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# ADR-0006 : Nommage du standard — CX-Bus (abandon de « MCP »)

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideur** : Mainteneur du projet (MiiK4L)
- **Expertises consultées** : Mainteneur, expert documentation
- **Phase de roadmap** : 0
- **Domaines impactés** : cx-bus, docs
- **Tags** : nommage, marque, standard

## 1. Contexte et problème

Le cahier des charges proposait de nommer le standard de modules « MCP »
(Mickaël Companion Platform). Or, en 2026, **« MCP » désigne le Model Context
Protocol** d'Anthropic, devenu un standard largement adopté de l'industrie IA.
Réutiliser ce sigle pour un projet **open source** destiné à être référencé,
recherché et repris pose un problème durable : collision de recherche/SEO,
confusion permanente dans la documentation (« quel MCP ? »), ambiguïté pour tout
contributeur ou IA. Le coût de correction est nul aujourd'hui, élevé plus tard.

## 2. Options étudiées

### Option A — Conserver « MCP »
- **Avantages** : aucun changement.
- **Inconvénients** : collision majeure avec un standard connu, confusion
  documentaire et de recherche pérenne. Mauvais pour un projet open source.

### Option B — Renommer en « CX-Bus » (Companion eXpansion Bus)
- **Avantages** : neutre, descriptif (bus d'extension), sans collision connue,
  se décline proprement (Module / Host / Manifest / SDK).
- **Inconvénients** : renommage à effectuer (trivial en Phase 0, avant tout
  code).

### Option C — Autre nom à définir ultérieurement
- **Avantages** : liberté.
- **Inconvénients** : retarde une décision qui doit être prise avant d'écrire la
  spec et le code.

## 3. Décision

Le standard d'extension est nommé **CX-Bus** (*Companion eXpansion Bus*). Le
**projet global** conserve le nom **Companion Platform**. CX-Bus désigne
uniquement le standard : connecteur, brochage, alimentation, détection,
identification, protocole, format mécanique et règles de compatibilité.

Nomenclature versionnée :
- **CX-Bus 1.0** — version du standard ;
- **CX-Bus Module** — la cartouche d'extension ;
- **CX-Bus Host** — la carte mère qui reçoit les modules ;
- **CX-Bus Manifest** — le descripteur d'identification (EEPROM) ;
- **CX-Bus SDK** — la surface d'outils/API pour créer des modules.

## 4. Raisons du choix

Éliminer une source de confusion structurelle dès le premier jour, à coût nul,
sert directement la lisibilité et l'adoption open source du projet. C'est
exactement le type de correction que l'on veut faire avant, pas après.

## 5. Conséquences

### Positives
- Identité claire et recherchable ; nomenclature extensible.

### Négatives / compromis acceptés
- Aucun réellement (décision prise avant toute implémentation).

### Impacts futurs
- Toute la documentation, la spec et le code emploient « CX-Bus ».

## 6. Liens
- [ADR-0005](0005-standard-cx-bus-et-identification.md)

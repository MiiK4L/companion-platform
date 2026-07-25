<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 7 — Modèle de persistance (*Proposé*, `DEC-L7-002`)

> **Statut : Ouvert / Proposé.** **Trois axes d'architecture distincts** : le
> **support de persistance** (*où*), le **modèle de cohérence** (*comment*) et la
> **politique d'écriture** (*quand*) — comparés **séparément** (à ne pas mélanger).
> Stocke **heure + état**. Valeurs `[DS]/[H]/[BL]` ; **aucune `[M]`**. → **ADR future
> non réservée**.

## Axe 1 — Support de persistance

| Support | Principe | Remarque |
|---------|----------|----------|
| **(P1) NVS** (clé-valeur) | stockage clé-valeur sur flash (ESP-IDF) | simple ; adapté petits états |
| **(P2) LittleFS** (système de fichiers) | FS à **wear-leveling**, résistant aux coupures | fichiers ; plus riche |
| **(P3) Secteur brut / EEPROM externe** | écriture directe d'une zone | contrôle fin ; à gérer soi-même |

## Axe 2 — Modèle de cohérence (**comment** garantir l'intégrité, orthogonal au support)

| Modèle | Principe | Robustesse coupure |
|--------|----------|--------------------|
| **(M1) Journal (log)** | append-only + rejeu au boot | reprise après coupure |
| **(M2) Transaction** | tout-ou-rien (commit) | forte cohérence |
| **(M3) Double copie A/B** | bascule atomique entre 2 copies | retour à la dernière valide |
| **(M4) Checkpoint** | instantanés périodiques + delta | compromis taille/fréquence |
| **(M5) Checksum / génération** | détection de corruption / MAJ interrompue | détection, pas correction seule |

## Axe 3 — Politique d'écriture (**quand** décider d'écrire, indépendant des axes 1 et 2)

| Politique | Principe | Vigilance |
|-----------|----------|-----------|
| **(W1) Immédiate** | écrit à chaque changement | usure flash ↑ |
| **(W2) Différée / groupée** | regroupe puis écrit | perte des derniers changements possible |
| **(W3) À l'événement** | écrit sur événements clés (arrêt, veille…) | dépend de la détection d'événement |
| **(W4) Périodique** | écrit à intervalle | compromis usure/fraîcheur |
| **(W5) Au repos** | écrit quand le système est inactif | latence de persistance |

> **Trois axes indépendants** : le **support** (axe 1, *où*), le **modèle de
> cohérence** (axe 2, *comment*) et la **politique d'écriture** (axe 3, *quand*) se
> combinent librement (ex. LittleFS + journal + écriture différée). La comparaison
> les traite **séparément** — **à ne pas mélanger**.

## Reprise après coupure brutale (exigence transverse)

| Exigence | Contenu | Étiquette |
|----------|---------|-----------|
| Détection d'écriture interrompue | distinguer coupure vs corruption (checksum/génération) | **[H]** |
| Retour à un état cohérent | dernière copie/transaction valide | **[BL]** |
| Intégrité (checksum) | 0 corruption non détectée | **[P]/[BL]** |
| Compromis politique d'écriture (axe 3) | fréquence vs usure flash vs perte tolérée | **[BL]** |

## Stockage de l'heure

L'**heure** (et son **état de validité**, cf. [base de temps](timebase-comparison.md))
est **persistée** selon le même modèle ; une reprise **ne doit jamais** restaurer
une heure **fausse** marquée « valide ».

## Alimente

- **`DEC-L7-002`** — **support** et **modèle de cohérence** arbitrés **après**
  mesures (reprise sur coupures). **Aucun support ni modèle retenu.**

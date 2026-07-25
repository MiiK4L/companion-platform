<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 7 — Modèle de persistance (*Proposé*, `DEC-L7-002`)

> **Statut : Ouvert / Proposé.** **Deux axes d'architecture distincts** : le
> **support de persistance** et le **modèle de cohérence** — comparés
> **séparément** (à ne pas mélanger). Stocke **heure + état**. Valeurs `[DS]/[H]/[BL]` ;
> **aucune `[M]`**. → **ADR future non réservée**.

## Axe 1 — Support de persistance

| Support | Principe | Remarque |
|---------|----------|----------|
| **(P1) NVS** (clé-valeur) | stockage clé-valeur sur flash (ESP-IDF) | simple ; adapté petits états |
| **(P2) LittleFS** (système de fichiers) | FS à **wear-leveling**, résistant aux coupures | fichiers ; plus riche |
| **(P3) Secteur brut / EEPROM externe** | écriture directe d'une zone | contrôle fin ; à gérer soi-même |

## Axe 2 — Modèle de cohérence (orthogonal au support)

| Modèle | Principe | Robustesse coupure |
|--------|----------|--------------------|
| **(M1) Écriture immédiate** | écrit à chaque changement | simple ; usure flash ↑ |
| **(M2) Écriture différée** | regroupe/écrit périodiquement | perte des derniers changements possible |
| **(M3) Journal (log)** | append-only + rejeu au boot | reprise après coupure |
| **(M4) Transaction** | tout-ou-rien (commit) | forte cohérence |
| **(M5) Double copie A/B** | bascule atomique entre 2 copies | retour à la dernière valide |
| **(M6) Checkpoint** | instantanés périodiques + delta | compromis taille/fréquence |

> Le **support** (axe 1) et le **modèle de cohérence** (axe 2) sont **indépendants** :
> un même support (ex. LittleFS) peut porter **plusieurs** modèles (journal, A/B,
> checkpoint). La comparaison les traite **séparément**.

## Reprise après coupure brutale (exigence transverse)

| Exigence | Contenu | Étiquette |
|----------|---------|-----------|
| Détection d'écriture interrompue | distinguer coupure vs corruption (checksum/génération) | **[H]** |
| Retour à un état cohérent | dernière copie/transaction valide | **[BL]** |
| Intégrité (checksum) | 0 corruption non détectée | **[P]/[BL]** |
| Politique d'écriture | fréquence vs usure flash vs perte tolérée | **[BL]** |

## Stockage de l'heure

L'**heure** (et son **état de validité**, cf. [base de temps](timebase-comparison.md))
est **persistée** selon le même modèle ; une reprise **ne doit jamais** restaurer
une heure **fausse** marquée « valide ».

## Alimente

- **`DEC-L7-002`** — **support** et **modèle de cohérence** arbitrés **après**
  mesures (reprise sur coupures). **Aucun support ni modèle retenu.**

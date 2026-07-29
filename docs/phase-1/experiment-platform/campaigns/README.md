<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Campagnes expérimentales

> **Statut : Proposé.** Cette section rassemble les **définitions** de campagnes.
> Une définition **décrit** ; elle n'exécute rien et ne produit aucune donnée.

Le [cycle d'une campagne](../campaign-workflow.md) sépare strictement
**description** → **implémentation** → **résultat**. Les pages ci-dessous
relèvent de la **description** ; les résultats (`RAW`/`REVIEWED`/`M`) vivent dans
les archives de runs, jamais dans la documentation.

## EXP-L1-BRINGUP-001 — contention SPI écran / CX-Bus

Question `DEC-L1-001` (budget GPIO/bus, option de partage) et risque **R2**.

| Page | Contenu |
|------|---------|
| [Définition de campagne](EXP-L1-BRINGUP-001.md) | question, variantes, modes, observables, protocole, verdict |
| [Baseline brouillon](baseline-draft.md) | champs `[BL]` proposés — **`draft`, n'autorise aucun run** |
| [Brochage candidat](pinout-candidate.md) | câblage **candidat**, non figé |
| [Matériel et actions physiques](materials-and-physical-actions.md) | matériel par rôle, procédure, traçabilité |
| [Gate matériel](hardware-gate.md) | checklist **bloquante** avant toute exécution (B4) |

**État : définition seule (lot B3).** Aucune mesure, aucun flash, aucun run,
aucune donnée `[M]`, aucune ADR. `DEC-L1-001` reste **Ouvert**.

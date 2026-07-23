<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 2A — Protocoles d'essai (sûreté électrique)

> **Statut : Proposé.** Protocoles **figés avant essai** (condition d'un résultat
> reproductible — voir [cadre de validation](../../validation-framework.md) §4).
> Ils **spécifient** les campagnes ; **aucune n'est exécutée** dans cette PR et
> **aucune valeur mesurée [M]** n'y figure. Les seuils sont **proposés [P]** ou
> issus de datasheet **[DS]** / calcul **[C]** / hypothèse **[H]** (voir
> [convention](../README.md)).

## Contenu imposé de chaque protocole

Conformément au [modèle de protocole](../../templates/test-protocol-template.md),
chaque fiche précise **avant essai** :

- conditions initiales ;
- instrumentation requise ;
- montage / fixture ;
- limites de courant et de tension ;
- critères d'**arrêt immédiat** ;
- procédure de **remise en état** entre deux campagnes ;
- éléments **susceptibles d'être détruits** ;
- **données brutes attendues** ;
- **`n_dut` / `n_runs` / `n_campaigns`** et leur justification ;
- **décision `DEC-*` alimentée**.

## Matrice de traçabilité — protocole → rapport futur → décision

> **Aucun rapport concret n'est versionné** (pas de gabarit vide). Le rapport
> sera créé **au démarrage de la campagne** correspondante, à partir du
> [modèle générique](../../templates/measurement-report-template.md), sous
> l'identifiant `RAPP-*` indiqué.

| Protocole | Identifiant | Rapport futur (`RAPP-*`) | `DEC-*` alimentée | Risque(s) |
|-----------|-------------|--------------------------|-------------------|-----------|
| [Inrush & rampe](inrush-et-rampe.md) | PROTO-L2A-INRUSH | RAPP-L2A-INRUSH | `DEC-L2-001` | R5 |
| [Court-circuit](court-circuit.md) | PROTO-L2A-SHORT | RAPP-L2A-SHORT | `DEC-L2-001` | R5 |
| [Bus-stuck](bus-stuck.md) | PROTO-L2A-STUCK | RAPP-L2A-STUCK | `DEC-L2-001` | collisions I²C |
| [Hot-plug (injection)](hot-plug-injection.md) | PROTO-L2A-HOTPLUG | RAPP-L2A-HOTPLUG | `DEC-L2-001` | R5, R2, back-powering |
| [Retrait en transaction](retrait-en-transaction.md) | PROTO-L2A-REMOVE | RAPP-L2A-REMOVE | `DEC-L2-001` | R5 |

## Note (frontière Lot 2B)

Aucun protocole d'**endurance mécanique de connecteur** ni de **résistance de
contact** ne figure ici : ils relèvent du **Lot 2B**. Le hot-plug est étudié via
un **banc d'injection** indépendant du connecteur final.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Gate matériel — conditions d'entrée de B4

> **Statut : normatif pour B4.** Cette checklist **empêche** B4 de lancer une
> campagne tant que les prérequis hérités de **B2** ne sont pas **vérifiés et
> consignés**. Aucun de ces points n'est satisfait à la fin de B3 : le firmware
> livré en B2 fournit un **contrat et un squelette**, pas une implémentation
> matérielle (cf. `firmware/experiment-bench/docs/target-comparison.md`).

## Checklist bloquante

Chaque ligne doit être **vérifiée**, **datée** et **tracée** (artefact, capture
ou log archivé). Tant qu'une seule reste ouverte, **aucun run de mesure** n'est
autorisé.

| # | Prérequis | Preuve attendue |
|---|-----------|-----------------|
| 1 | **Build Pico SDK réel** du firmware module | log de build + empreinte de l'artefact, versions de toolchain figées |
| 2 | **PIO SPI-esclave fonctionnel** | capture analyseur d'un échange cadencé par le maître |
| 3 | **Host ↔ Slave nominal validé** | une transaction complète aboutit de bout en bout |
| 4 | **IRQ sortante vérifiée** | assertion **et** relâchement observés sur la ligne, corrélés à la transaction |
| 5 | **Trame correcte acceptée** | trame valide → `ACK_OK`, compteur `tx_ok` incrémenté |
| 6 | **Trame CRC corrompue rejetée** | corruption réelle d'un octet → rejet, `crc_errors` incrémenté |
| 7 | **Artefacts bruts importables via B1** | import `manual-import` réussi, empreintes stables, archive vérifiée (`verify_run`) |
| 8 | **Synchronisation temporelle validée** | horodatages hôte/module corrélés, dérive bornée et consignée (BL-005) |
| 9 | **Surcoût d'instrumentation borné** | mesure du surcoût, conforme à BL-006 |

Les points 5 et 6 constituent l'**auto-test du banc** : ils prouvent que la
chaîne de détection d'intégrité fonctionne **avant** de mesurer, ce qui est la
raison pour laquelle la campagne elle-même n'injecte **aucune** faute (BL-209).

## Point ouvert à trancher en B4

Le moteur portable livré en B2 exécute **un** flux. Le mode `concurrent` en
exige **deux** simultanés (afficheur + module). B4 devra déterminer si cela
requiert une **évolution du cœur** — auquel cas cette évolution fera l'objet
d'une **PR séparée de la campagne**, conformément à la règle « ne jamais mélanger
évolution du socle et campagne ».

## Après le gate

Une fois les 9 points satisfaits :

1. **revue** de la baseline brouillon ;
2. **instanciation** des valeurs restées ouvertes (BL-104, BL-105) à partir des
   modes isolés ;
3. **approbation** de la baseline (`status: approved`, `approved_by`,
   `approved_at`) ⇒ nouvelle `baseline_version` ;
4. **exécution** des runs, dans l'ordre alterné/randomisé prévu au protocole.

Aucune donnée `[M]` ne peut exister avant l'étape 4, et un run `[M]` reste un
**run individuellement qualifié** — pas une preuve suffisante pour arbitrer
`DEC-L1-001`.

<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Matériel et actions physiques — EXP-L1-BRINGUP-001

> **Statut : Proposé — définition seule.** Aucune action de cette page n'est
> exécutée en B3. Elle décrit ce que **B4** devra faire, dans l'ordre, une fois
> le [gate matériel](hardware-gate.md) franchi.
>
> **Aucun composant ni MPN n'est retenu.** Les éléments sont décrits par leur
> **rôle** et leurs **caractéristiques minimales**.

## 1. Matériel requis (par rôle)

| Rôle | Caractéristiques minimales | Quantité |
|------|---------------------------|----------|
| Carte hôte | véhicule de référence XIAO ESP32-S3, USB-C, SPI maître | 1 |
| Carte module | RP2040 (cible **candidate**), PIO disponible pour SPI esclave | 1 |
| Charge afficheur | périphérique SPI de type afficheur, CS dédié | 1 |
| Alimentation | 3,3 V stable dans les bornes BL-001, courant suffisant | 1 |
| Câblage | fils courts (≤ 100 mm, BL-003), masse commune | — |
| Instrumentation temporelle | analyseur logique ≥ 4 voies, échantillonnage ≫ fréquence SPI | 1 |
| Poste de travail | toolchains Pico SDK **et** ESP-IDF installées (versions figées) | 1 |

> La disponibilité réelle de ces éléments relève de l'**inventaire**, distinct
> des **capacités du banc de référence** (cf.
> [banc de référence](../reference-bench.md)).

## 2. Actions physiques (ordre d'exécution en B4)

1. **Hors tension.** Vérifier que les deux cartes sont non alimentées.
2. **Câbler la masse en premier** (point unique, BL-002), puis l'alimentation.
3. **Câbler le bus** selon le [brochage candidat](pinout-candidate.md), variante
   du jour ; vérifier la longueur des liaisons (BL-003).
4. **Câbler la ligne IRQ** module → hôte, dans le bon sens (sortie module,
   entrée hôte).
5. **Poser les sondes** sur les points de mesure déclarés.
6. **Consigner le câblage** : photo + table des liaisons effectives, jointe au
   contexte d'exécution du run.
7. **Mettre sous tension** ; vérifier la tension d'alimentation (BL-001) et la
   consigner.
8. **Flasher** hôte et module avec les builds dont l'empreinte est enregistrée
   dans le `build_manifest`.
9. **Exécuter l'auto-test du banc** (cf. gate matériel) avant tout run de mesure.
10. **Exécuter le run** du cas planifié, selon l'ordre alterné/randomisé.
11. **Remise à zéro complète** entre deux runs : hors tension, puis reprise à
    l'étape 7.
12. **Consigner** température ambiante, ordre effectif, incidents.

## 3. Sécurité et intégrité du banc

- Ne jamais modifier le câblage sous tension.
- Vérifier l'orientation de l'alimentation avant mise sous tension.
- Toute modification de câblage **en cours de campagne** invalide les runs
  suivants tant que le câblage n'est pas re-consigné.
- Un câblage divergeant de la table consignée rend le run `INVALID` (et non
  `FAIL`).

## 4. Traçabilité attendue par run

Câblage effectif · variante et mode · ordre effectif d'exécution · tension
mesurée · température ambiante · empreintes de build (hôte et module) · versions
de toolchain · identité de l'opérateur · incidents éventuels. Ces éléments
alimentent l'`execution-context` exigé par le socle pour tout run réel.

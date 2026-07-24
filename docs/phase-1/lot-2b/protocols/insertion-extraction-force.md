<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# PROTO-L2B-FORCE — Force d'insertion / extraction & rétention

> **Statut : Brouillon (en attente de baselining + annexe fixture par famille).**
> Générique. [Conventions communes](README.md) ·
> [définitions d'événements](event-definitions.md). Seuils `[P]/[BL]` ; **aucune
> [M]**. Modèle : [protocole de test](../../templates/test-protocol-template.md).

## Lot & décision visée

Lot 2B · alimente **`DEC-L2-002`** (ergonomie & rétention).

## Objectif (une question mesurable)

Les **forces d'insertion et d'extraction** restent-elles dans une plage
**ergonomique** et **stable** au cours de l'endurance, et la **rétention**
reste-t-elle suffisante (anti-arrachement) ?

## Conditions & instrumentation

Dynamomètre / capteur de force sur banc de course contrôlée (vitesse `[BL]`) ;
mesure des forces à l'**insertion**, à l'**extraction** et de la **force de
rétention** (résistance à l'arrachement). Fixture famille = **annexe à l'essai**.

## Seuils de réussite / échec chiffrés

| Grandeur | Seuil | Étiquette |
|----------|-------|-----------|
| Force d'insertion | dans `[BL]`–`[BL]` N (ergonomique) | **[BL]** |
| Force d'extraction | dans `[BL]`–`[BL]` N | **[BL]** |
| Force de rétention (anti-arrachement) | ≥ `[BL]` N | **[BL]** |
| Dérive des forces sur endurance | ≤ `[BL]` % | **[BL]** |

### Champs à finaliser au baselining (`[BL]`)

- **Plages ergonomiques** d'insertion/extraction (N) et **rétention minimale** (N).
- **Vitesse** de course et **dérive** admissible sur l'endurance.

## Plan d'échantillonnage

- **`n_dut`** ≥ 2 · **`n_runs`** ≥ `[BL]` mesures par état · **`n_campaigns`** ≥ 2 ·
  mesures **avant / pendant / après** l'[endurance](endurance-insertion.md).
- **Méthode** : min/max/percentiles ; règle d'aberrants pré-définie.

## Critères d'arrêt immédiat

Force hors plage franche (blocage, insertion impossible) ; rupture ; capteur de
force saturé.

## Remise en état / éléments destructibles

Usure possible de l'assemblage ; **DUT neufs par campagne** (paire complète, cf.
[définition du DUT](README.md)) — **pas** de substitution en cours de campagne ;
ré-étalonner le capteur de force.

## Données brutes attendues

Courbes force-déplacement (insertion/extraction), force de rétention, par DUT et
par palier d'endurance ; SHA-256 par fichier au rapport.

## Sécurité opérateur

Banc de force motorisé : protections, arrêt d'urgence, pas de main dans la course.

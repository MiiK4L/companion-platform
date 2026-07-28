<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Banc de référence — capacités, équivalence & traçabilité

> **Statut : Proposé — cadre commun révisable.** Document **unique** pour les
> **capacités minimales** attendues des instruments, la **règle d'équivalence** et
> les **exigences de traçabilité** communes à toutes les campagnes. Les modèles
> cités sont des **exemples** validant l'existence d'une capacité — **jamais une
> recommandation d'achat ni le matériel réellement possédé** (voir §2).

## 1. Capacités minimales requises (exigences générales)

Un instrument est **compatible** s'il satisfait les **capacités minimales** de sa
classe. Ces exigences sont **générales et stables** ; elles ne dépendent d'aucun
modèle. Les seuils précis restent `[BL]` (fixés au baselining, selon le
protocole). Les modèles entre parenthèses sont des **exemples** (§3).

| Classe d'instrument | Capacités minimales (grandeurs à spécifier `[BL]`) | Exemples (illustratifs) |
|---------------------|----------------------------------------------------|-------------------------|
| Oscilloscope | Bande passante, fréquence d'échantillonnage, profondeur mémoire, nb voies, résolution verticale | 2 séries de gammes différentes |
| Sonde de courant | Plage de courant, bande passante, sensibilité, dérive | — |
| Alimentation de laboratoire | Tension/courant max, **limitation de courant** réglable, résolution de consigne/mesure, faible bruit | — |
| Analyseur logique | Nb de voies, fréquence d'échantillonnage, seuils logiques, profondeur | — |
| Multimètre (DMM) | Résolution (digits), précision DC/AC, plages courant/tension/résistance | — |
| Cycleur mécanique | Course, force, comptage de cycles, répétabilité | — |
| Référence de temps | Stabilité (ppm), traçabilité, sortie exploitable | — |

> Les grandeurs ci-dessus sont **méthodologiques** : chaque protocole précise
> **lesquelles** sont critiques pour sa mesure et **fixe leurs seuils** `[BL]`.

## 2. Équivalence instrumentale (règle)

> **Deux instruments sont équivalents s'ils satisfont les capacités minimales
> définies au §1.** Les modèles cités ne sont que des **exemples** validant que
> ces capacités existent sur le marché ; ils **ne constituent pas une
> recommandation d'achat** et **ne présument pas** du matériel réellement utilisé.

- Substituer un instrument par un **équivalent** (capacités satisfaites) **ne
  change pas** l'exigence ni la validité d'une campagne — à condition de
  **consigner l'instrument réel** (§4).
- Une capacité **non satisfaite** rend l'instrument **non compatible** pour la
  grandeur concernée : le fait doit être **tracé**, pas contourné.

## 3. Exemples de modèles (illustratifs uniquement)

Cette liste **n'est pas** un inventaire ni un choix. Elle donne **2–3 modèles par
classe**, seulement pour montrer que la capacité existe. *(À compléter au fil de
l'étude ; l'ajout d'un exemple ne vaut pas décision.)*

- **Oscilloscope** : classe « entrée de gamme USB » ; classe « bench 4 voies ».
- **Sonde de courant** : shunt + amplificateur ; sonde à effet Hall.
- **Alimentation** : alim linéaire à limitation ; alim programmable double sortie.
- **Analyseur logique** : analyseur USB multi-voies ; module intégré à l'oscilloscope.

> Aucun MPN n'est **retenu**. La sélection éventuelle d'un instrument est un choix
> d'**équipement de banc**, distinct de toute `DEC-*` (qui portent sur le
> **produit**, pas sur l'instrumentation).

## 4. Inventaire du banc réellement disponible (renseigné à l'usage)

> **Section vivante et modifiable**, **indépendante** des exigences du §1.
> Renseignée **plus tard**, avec les instruments **effectivement utilisés**. La
> modifier **ne change pas** les capacités requises.

| Instrument réel (marque / modèle) | Classe (§1) | Capacités constatées | Firmware / version | Notes |
|-----------------------------------|-------------|----------------------|--------------------|-------|
| *(à renseigner)* | — | — | — | — |

## 5. Exigences de traçabilité communes (toutes campagnes)

Chaque campagne **doit** consigner dans son rapport (champs portés par le
manifeste, cf. [measurement-data-model](measurement-data-model.md)) :

- **Instrument réellement utilisé** : marque, modèle, **version firmware** (si applicable) ;
- **Configuration** : sondes, calibre/gamme, paramètres importants (couplage, filtre…) ;
- **Version du firmware testé** (DUT) ;
- **Révision matérielle du DUT** ;
- **Date/heure** ;
- **Conditions de test pertinentes** : température, alimentation, environnement.

> Ces métadonnées sont **obligatoires** pour qu'un résultat soit **rejouable** et
> **comparable** ; elles complètent (sans les remplacer) les règles de preuves du
> [cadre de validation](../validation-framework.md).

## Renvois

- [Modèle de données](measurement-data-model.md) · [Cycle d'une campagne](campaign-workflow.md)
- [Cadre de validation](../validation-framework.md) · [Sourcing & BOM](../sourcing-and-bom.md)

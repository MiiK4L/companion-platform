# ADR-0010 : Stratégie de licences multi-composants

- **Statut** : Accepté
- **Date** : 2026-07-21
- **Décideurs** : Mainteneur, expert documentation, expert juridique open source
- **Phase de roadmap** : 0
- **Domaines impactés** : tous
- **Tags** : licences, open-source, gouvernance

## 1. Contexte et problème

Le projet mêle firmware, matériel (schémas, PCB, boîtier), documentation et
identité visuelle. Ces catégories n'ont pas les mêmes bonnes licences : une
licence logicielle ne protège pas correctement du matériel, et l'identité de
marque a des besoins différents des sources. Il faut une stratégie claire qui
favorise l'adoption tout en protégeant l'écosystème et la marque, et qui
autorise les contributions tierces.

## 2. Options étudiées

- **Tout permissif (MIT / CERN-OHL-W / CC-BY)** : adoption maximale, mais un
  dérivé peut se refermer entièrement (firmware comme matériel).
- **Tout copyleft fort (GPL / CERN-OHL-S / CC-BY-SA)** : protection maximale de
  l'écosystème, mais peut freiner certains usages et modules fermés.
- **Mixte, adapté par composant** : équilibre adoption/protection, chaque brique
  sous la licence la plus pertinente.

## 3. Décision

Stratégie **multi-licences par composant** :

| Composant | Licence |
|-----------|---------|
| Firmware, OS, SDK, outils officiels (`firmware/`, `apps/` officielles, `tools/`) | **Apache-2.0** |
| Matériel officiel : schémas, PCB, boîtier (`hardware/`, `case/`, `modules/` officiels) | **CERN-OHL-S-2.0** |
| Documentation (`docs/`) | **CC-BY-4.0** |
| Identité visuelle (nom, logo, mascotte, illustrations officielles) | **Réservée / Tous droits réservés** au début |

Règles complémentaires :
- Les **apps et modules tiers** peuvent adopter **la licence de leur choix** ;
  le **cœur** reste Apache-2.0 et le **matériel officiel** CERN-OHL-S.
- Déclaration de licence **par fichier via en-têtes SPDX**
  (`SPDX-License-Identifier: …`).

## 4. Raisons du choix

Apache-2.0 favorise l'adoption du firmware/SDK et protège des brevets (mieux que
MIT) — clé pour un écosystème d'apps/modules. CERN-OHL-S garde les dérivés
matériels ouverts (réciprocité forte), cohérent avec un projet open hardware de
référence. CC-BY protège la paternité de la doc sans la verrouiller. Réserver
l'identité visuelle protège la marque le temps que le projet mûrisse.

## 5. Conséquences

### Positives
- Adoption large côté logiciel, écosystème matériel protégé, marque maîtrisée.

### Négatives / compromis acceptés
- Complexité : plusieurs licences à gérer → atténuée par les en-têtes SPDX et
  `LICENSING.md`.

### Impacts futurs
- La CI pourra vérifier la présence des en-têtes SPLD/SPDX (Phase ultérieure).
- Une future ouverture de l'identité visuelle fera l'objet d'une nouvelle ADR.

## 6. Liens
- `LICENSING.md` (racine) · [ADR-0012](0012-monorepo-et-versioning.md)

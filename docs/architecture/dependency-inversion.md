# Modèle de dépendances : ports & adaptateurs

C'est le principe directeur de l'architecture firmware : il vise la longévité, la
testabilité et l'évolutivité de la plateforme. Décision fondatrice :
[ADR-0007](../adr/0007-hal-et-companion-sdk.md).

Le firmware suit un modèle **ports / adaptateurs** (hexagonal), et **non** une
pile de couches strictes. Les interfaces abstraites (les *ports*) sont au centre
et **ne dépendent d'aucune implémentation concrète** ; les *adaptateurs*
concrets dépendent des ports qu'ils implémentent. C'est le sens correct de
l'inversion des dépendances : *les abstractions ne dépendent pas des détails,
les détails dépendent des abstractions.*

## Le modèle

```
        Applications
   (Lua chargées par le runtime  ·  ou natives compilées dans l'image)
                    │  dépendent uniquement de
                    ▼
     Companion SDK  — contrat / API applicative
     (façade fournie par les services ; ne dépend pas des drivers)
                    │  dépend de
                    ▼
     Services métier & plateforme
     (Module Manager, App Manager, UI, Power, Storage, Connectivity, Companion)
                    │  dépendent d'interfaces abstraites
                    ▼
     Ports de plateforme  (interfaces abstraites, PORTABLES, sans ESP-IDF)
     display · input · clock · storage · bus · power · scheduler …
                    ▲
                    │  implémentés par
     ┌──────────────┴───────────────┐
     Adaptateurs cible               Adaptateurs host
     ESP32-S3 / ESP-IDF              (mocks/simulateurs pour les tests PC)
     │  s'appuient sur
     ▼
     Drivers matériels · ESP-IDF · FreeRTOS · silicium
```

Le **point de composition** (composition root), exécuté au **démarrage**,
choisit les adaptateurs concrets et les injecte derrière les ports que les
services utilisent. C'est le seul endroit qui « connaît » à la fois les
interfaces et leurs implémentations.

## Les 8 règles de dépendance

1. **Les applications ne dépendent que du Companion SDK.**
2. **Le Companion SDK est une façade/un contrat fourni par les services** ; il ne
   dépend pas directement des drivers.
3. **Les services dépendent d'interfaces abstraites (les ports)** — jamais
   d'ESP-IDF, de FreeRTOS ni de pilotes concrets.
4. **Les adaptateurs et drivers concrets dépendent des interfaces qu'ils
   implémentent** (les ports).
5. **Le paquet qui contient les ports reste portable** et ne connaît pas
   ESP-IDF.
6. **Le point de composition assemble ports et implémentations au démarrage.**
7. **Une dépendance peut contourner un niveau** lorsqu'il n'existe aucune raison
   fonctionnelle de le traverser (il n'y a pas de règle artificielle « aucun
   niveau ne saute un autre »). Ce qui reste interdit, ce sont les dépendances
   d'une abstraction vers un détail concret (règles 3-5).
8. **La portabilité est un objectif, pas une garantie absolue.**

> ⚠️ Ne pas confondre **Companion SDK** (l'API vue par les apps) et **CX-Bus SDK**
> (l'outillage pour *créer des modules matériels* conformes au standard). Voir le
> [glossaire](glossary.md).

## Apps natives vs apps Lua

Les deux ne voient que le **Companion SDK**, mais leur cycle de vie diffère :

- **Apps natives** : écrites en C, elles utilisent l'API publique du SDK mais
  sont **compilées dans l'image firmware**. Les ajouter/mettre à jour implique de
  reconstruire et reflasher.
- **Apps Lua** : **chargées à l'exécution** par le runtime (App Manager,
  [ADR-0002](../adr/0002-modele-execution-applications.md)) ; leur faisabilité et
  leurs limites d'installation dynamique restent à valider par prototype (P8).

## Ce que le modèle vise

L'objectif est de **préserver la compatibilité applicative dans les limites du
contrat et des capacités garanties par le Companion SDK**, lorsqu'une pièce
bas niveau change. Ce n'est pas une garantie absolue « zéro impact » : un
changement de silicium, de RTOS, d'écran ou de runtime demande d'écrire ou
d'adapter un adaptateur, et peut modifier des performances, des capacités
disponibles ou des contraintes.

| Changement bas niveau | Effet visé |
|-----------------------|-----------|
| Nouveau contrôleur d'écran | Nouvel adaptateur/driver ; apps inchangées si le contrat d'affichage est respecté |
| Nouveau MCU / RTOS | Nouvel adaptateur cible ; compatibilité applicative visée dans les limites du contrat du SDK |
| Nouveau runtime (Lua → +WASM) | Derrière l'abstraction de runtime ([ADR-0002](../adr/0002-modele-execution-applications.md)) ; à valider |
| Nouvelle jauge de batterie | Nouvel adaptateur/driver ; API `power` inchangée |

Bénéfice concret et immédiat : **on peut tester services et SDK sur PC** via des
adaptateurs host (mocks) implémentant les mêmes ports — CI rapide, sans matériel
(voir [`tests/`](../../tests/README.md)).

## Cartographie avec l'arborescence `firmware/`

| Dossier | Rôle dans le modèle |
|---------|---------------------|
| `firmware/hal/` | **Ports de plateforme** : interfaces abstraites, portables, **sans ESP-IDF**. |
| `firmware/drivers/` | **Adaptateurs cible + drivers concrets** implémentant les ports (ESP32-S3 via ESP-IDF, composants). |
| `firmware/kernel/` | Mécanismes centraux + **point de composition** au démarrage. |
| `firmware/services/` | Services, dépendant des ports. |
| `firmware/companion-sdk/` | Façade/contrat applicatif exposé par les services. |
| adaptateurs **host** | Sous `tests/` : implémentations mock des ports pour les tests PC. |

## Comment on le fait respecter

- Découpage physique en composants (`hal` = ports ; `drivers` = adaptateurs ;
  `services`, `companion-sdk`, `apps`).
- Revue de PR : une abstraction (port, service, SDK) qui inclut un en-tête
  ESP-IDF/FreeRTOS ou un driver concret est refusée.
- À terme, vérification automatisée en CI (règles d'inclusion).

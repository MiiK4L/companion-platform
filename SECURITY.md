<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Politique de sécurité

Companion Platform est un appareil connecté (Wi-Fi/BLE via Seeed XIAO ESP32-S3),
conçu pour durer et pour accueillir des apps et des modules **tiers**. La
sécurité est prise au sérieux, mais ce document décrit un **état de projet en
Phase 0** : plusieurs mécanismes sont **prévus**, non implémentés, et le
**modèle de menace est encore en construction**.

## Périmètre

Sont concernés : le **firmware** (kernel, adaptateurs/drivers, services, Companion
SDK), le **processus de mise à jour** (OTA prévu), le standard **CX-Bus**
(identification et communication des modules), et les **outils** officiels
(`tools/`) manipulant des artefacts de build/flash/provisioning.

> **Secure Boot, Flash Encryption et OTA signé ne constituent pas, à eux seuls,
> la sécurité des applications.** Ce sont des cibles de **production** (ESP-IDF),
> non requises en prototypage. La confiance envers les **modules physiques**, les
> **Manifests** et les **paquets Lua** relève du modèle de menace ci-dessous, qui
> est une problématique distincte.

## Signaler une vulnérabilité

**N'ouvrez pas d'Issue publique** pour une vulnérabilité.

- **État actuel : le dépôt est privé.** Le *Private Vulnerability Reporting* de
  GitHub n'est **pas disponible** dans la configuration actuelle (offre/visibilité) —
  vérifié le 2026-07-21 (l'API renvoie 404). Aucun canal public n'est donc encore
  publié, et **aucune adresse de contact n'est inventée ici**.
- **Avant le passage en public**, un **canal privé valide sera établi et publié**
  dans ce fichier : activation du *Private Vulnerability Reporting* sur le dépôt
  public (onglet **Security → Report a vulnerability**) et/ou un contact de
  sécurité dédié. Cette section sera mise à jour à ce moment-là.
- Tant que le dépôt reste privé, seul le mainteneur y a accès ; un signalement se
  fait directement auprès de lui par un moyen privé convenu.

Merci de ne pas tester une vulnérabilité sur du matériel ou une infrastructure
de tiers sans autorisation.

## Versions supportées

Aucune release publique n'existe encore. Un **tableau des versions supportées**
sera ajouté ici dès la première release (par domaine : `fw-`, `spec-`, etc.).

## Modèle de menace (à développer)

Un modèle de menace **dédié** sera élaboré (Phase 1+). Il devra couvrir au moins :

| Menace | Enjeu |
|--------|-------|
| **Modules physiques non fiables** | Un module tiers peut être défectueux ou malveillant (électrique et logique — voir SPEC §7 et §8). |
| **Manifests falsifiés** | Le CRC ne garantit **pas** l'authenticité ; un Manifest peut être forgé (voir `standards/cx-bus/cx-bus-manifest.md`). |
| **Paquets Lua non fiables** | Provenance, intégrité, permissions et exécution de code applicatif. |
| **Permissions des apps** | Ce à quoi une app (native ou Lua) a droit via le SDK. |
| **Clés de signature** | Gestion, stockage et rotation d'éventuelles clés. |
| **Downgrade** | Empêcher le retour forcé à une version vulnérable. |
| **Récupération après mise à jour échouée** | Robustesse de l'OTA (rollback sûr). |

> ⚠️ **Le runtime Lua n'est PAS présumé « sandboxé ».** Lua intégré n'est pas
> automatiquement une sandbox sûre : l'isolation, les permissions et les limites
> de ressources sont une **question de sécurité ouverte**, à concevoir et à
> **prouver** — pas une propriété acquise.

## Divulgation

Les vulnérabilités seront traitées de façon responsable, avec divulgation
coordonnée après correctif. Les délais et un processus formel seront précisés
lorsque le canal de signalement public sera en place.

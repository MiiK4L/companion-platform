<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: Apache-2.0
-->

# Services — Services métier de la plateforme

> Statut : **Phase 0 — Fondations**. Les API et comportements détaillés sont marqués
> « ⏳ À définir ».

Les **services** implémentent les fonctions transverses de la plateforme au-dessus du kernel.
Ils orchestrent le matériel et l'état pour offrir des capacités de haut niveau, que le
**Companion SDK** expose ensuite aux apps.

Références de décision : ADR-0002, ADR-0003, ADR-0005, ADR-0006.

## Services

| Service | Rôle |
|---------|------|
| **Module Manager** | Détection, identification (lecture du **CX-Bus Manifest**), et intégration des modules CX-Bus. Gère l'insertion/retrait à chaud et le power-gating logique. |
| **App Manager** | Cycle de vie des apps (installation, lancement, arrêt), exécution d'un **runtime de script** (**Lua** candidat en V1) derrière une abstraction de runtime (ADR-0002). L'installation dynamique sans reflash reste à valider (prototype). |
| **UI** | Rendu et interface via un **moteur graphique** (**LVGL** candidat) — widgets, sprites, power-gating écran (ADR-0003). |
| **Power** | Politiques d'alimentation, autonomie, coordination avec la gestion d'énergie du kernel. |
| **Storage** | Persistance de haut niveau (données d'apps, état, préférences). |
| **Connectivity** | Wi-Fi / Bluetooth (activation à la demande, gestion de la consommation). |
| **Companion** | Le personnage comme **service de présentation d'état transverse** : expose l'humeur/état global du système de façon incarnée, indépendamment des apps. |

### Le Companion comme service

Le **Companion** (le personnage) n'est pas une app : c'est un **service de présentation
transverse** qui reflète l'état du système et des apps sous une forme incarnée. Il est distinct
de l'app Tamagotchi, qui, elle, est une app scriptée (Lua candidat) consommant le SDK (voir
[`apps/tamagotchi/`](../../apps/tamagotchi/README.md)).

## Règles de dépendance

- Les services dépendent des **ports abstraits** ([`hal/`](../hal/README.md)) et des mécanismes
  du **kernel** — jamais d'ESP-IDF/FreeRTOS ni d'un driver concret directement (ADR-0007). Les
  implémentations leur sont fournies par le **point de composition** au démarrage.
- Les services **NE DOIVENT PAS** dépendre du SDK ni des apps : le **Companion SDK** est la
  façade qu'ils **fournissent**, seule surface visible côté application.
- Une app ne parle jamais à un service en direct (ex. Module Manager) : elle passe par le SDK.

> ⏳ **À définir — Phases ultérieures** : API interne de chaque service, protocole du Module
> Manager, intégration du runtime de script (Lua candidat) dans l'App Manager, modèle d'état
> du Companion.

## Licence

Firmware sous **Apache-2.0**. Documentation sous **CC-BY-4.0**.

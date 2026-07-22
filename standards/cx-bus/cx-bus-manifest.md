<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# CX-Bus Manifest — Descripteur d'identification des modules

> **Statut : BROUILLON / DRAFT** — Phase 0 (Fondations).
> La structure *logique* des champs est esquissée ici. Le **format binaire exact** (offsets,
> tailles, endianness, encodage) et le **support de stockage** sont « ⏳ À définir — Phase 1 ».

Références de décision : ADR-0005, ADR-0006. Voir aussi
[`SPEC-CX-Bus-1.0.md`](./SPEC-CX-Bus-1.0.md), §8 (dont les contraintes de **lecture sûre** et le
fait que l'EEPROM I²C soit un mécanisme **candidat non figé**).

## Rôle

Le **CX-Bus Manifest** est le descripteur d'identification d'un **CX-Bus Module**. Il permet au
**Host** de **reconnaître** un module, de **vérifier sa compatibilité** et de **découvrir ses
capacités**. Il ne **provoque jamais**, à lui seul, l'installation ou l'exécution de code (voir
« Modèle de confiance » plus bas).

## Antériorité : spec EEPROM des HAT Raspberry Pi

Le modèle s'inspire de la **spécification EEPROM des HAT Raspberry Pi** (auto-description
d'extensions : identification vendeur/produit, description matérielle). Attention à la
terminologie : cette antériorité repose sur un **en-tête avec signature *magique* et somme de
contrôle**, ce qui **n'est pas** une authentification cryptographique. Le CX-Bus adopte le
principe d'auto-description, pas une garantie d'authenticité (voir ci-dessous).

## Intégrité vs authenticité — ne pas confondre

| Élément | Ce qu'il garantit | Ce qu'il ne garantit PAS |
|---------|-------------------|--------------------------|
| `magic` / en-tête | Qu'on lit *probablement* un Manifest CX-Bus | Rien sur l'origine |
| `crc` | Détection d'une **corruption accidentelle** | **Aucune** authenticité ni intégrité contre un acteur malveillant |
| Authentification cryptographique (signature) | Origine/authenticité | — |

Le CRC détecte un support abîmé ; il **ne protège pas** contre un Manifest **falsifié**. Une
éventuelle **authentification cryptographique** est une **décision de sécurité distincte** (voir
`SECURITY.md`, modèle de menace), non actée à ce stade.

## Structure logique des champs (esquisse)

| Champ | Rôle |
|-------|------|
| `magic` / signature | Marqueur d'en-tête (intégrité de format, **pas** authenticité). |
| `manifest_version` | Version du *format* de Manifest. |
| `cx_bus_protocol_required` | Version du standard CX-Bus exigée par le module (compatibilité, SPEC §10). |
| `vendor_id` | Identifiant du **fabricant** (voir « Espace de nommage » ci-dessous). |
| `product_id` | Identifiant du **produit** chez ce fabricant. |
| `hardware_revision` | Révision matérielle (révision de PCB). |
| `model_name` | Nom lisible du **modèle**. |
| `serial` *(optionnel)* | Identifiant d'**exemplaire** (numéro de série), distinct du modèle. |
| `capabilities` | Capacités déclarées (ex. capteur, actionneur, stockage). |
| `resources_required` | **Ressources/signaux** utilisés (I²C, IRQ…) et, à terme, **besoins électriques** (courant nominal/crête, modes de puissance) — ce que le Host peut **vérifier**, pas un « routage ». |
| `compatible_apps` | Liste de **recommandations** d'apps compatibles (voir modèle de confiance) — **pas** des apps fournies ni autorisées. |
| `crc` | Somme de contrôle d'**intégrité accidentelle** (voir ci-dessus). |

### Espace de nommage des identifiants (`vendor_id`)

Un **registre central** de `vendor_id` entrerait en tension avec la promesse « un tiers peut
créer un module **sans autorisation centrale** ». À évaluer en Phase 1 : privilégier une
convention **sans autorité administrée** — par exemple **reverse-DNS** (`com.exemple.module`) ou
un **UUID de fabricant** auto-généré — plutôt qu'un registre attribué.

### `resources_required` (ex-`pinmap_used`)

Le Host **ne peut pas « router »** des broches physiquement fixes. Ce champ décrit donc les
**ressources et signaux requis** par le module ; le Host s'en sert pour **vérifier** ce qu'il
peut réellement fournir (adresses I²C libres, disponibilité d'une IRQ, budget de courant une
fois le modèle d'alimentation défini — SPEC §6), et **refuser proprement** sinon.

## Modèle de confiance (apps & capacités)

Un Manifest provenant d'un **module tiers non fiable** ne doit **jamais** déclencher
automatiquement l'installation ou l'exécution de code. Le flux est :

1. **Module Manager** : lit le Manifest, vérifie l'intégrité de format, **publie des capacités**.
2. **App Manager** : décide **ensuite** si une app **déjà installée** ou issue d'une **source
   approuvée** peut être **proposée** ou **lancée** — jamais sur la seule foi du Manifest.

`compatible_apps` est donc une **association/recommandation**, résolue contre un catalogue de
confiance, et non une autorisation.

### Distribution des paquets Lua — à spécifier séparément (Phase 1+)

Les points suivants relèvent d'un **document de distribution logicielle distinct** (pas du
Manifest matériel), et restent **⏳ à définir** : où **résident** les paquets Lua ; comment ils
sont **distribués** ; leur **manifeste logiciel** et leur **version** ; leur **signature /
intégrité** ; la **source de confiance** ; le **consentement utilisateur** ; le fonctionnement
**hors ligne** ; les **permissions** ; la **révocation** ; la **compatibilité SDK**.

## Règles de lecture par le Host (principe)

1. Lire l'en-tête, vérifier `magic`.
2. Vérifier le `crc` (intégrité **accidentelle** seulement) ; rejeter si invalide.
3. Vérifier `cx_bus_protocol_required` vs version supportée.
4. Vérifier `resources_required` vs ressources disponibles ; refuser proprement sinon.
5. Publier identité et `capabilities` au **Module Manager**.
6. **Ne rien installer/lancer automatiquement** : `compatible_apps` est transmis à l'**App
   Manager** comme recommandation, soumise à la politique de confiance.

## Exemple ILLUSTRATIF (non normatif)

> ⚠️ **Non normatif.** Noms, tailles et valeurs n'illustrent que l'*intention* ; ils ne
> constituent PAS le format binaire (« ⏳ À définir — Phase 1 »).

```text
CX-Bus Manifest (pseudo-structure illustrative)
────────────────────────────────────────────────
magic                    = "CXBM"                 # intégrité de format, pas authenticité
manifest_version         = 1
cx_bus_protocol_required = 1.0
vendor_id                = "com.acme"             # convention reverse-DNS (sans registre central)
product_id               = "env-sensor"
hardware_revision        = "A"
model_name               = "Capteur environnemental"
serial                   = 0x... (optionnel)      # identifiant d'exemplaire
capabilities             = [ SENSOR_TEMP, SENSOR_HUMIDITY, I2C_PERIPHERAL ]
resources_required       = { i2c_addresses: [0x44], irq: yes, current_mA: <à définir> }
compatible_apps          = [ "com.acme.weather" ] # RECOMMANDATION, non autorisation
crc                      = 0x????                 # intégrité accidentelle
```

> ⏳ **À définir — Phase 1** : support de stockage et lecture sûre (SPEC §8), endianness,
> encodage, longueurs, format de `capabilities`/`resources_required`, algorithme de CRC,
> besoins électriques normatifs, éventuelle authentification cryptographique (décision sécurité).

---

*Document publié sous CC-BY-4.0. Voir le [README du standard](./README.md).*

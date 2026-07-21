# CX-Bus Manifest — Descripteur d'identification des modules

> **Statut : BROUILLON / DRAFT** — Phase 0 (Fondations).
> La structure *logique* des champs est fixée ici. Le **format binaire exact** (offsets,
> tailles, endianness, encodage) est marqué « ⏳ À définir — Phase 1 ».

Références de décision : ADR-0005, ADR-0006. Voir aussi la spécification
[`SPEC-CX-Bus-1.0.md`](./SPEC-CX-Bus-1.0.md), §8.

## Rôle

Le **CX-Bus Manifest** est le descripteur d'identification qu'embarque chaque **CX-Bus
Module**. Il permet au **Host** de reconnaître automatiquement un module inséré, de vérifier
sa compatibilité, de connaître ses capacités et de découvrir les applications qu'il expose —
sans intervention de l'utilisateur.

## Antériorité : spec EEPROM des HAT Raspberry Pi

Le modèle retenu s'inspire directement de la **spécification EEPROM des HAT (Hardware
Attached on Top) Raspberry Pi**, qui a fait ses preuves comme mécanisme d'auto-description
d'extensions matérielles : une EEPROM I²C portant un en-tête signé, des champs
d'identification vendeur/produit et une description du matériel. Le CX-Bus adapte ce principe
à ses propres besoins (bus partagés, apps exposées, capacités).

## Support de stockage

Le Manifest DOIT être stocké dans une **EEPROM I²C** présente sur le Module. Le Host lit
cette EEPROM lors de la séquence d'identification (voir SPEC §7, étape 5). L'adresse I²C de
l'EEPROM et la stratégie anti-collision relèvent du plan d'adressage (SPEC §9).

> ⏳ **À définir — Phase 1** : taille minimale de l'EEPROM, adresse I²C réservée du Manifest,
> comportement si l'EEPROM est absente ou illisible.

## Structure logique des champs

Les champs ci-dessous décrivent le **contenu logique** du Manifest. Ordre, tailles et
encodage binaire sont à figer en Phase 1.

| Champ | Rôle |
|-------|------|
| `magic` / signature | Marqueur d'en-tête identifiant un Manifest CX-Bus valide. |
| `manifest_version` | Version du *format* de Manifest (permet l'évolution du descripteur lui-même). |
| `cx_bus_protocol_required` | Version du standard CX-Bus exigée par le module (support des règles de compatibilité, SPEC §10). |
| `vendor_id` | Identifiant du fabricant/auteur du module. |
| `product_id` | Identifiant du produit chez ce vendeur. |
| `hw_revision` | Version matérielle du module (révision de PCB). |
| `name` | Nom lisible du module. |
| `author` | Auteur/organisation. |
| `uuid` | Identifiant unique universel de l'exemplaire ou du modèle. |
| `capabilities` | Liste des capacités/fonctionnalités déclarées (ex. capteur, actionneur, stockage). |
| `pinmap_used` | Brochage/broches réellement utilisés ou exigés par le module (cohérence avec SPEC §5). |
| `apps_exposed` | Liste des identifiants d'apps exposées par le module (consommées par l'App Manager). |
| `crc` | Somme de contrôle d'intégrité du Manifest. |

### Notes de conception

- **`cx_bus_protocol_required`** est central pour la compatibilité ascendante : le Host
  compare cette valeur à la version qu'il implémente et refuse proprement un module trop
  récent (SPEC §10).
- **`capabilities`** et **`pinmap_used`** permettent au Host de vérifier qu'il peut alimenter
  et router le module dans son budget GPIO/courant (voir `hardware/mainboard-v1/`).
- **`apps_exposed`** relie le matériel au logiciel : l'**App Manager** (voir
  `firmware/services/`) utilise ces identifiants pour proposer/charger les apps du module.
- **`crc`** protège contre une EEPROM corrompue ; un Manifest invalide DOIT être rejeté.

## Exemple ILLUSTRATIF (non normatif)

> ⚠️ **Exemple non normatif.** Les noms, tailles et valeurs ci-dessous ne servent qu'à
> illustrer l'*intention*. Ils NE constituent PAS le format binaire du Manifest, qui est
> « ⏳ À définir — Phase 1 ».

```text
CX-Bus Manifest (pseudo-structure illustrative)
────────────────────────────────────────────────
magic                    = "CXBM"            # signature d'en-tête
manifest_version         = 1
cx_bus_protocol_required = 1.0
vendor_id                = 0x0042            # attribué par le registre CX-Bus (à définir)
product_id               = 0x0007
hw_revision              = "A"
name                     = "Capteur environnemental"
author                   = "ACME Modules"
uuid                     = 550e8400-e29b-41d4-a716-446655440000
capabilities             = [ SENSOR_TEMP, SENSOR_HUMIDITY, I2C_PERIPHERAL ]
pinmap_used              = { I2C: [SDA, SCL], IRQ: yes, MOD_EN: yes }
apps_exposed             = [ "acme.weather", "acme.airquality" ]
crc                      = 0x????            # calculé sur l'ensemble
```

## Règles de lecture par le Host (principe)

1. Lire l'en-tête et vérifier `magic`.
2. Vérifier le `crc` ; rejeter si invalide.
3. Vérifier `cx_bus_protocol_required` vs version supportée (compatibilité).
4. Exposer l'identité et les `capabilities` aux services (Module Manager).
5. Transmettre `apps_exposed` à l'App Manager.

> ⏳ **À définir — Phase 1** : endianness, encodage des chaînes, longueurs maximales, format
> exact de `capabilities`/`apps_exposed`, algorithme de CRC, gestion des versions de
> `manifest_version`.

---

*Document publié sous CC-BY-4.0. Voir le [README du standard](./README.md).*

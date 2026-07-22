# Gabarit de module CX-Bus

> Statut : **Phase 0 — Fondations**. Gabarit destiné aux auteurs de modules CX-Bus tiers.

Ce dossier est un **gabarit (template)** pour concevoir un **CX-Bus Module** conforme au
standard. Copiez-le, renommez-le, et remplissez chaque section pour votre module.

Références : [`standards/cx-bus/`](../../standards/cx-bus/README.md), ADR-0005, ADR-0006.

## Structure attendue

Un module CX-Bus DEVRAIT être organisé ainsi :

```
mon-module/
├── README.md            # description du module (ce gabarit)
├── doc/                 # documentation : fonction, brochage utilisé, notes de conception
├── kicad/               # conception matérielle (schéma, PCB)
└── firmware/            # firmware du module et/ou apps exposées
```

## Conformité au standard CX-Bus

Pour être déclaré **« CX-Bus compatible »**, le module DOIT respecter les critères de
conformité de la spécification (voir `SPEC-CX-Bus-1.0.md`, §11) :

1. Format mécanique et connecteur conformes (§3, §4).
2. Brochage et rails respectés, budget de courant non dépassé (§5, §6).
3. **CX-Bus Manifest** valide et lisible via le **support d'identification** normatif (§8 ;
   mécanisme à figer en Phase 1, EEPROM I²C = option privilégiée à évaluer).
4. Rôles et arbitrage des bus partagés respectés (§9) — ne jamais bloquer le bus.
5. Comportement sûr à l'insertion/retrait à chaud (§7).
6. Version de protocole CX-Bus requise correctement déclarée.

> ⏳ Rappel : de nombreuses valeurs (connecteur, brochage, budget de courant) sont figées en
> Phase 1. Suivez la spécification à jour.

## CX-Bus Manifest à fournir

Votre module DOIT fournir un **CX-Bus Manifest** via le **support d'identification** normatif
du standard (mécanisme à définir en Phase 1 ; EEPROM I²C candidate — voir
`standards/cx-bus/cx-bus-manifest.md`). Renseignez au minimum : signature, version de Manifest,
version de protocole requise, `vendor_id`, `product_id`, version matérielle, nom, auteur, UUID,
capacités, brochage utilisé, apps exposées et CRC.

## Apps exposées

Listez ici les apps que votre module expose à la plateforme (identifiants déclarés dans le
champ `apps_exposed` du Manifest). L'**App Manager** les découvre et les propose via ces
identifiants.

- `vendor.mon-app` — description courte…

## Licence

**Vous choisissez la licence de votre propre module.** Le cœur logiciel de la plateforme reste
**Apache-2.0** et le matériel officiel **CERN-OHL-S-2.0**, mais un auteur tiers est libre de
publier son module (matériel, firmware, docs) sous la licence de son choix. Indiquez-la
clairement ici.

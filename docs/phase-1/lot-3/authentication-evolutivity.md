<!--
SPDX-FileCopyrightText: 2026 Companion Platform contributors

SPDX-License-Identifier: CC-BY-4.0
-->

# Lot 3 — Évolutivité vers l'authentification (hors V1, *aucune décision*)

> **Statut : exigence d'évolutivité — PAS une décision.** **Aucune DEC ouverte**,
> **aucune ADR**, **aucune obligation V1**. Ce document garantit qu'un chemin vers
> l'**authentification cryptographique** reste **possible** sans l'**imposer**.
> Valeurs `[H]` ; **aucune `[M]`**.

## 1. Pourquoi séparer, et ne rien décider maintenant

**Authentification** (« puis-je être considéré digne de confiance ? ») est
distincte de l'**identification** et de la **description**. La traiter en V1
imposerait une complexité (pile crypto, provisioning de clés, gestion de
certificats/révocation) **non nécessaire** pour faire fonctionner un module.
Conformément au cadrage, **aucune DEC n'est ouverte** : une décision ne sera prise
que si un besoin réel émerge (signature du Manifest, Secure Element, certificats,
challenge-réponse).

> **CRC ≠ authenticité.** Le CRC détecte la **corruption**, pas la
> **contrefaçon**. Ne jamais présenter un CRC comme une preuve d'authenticité.

## 2. Réservations qui gardent le chemin ouvert (sans coût V1 significatif)

| Réservation | Effet | Coût V1 |
|-------------|-------|---------|
| **Champ de version** du Manifest | permet d'introduire un profil « signé » plus tard | nul |
| **TLV extensible** (types inconnus ignorables) | ajout de TLV « signature / certificat » sans casser les lecteurs V1 | nul |
| **TLV réservé « évolutivité »** | emplacement documenté pour futures données d'auth | nul |
| **Périmètre CRC explicite** | distinguer plus tard « intègre » de « authentifié » | nul |
| Ne pas exclure un **rail/adressage** pour un futur élément actif | un SE/µC pourra être ajouté | nul |

## 3. Options futures possibles (documentées, non retenues)

- **Signature du Manifest** (clé publique fabricant vérifiée par le Host).
- **Secure Element** côté module (ex. famille CryptoAuthentication) — challenge-réponse.
- **Certificats** + chaîne de confiance (révocation, péremption) — lourd.
- **Challenge-réponse symétrique** — provisioning de secrets partagés.

Chaque option a des **implications** (provisioning, coût, complexité Host,
gestion de clés) qui seront **arbitrées le moment venu**, dans une **DEC dédiée**.

## 4. Lien avec le modèle de confiance (déjà acté)

Le modèle reste : le **Module Manager publie les capacités**, l'**App Manager
décide** ; **aucune installation automatique** sur la seule foi du Manifest (cf.
architecture / ADR CX-Bus). L'authentification **renforcerait** ce modèle sans le
remplacer.

## Ne fige rien

Aucune brique d'authentification n'est requise en V1 ; aucune n'est interdite pour
l'avenir. **Aucune décision, aucune ADR.**

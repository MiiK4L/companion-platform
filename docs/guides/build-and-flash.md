# Guide — Build et flash

Ce guide décrit comment compiler le firmware Companion Platform avec
**ESP-IDF** et le flasher sur un appareil basé sur le **Seeed XIAO
ESP32-S3**.

## 1. Prérequis

- À compléter — Phase 1 : version d'ESP-IDF supportée, dépendances
  système (Python, outils de build), plateformes hôtes testées
  (Linux/macOS/Windows).

## 2. Cloner le dépôt

- À compléter — Phase 1 : commande de clonage, initialisation des
  sous-modules éventuels, structure générale du monorepo à connaître
  avant de builder (`firmware/`, `apps/`, `hardware/`...).

## 3. Configurer l'environnement ESP-IDF

- À compléter — Phase 1 : installation/activation d'ESP-IDF
  (`install.sh`/`export.sh` ou équivalent), variables d'environnement
  attendues.

## 4. Configurer le projet (sdkconfig)

- À compléter — Phase 1 : cible (`idf.py set-target esp32s3`), options de
  configuration pertinentes (`idf.py menuconfig`), profils de
  configuration prévus (dev vs production, Secure Boot/Flash Encryption
  — voir `SECURITY.md`).

## 5. Builder

- À compléter — Phase 1 : commande de build (`idf.py build`), où
  trouver les artefacts produits, temps de build indicatif.

## 6. Flasher

- À compléter — Phase 1 : commande de flash (`idf.py flash`),
  identification du port série, cas particulier du premier flash sur
  carte vierge.

## 7. Monitorer

- À compléter — Phase 1 : commande de monitoring série (`idf.py
  monitor`), comment interpréter les logs de démarrage (HAL, drivers,
  Companion SDK, chargement du runtime Lua).

## 8. Problèmes courants

- À compléter — Phase 2 : FAQ de dépannage (port non détecté, échec de
  flash, boot loop) au fur et à mesure des retours de la communauté.

## Voir aussi

- [`docs/guides/contributing-code.md`](contributing-code.md)
- `firmware/` pour l'organisation du code source
- `hardware/README.md` pour les révisions matérielles supportées

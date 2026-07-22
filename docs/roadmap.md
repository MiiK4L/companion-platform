# Feuille de route

La roadmap suit une **progression ordonnée** (les étapes ne sont pas sautées à la
légère) :

> Vision → Architecture → Documentation → Validation → Implémentation → Tests → Optimisation

Le processus reste **itératif** : une mesure ou un prototype peut justifier un
**retour à une étape antérieure** (réviser une décision, refaire un budget). Chaque
phase se termine par une **porte de validation** (revue + décisions tracées en
ADR). On privilégie une fondation solide à un prototype rapide.

## État d'avancement

| Phase | Objectif | État |
|-------|----------|------|
| **0** | Fondations : architecture, dépôt, ADR, CI docs | ✅ En cours de validation |
| **1** | Spécification CX-Bus 1.0 + choix des composants + budgets | ⏭️ Suivante |
| 2 | Schémas électroniques | ⏳ |
| 3 | PCB v1 | ⏳ |
| 4 | Boîtier (co-conçu avec le PCB) | ⏳ |
| 5 | HAL + squelette d'OS + tests host | ⏳ |
| 6 | Firmware minimal (boot, écran, entrées, énergie) | ⏳ |
| 7 | Mini système d'exploitation (noyau, services) | ⏳ |
| 8 | Module Manager (détection/identification CX-Bus) | ⏳ |
| 9 | Première application : Tamagotchi (app scriptée, Lua candidat) | ⏳ |
| 10 | Premier module externe | ⏳ |
| 11 | Applications réseau | ⏳ |
| 12 | Applications homelab | ⏳ |
| 13 | Applications électroniques | ⏳ |
| 14 | Optimisation (énergie, mémoire) | ⏳ |
| 15 | Documentation complète & release | ⏳ |

## Détail des phases

### Phase 0 — Fondations *(en cours de validation)*
Architecture globale, arborescence du dépôt, 13 ADR, gouvernance, licences,
**CI documentaire (build MkDocs strict, sans publication automatique)**,
gestion de projet (**Milestones, Labels**). **Porte :** validation de
l'architecture par le mainteneur, puis push initial propre.

### Phase 1 — Spécification CX-Bus & composants
Faire **converger** (sans figer prématurément) la [spec CX-Bus](../standards/cx-bus/SPEC-CX-Bus-1.0.md)
et sélectionner les **composants de production** (écran, RTC, accéléromètre,
alimentation…), avec **sourcing en temps réel** (prix, stock, cycle de vie,
distributeurs, **≥ 2 alternatives réellement compatibles**). Établir le **budget
GPIO** et un **premier budget énergétique** (mesuré, non décrété). Un **lot dédié
à la sûreté électrique et au threat model CX-Bus** est ajouté.

**Prototypes exploratoires et petits PCB de validation sont autorisés en Phase 1**
(les schémas de production restent en Phase 2). Ces prototypes **évaluent des
candidats** (ils ne les actent pas) : P1 bring-up GPIO/bus, **P2 écran+LVGL
candidats** (RAM/conso), P3 deep sleep & réveils, **P4 base de temps / RTC
candidat**, **P5 jauge candidate**, P6 hot-plug, **P7 support d'identification
(EEPROM candidate)**, **P8 runtime Lua candidat & installation dynamique**, P9
endurance connecteur — **complétés** par : **bus isolé / module non alimenté**,
**court-circuit & surintensité (alim de labo)**, **module bloquant SDA/SCL**,
**retrait pendant une transaction**, **partage SPI avec l'écran**. Chaque
candidat validé fera l'objet d'une ADR de décision.

> ⚠️ On **distingue les composants de prototype des composants de production**. Un
> changement de composant n'est **pas** un « swap indolore » : le modèle
> ports/adaptateurs réduit l'impact *logiciel*, mais pas les impacts électriques,
> mécaniques ou de performance.

**Porte de sortie :** le tag **`spec-v1.0.0`** n'est promis qu'**après** critères
de sortie satisfaits (schéma du chemin d'alimentation, prototype de connecteur,
mesures d'inrush, tests d'insertion/retrait, test de bus bloqué, endurance
mécanique minimale). D'ici là : versions **0.x / 1.0.0-rc**. Les décisions de
composants sont tracées par ADR (regroupées quand c'est cohérent).

### Phases 2–4 — Électronique & mécanique
Schémas (ERC), PCB v1 (DRC, Gerbers, BOM), boîtier imprimable 3D co-conçu.
CI matérielle (KiCad ERC/DRC, export automatique). **Portes :** revue de
schémas, revue de PCB, revue d'intégration mécanique.

### Phases 5–8 — Fondations logicielles
HAL + implémentation hôte (mock) + tests host **avant** tout driver ; firmware
minimal ; noyau et services ; Module Manager. **Portes :** couverture de tests
host, démonstration d'un boot complet, démonstration détection module.

### Phases 9–13 — Applications
Tamagotchi (app scriptée de référence, Lua candidat), premier module externe, puis familles d'apps
(réseau, homelab, électronique). Chaque app livre sa **documentation** dans la
même PR.

### Phases 14–15 — Optimisation & documentation finale
Optimisation énergie/mémoire mesurée contre les budgets ; documentation
complète, guides finalisés, première release publique.

## Versions
Versioning SemVer **par domaine**, syntaxe uniforme **`<domaine>-vX.Y.Z`** (tags
`hw-`, `fw-`, `sdk-`, `spec-`, `mod-`, `app-`), avec matrice de compatibilité.
Le firmware est **un seul artefact** (`fw-`) : pas de tag `os-` séparé. Voir
[ADR-0012](adr/0012-monorepo-et-versioning.md).

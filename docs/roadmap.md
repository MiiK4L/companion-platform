# Feuille de route

La roadmap suit une logique stricte, **sans jamais sauter d'étape** :

> Vision → Architecture → Documentation → Validation → Implémentation → Tests → Optimisation

Chaque phase se termine par une **porte de validation** (revue + décisions
tracées en ADR) avant de passer à la suivante. On privilégie toujours une
fondation solide à un prototype rapide.

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
| 9 | Première application : Tamagotchi (Lua) | ⏳ |
| 10 | Premier module externe | ⏳ |
| 11 | Applications réseau | ⏳ |
| 12 | Applications homelab | ⏳ |
| 13 | Applications électroniques | ⏳ |
| 14 | Optimisation (énergie, mémoire) | ⏳ |
| 15 | Documentation complète & release | ⏳ |

## Détail des phases

### Phase 0 — Fondations *(en cours de validation)*
Architecture globale, arborescence du dépôt, 13 ADR, gouvernance, licences,
CI documentaire (MkDocs + Pages), gestion de projet (Projects, Milestones,
Labels). **Porte :** validation de l'architecture par le mainteneur, puis push
initial propre.

### Phase 1 — Spécification CX-Bus & composants
Figer la [spec CX-Bus 1.0](https://github.com/MiiK4L/companion-platform/blob/main/standards/cx-bus/SPEC-CX-Bus-1.0.md) (connecteur,
brochage, alimentation, Manifest binaire), sélectionner les composants (écran,
RTC, accéléromètre, jauge, expander, load switch), établir le **budget GPIO** et
le **budget énergétique**. **Porte :** ADR de sélection des composants + spec
CX-Bus 1.0 gelée.

### Phases 2–4 — Électronique & mécanique
Schémas (ERC), PCB v1 (DRC, Gerbers, BOM), boîtier imprimable 3D co-conçu.
CI matérielle (KiCad ERC/DRC, export automatique). **Portes :** revue de
schémas, revue de PCB, revue d'intégration mécanique.

### Phases 5–8 — Fondations logicielles
HAL + implémentation hôte (mock) + tests host **avant** tout driver ; firmware
minimal ; noyau et services ; Module Manager. **Portes :** couverture de tests
host, démonstration d'un boot complet, démonstration détection module.

### Phases 9–13 — Applications
Tamagotchi (app Lua de référence), premier module externe, puis familles d'apps
(réseau, homelab, électronique). Chaque app livre sa **documentation** dans la
même PR.

### Phases 14–15 — Optimisation & documentation finale
Optimisation énergie/mémoire mesurée contre les budgets ; documentation
complète, guides finalisés, première release publique.

## Versions
Versioning SemVer **par domaine** (tags `hw-`, `fw-`, `os-`, `spec-`, `mod-`,
`app-`) avec matrice de compatibilité. Voir
[ADR-0012](adr/0012-monorepo-et-versioning.md).

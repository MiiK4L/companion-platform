# Registre des décisions d'architecture (ADR)

Ce dossier contient les **Architecture Decision Records** du projet Companion
Platform. Chaque décision structurante y est tracée de façon **immuable** :
problème, options étudiées, avantages/inconvénients, choix retenu et
conséquences. C'est la mémoire technique du projet — n'importe quelle IA ou
ingénieur doit pouvoir comprendre *pourquoi* une chose est ainsi en lisant ce
dossier.

## Principe

- Une décision importante = une ADR.
- Une ADR acceptée est **immuable**. Pour changer d'avis, on crée une nouvelle
  ADR qui **remplace** l'ancienne (statut « Remplacé par ADR-XXXX »).
- Format : [template.md](template.md) (MADR adapté).
- Voir [ADR-0000](0000-adopter-les-adr.md) pour le processus complet.

## Index

| ADR | Titre | Statut | Phase |
|-----|-------|--------|-------|
| [0000](0000-adopter-les-adr.md) | Adopter les ADR pour tracer les décisions | Accepté | 0 |
| [0001](0001-framework-firmware-esp-idf.md) | Framework firmware : ESP-IDF comme fondation | Accepté | 0 |
| [0002](0002-modele-execution-applications.md) | Modèle d'exécution des applications : hybride natif + Lua | Accepté (orientation) | 0 |
| [0003](0003-technologie-et-abstraction-ecran.md) | Écran TFT IPS couleur abstrait par un moteur graphique | Accepté (orientation) | 0 |
| [0004](0004-coeur-de-calcul-socket.md) | Cœur de calcul XIAO ESP32-S3 socketé et remplaçable | Accepté (orientation) | 0 |
| [0005](0005-standard-cx-bus-et-identification.md) | Standard d'extension CX-Bus et identification par EEPROM | Accepté (principes) | 0 |
| [0006](0006-nommage-cx-bus.md) | Nommage du standard : CX-Bus (abandon de « MCP ») | Accepté | 0 |
| [0007](0007-hal-et-companion-sdk.md) | Inversion des dépendances : HAL + Companion SDK | Accepté | 0 |
| [0008](0008-architecture-alimentation.md) | Architecture d'alimentation : jauge + power-gating | Accepté (principes) | 0 |
| [0009](0009-rtc-externe-et-persistance-etat.md) | RTC externe et persistance d'état hors tension | Accepté (principes) | 0 |
| [0010](0010-strategie-licences.md) | Stratégie de licences multi-composants | Accepté | 0 |
| [0011](0011-docs-as-code.md) | Documentation as code plutôt que Wiki GitHub | Accepté | 0 |
| [0012](0012-monorepo-et-versioning.md) | Monorepo et versioning SemVer par domaine | Accepté | 0 |

## Proposer une nouvelle décision

Ouvrir une Issue « Proposition d'ADR » (gabarit dédié), en discuter, puis
soumettre la nouvelle ADR par Pull Request avec le préfixe de commit `adr:`.

# Politique de sécurité

Companion Platform est un appareil électronique connecté (Wi-Fi/BLE via
Seeed XIAO ESP32-S3) destiné à durer des années et à accueillir de
nombreuses apps et modules tiers. La sécurité du firmware, du processus de
mise à jour et de l'écosystème CX-Bus est donc traitée avec sérieux, dès les
premières phases du projet.

## Périmètre

Sont couverts par cette politique :

- Le **firmware** (kernel, HAL, drivers, services, Companion SDK).
- Le **processus de mise à jour** du firmware, notamment les mécanismes
  **OTA (Over-The-Air)** prévus dans la roadmap.
- Les mécanismes matériels de démarrage sécurisé prévus pour l'ESP32-S3 :
  **Secure Boot** et **Flash Encryption** (à activer en configuration de
  production lorsque la plateforme atteindra la maturité correspondante).
- Le protocole et le standard **CX-Bus** (identification des modules,
  communication inter-cartes).
- Les outils officiels (`tools/`) lorsqu'ils manipulent des artefacts de
  build, de flash ou de provisioning.

Ne sont **pas couverts** par cette politique (à traiter comme des bugs
fonctionnels classiques via les Issues, pas comme des vulnérabilités) :

- Les apps ou modules tiers non maintenus par le projet, sauf si la faille
  provient d'une API du Companion SDK ou de la HAL elle-même.
- Les problèmes purement esthétiques ou d'ergonomie sans impact sur la
  sécurité.

> **État du projet.** Les mécanismes Secure Boot, Flash Encryption et OTA
> sécurisé sont **prévus** mais pas nécessairement tous implémentés à ce
> stade du projet. Consultez `docs/roadmap.md` et les ADR concernées pour
> l'état d'avancement réel. Un signalement reste pertinent même sur des
> mécanismes en cours de conception.

## Signaler une vulnérabilité

**Merci de ne pas ouvrir d'Issue publique pour une vulnérabilité de
sécurité.** La divulgation publique avant correction expose l'ensemble des
utilisateurs de la plateforme.

Pour signaler une vulnérabilité de façon responsable, utilisez les
**GitHub Security Advisories privés** du dépôt :

1. Rendez-vous sur l'onglet **Security** du dépôt GitHub.
2. Cliquez sur **Report a vulnerability** pour ouvrir un avis de sécurité
   privé, visible uniquement par les mainteneurs.
3. Décrivez le problème avec autant de détails que possible :
   - Composant concerné (firmware, HAL, driver, OTA, CX-Bus, outil...).
   - Version ou tag concerné (ex. `fw-1.2.0`, `os-0.1.0`).
   - Étapes de reproduction, preuve de concept si disponible.
   - Impact estimé (exécution de code, déni de service, fuite
     d'information, contournement de Secure Boot, etc.).

Si GitHub Security Advisories n'est pas accessible pour une raison
quelconque, contactez l'équipe via un canal privé listé dans les
Discussions du dépôt, en indiquant clairement qu'il s'agit d'un
signalement de sécurité.

Merci de ne pas tester une vulnérabilité potentielle sur du matériel ou une
infrastructure appartenant à des tiers sans leur autorisation.

## Délais de réponse indicatifs

Ces délais sont des objectifs, pas des garanties contractuelles :

| Étape                                            | Délai indicatif        |
|---------------------------------------------------|-------------------------|
| Accusé de réception du signalement                 | Sous 5 jours ouvrés      |
| Première évaluation (sévérité, périmètre confirmé) | Sous 14 jours            |
| Publication d'un correctif ou d'une mitigation     | Selon sévérité, généralement sous 90 jours |
| Divulgation publique coordonnée                    | Après correctif disponible, en accord avec la personne ayant signalé |

Les vulnérabilités critiques (exécution de code à distance, contournement
de Secure Boot, compromission de la chaîne de mise à jour OTA) sont
traitées en priorité et peuvent accélérer ce calendrier.

## Reconnaissance

Sauf demande contraire de votre part, les personnes ayant signalé une
vulnérabilité de façon responsable seront créditées dans les notes de
version du correctif correspondant.

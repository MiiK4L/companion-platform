// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Modèle AppArtifactView — VUE OPAQUE et NEUTRE d'un artefact d'application.
// Candidat, non figé. Le service NE l'interprète PAS : ni taille, ni contenu,
// ni mode d'accès. La façon de le résoudre (streaming, mapping mémoire,
// chargement intégral, par nom / par hash…) relève des ADAPTATEURS ou de
// décisions futures (DEC-L8-001/DEC-L8-002), pas de ce squelette.
//
// CONTRAT DE DURÉE DE VIE (candidat, révisable) :
//   - La vue et les données qu'elle référence (`id`, `handle`) sont la propriété
//     de la SOURCE (l'adaptateur IAppSource), pas du service ni du runtime.
//   - La vue est EMPRUNTÉE : elle n'est valide que **pendant l'appel** à
//     `IRuntime.launch()`. Le runtime NE PEUT PAS la conserver après le retour.
//   - AUCUN transfert de propriété, AUCUNE libération par le service/runtime.
//   - Cette version candidate impose uniquement une validité **pendant l'appel** ;
//     un besoin de rétention ultérieure serait une décision future explicite.
#ifndef COMPANION_MODELS_APP_ARTIFACT_VIEW_H
#define COMPANION_MODELS_APP_ARTIFACT_VIEW_H

typedef struct AppArtifactView {
  const char *id;   // libellé opaque (diagnostic/journal seulement), emprunté
  void *handle;     // poignée opaque vers la source ; JAMAIS déréférencée par le service
} AppArtifactView;

#endif  // COMPANION_MODELS_APP_ARTIFACT_VIEW_H

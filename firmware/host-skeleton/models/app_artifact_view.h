// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Modèle AppArtifactView — VUE OPAQUE et NEUTRE d'un artefact d'application.
// Candidat, non figé. Le service NE l'interprète PAS : ni taille, ni contenu,
// ni mode d'accès. La façon de le résoudre (streaming, mapping mémoire,
// chargement intégral, par nom / par hash…) relève des ADAPTATEURS ou de
// décisions futures (DEC-L8-001/DEC-L8-002), pas de ce squelette.
#ifndef COMPANION_MODELS_APP_ARTIFACT_VIEW_H
#define COMPANION_MODELS_APP_ARTIFACT_VIEW_H

typedef struct AppArtifactView {
  const char *id;   // identifiant logique opaque (diagnostic/journal seulement)
  void *handle;     // poignée opaque vers la source ; JAMAIS déréférencée par le service
} AppArtifactView;

#endif  // COMPANION_MODELS_APP_ARTIFACT_VIEW_H

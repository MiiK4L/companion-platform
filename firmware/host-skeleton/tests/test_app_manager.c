// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Tests host de l'AppManager via la composition root, avec mocks/bouchon.
// Démontrent UNIQUEMENT : injection des ports, appel du port runtime avec une
// vue opaque, propagation succès/échec, journalisation via ILog, et
// remplacement du FakeRuntime sans modifier le service.
// AUCUN test ne fige de taille (4096), d'adressage par nom, ni de cycle load->run.
#include "composition/composition_root.h"
#include "models/app_reference.h"
#include "tests/test_support.h"

// Poignée opaque factice : le service ne doit jamais la déréférencer.
static int kOpaque;

// Références OPAQUES factices : de simples octets, SANS signification (ni nom, ni
// chemin, ni chaîne terminée). `kKnownBytes` est la référence reconnue ;
// `kOtherBytes` en diffère par le contenu — le test ne dépend d'aucune sémantique.
static const unsigned char kKnownBytes[] = {0x01, 0x02, 0x03};
static const unsigned char kOtherBytes[] = {0x09};

static AppReference known_ref(void) {
  AppReference r = {kKnownBytes, sizeof(kKnownBytes)};
  return r;
}

static AppReference other_ref(void) {
  AppReference r = {kOtherBytes, sizeof(kOtherBytes)};
  return r;
}

// Résolution OK -> délégation au runtime -> succès + log INFO.
void test_launch_success(void) {
  HostComposition c;
  host_composition_build(&c, known_ref(), &kOpaque, RT_OK);

  am_status_t r = app_manager_launch(&c.app_manager, known_ref());

  EXPECT_EQ_INT(r, AM_OK, "launch should succeed");
  EXPECT_EQ_INT(c.runtime.launch_calls, 1, "runtime.launch called once");
  EXPECT(c.runtime.last_artifact_id == 0 ? 0 : 1, "runtime received an artifact view");
  EXPECT_EQ_INT(c.log.count[LOG_INFO], 1, "one INFO log on success");
}

// Source ne résout pas (référence inconnue) -> erreur source, runtime JAMAIS appelé.
void test_source_not_found(void) {
  HostComposition c;
  host_composition_build(&c, known_ref(), &kOpaque, RT_OK);

  am_status_t r = app_manager_launch(&c.app_manager, other_ref());

  EXPECT_EQ_INT(r, AM_ERR_SOURCE, "unresolved ref -> source error");
  EXPECT_EQ_INT(c.runtime.launch_calls, 0, "runtime not called on source error");
  EXPECT_EQ_INT(c.log.count[LOG_WARN], 1, "one WARN log on source error");
}

// Le runtime (bouchon) échoue -> l'AppManager propage l'erreur runtime.
void test_runtime_failure(void) {
  HostComposition c;
  host_composition_build(&c, known_ref(), &kOpaque, RT_ERR);

  am_status_t r = app_manager_launch(&c.app_manager, known_ref());

  EXPECT_EQ_INT(r, AM_ERR_RUNTIME, "runtime failure propagated");
  EXPECT_EQ_INT(c.runtime.launch_calls, 1, "runtime was invoked once");
  EXPECT_EQ_INT(c.log.count[LOG_ERROR], 1, "one ERROR log on runtime failure");
}

// Remplacement du runtime (statut différent) SANS toucher au service.
void test_runtime_swappable(void) {
  HostComposition ok;
  host_composition_build(&ok, known_ref(), &kOpaque, RT_OK);
  HostComposition ko;
  host_composition_build(&ko, known_ref(), &kOpaque, RT_ERR);

  // Même service (même code), moteurs bouchons différents -> résultats différents.
  EXPECT_EQ_INT(app_manager_launch(&ok.app_manager, known_ref()), AM_OK,
                "swap: OK runtime -> success");
  EXPECT_EQ_INT(app_manager_launch(&ko.app_manager, known_ref()), AM_ERR_RUNTIME,
                "swap: failing runtime -> runtime error");
}

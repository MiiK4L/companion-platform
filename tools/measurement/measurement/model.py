# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Vocabulaire et regles transverses du socle experimental.

Regroupe : natures d'acquisition, statuts de preuve, verdicts, surete des noms
de fichiers/identifiants et regle de completude des metadonnees pour une
campagne REELLE. Aucune dependance a un framework de test.
"""

from __future__ import annotations

import re
from typing import Any

#: Version de l'outillage (deterministe ; distincte du commit git, cf. build_manifest).
TOOLING_VERSION = "0.1.0"

#: Nature de l'acquisition (declaree explicitement par chaque driver).
ACQUISITION_NATURES = ("measured", "simulated")

#: Statut de preuve. Une acquisition reelle sort en RAW ; la promotion vers M est
#: une operation explicite et controlee. Une acquisition simulee est terminale en S.
EVIDENCE_RAW = "RAW"
EVIDENCE_REVIEWED = "REVIEWED"
EVIDENCE_MEASURED = "M"
EVIDENCE_SIMULATED = "S"
EVIDENCE_STATUSES = (EVIDENCE_RAW, EVIDENCE_REVIEWED, EVIDENCE_MEASURED, EVIDENCE_SIMULATED)

#: Verdicts possibles d'un run (un resultat ambigu n'est pas force en PASS/FAIL).
VERDICTS = ("PASS", "FAIL", "INCONCLUSIVE", "INVALID", "NOT_RUN")
#: Verdicts decisifs autorisant (sous conditions) une promotion en preuve mesuree.
DECISIVE_VERDICTS = ("PASS", "FAIL")

#: Types d'evenements de l'historique append-only.
EVENT_TYPES = ("acquisition", "review", "verdict", "promotion")

#: Statut de baseline (enum).
BASELINE_STATUSES = ("approved", "draft", "not_applicable")

#: Motifs partages avec les schemas (source unique).
SHA256_PATTERN = r"^[a-f0-9]{64}$"
ISO8601_PATTERN = r"^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(\.\d+)?(Z|[+-]\d{2}:\d{2})$"
DEC_PATTERN = r"^DEC-[A-Z0-9]+-[0-9]{3}$"
SAFE_ID_PATTERN = r"^[A-Za-z0-9][A-Za-z0-9_-]*$"

#: Champs de tracabilite obligatoires (details d'evenement) pour un run REEL.
REVIEW_REQUIRED_FIELDS = ("reviewer", "reviewed_at", "review_reason", "review_checklist")
PROMOTION_REQUIRED_FIELDS = ("promoted_by", "promoted_at", "promotion_reason")
VERDICT_LINK_FIELDS = (
    "verdict_rule_ref",
    "analysis_result_sha256",
    "analysis_tool",
    "analysis_tool_version",
    "decided_by",
    "decided_at",
)

#: Nom de serie : minuscule, sans separateur de chemin (empeche toute evasion).
SERIES_NAME_RE = re.compile(r"^[a-z0-9][a-z0-9_-]*$")
#: Nom de fichier (composant unique) : points autorises, aucun separateur de chemin.
FILENAME_RE = re.compile(r"^[A-Za-z0-9][A-Za-z0-9._-]*$")
#: Identifiant injectable (run_id, experiment_id) : sans point ni separateur de chemin.
SAFE_ID_RE = re.compile(SAFE_ID_PATTERN)


class UnsafeNameError(ValueError):
    """Nom de fichier ou identifiant non conforme au format sur."""


class IncompleteMetadataError(Exception):
    """Metadonnees obligatoires manquantes pour une campagne reelle."""


def ensure_series_name(name: str) -> str:
    """Valide un nom de serie (minuscule, sans separateur de chemin)."""
    if not isinstance(name, str) or not SERIES_NAME_RE.match(name):
        raise UnsafeNameError(
            f"nom de serie invalide {name!r} (attendu {SERIES_NAME_RE.pattern})"
        )
    return name


def ensure_safe_id(value: str, *, kind: str) -> str:
    """Valide un identifiant injectable (run_id, experiment_id)."""
    if not isinstance(value, str) or not SAFE_ID_RE.match(value):
        raise UnsafeNameError(f"{kind} invalide {value!r} (attendu {SAFE_ID_RE.pattern})")
    return value


def ensure_filename(name: str) -> str:
    """Valide un nom de fichier (composant unique, sans separateur de chemin)."""
    if not isinstance(name, str) or ".." in name or not FILENAME_RE.match(name):
        raise UnsafeNameError(
            f"nom de fichier invalide {name!r} (attendu {FILENAME_RE.pattern}, sans '..')"
        )
    return name


# Champs obligatoires d'une campagne REELLE (mesuree). Chaque feuille doit etre
# renseignee ; "N/A" (explicite) est accepte quand un champ n'est pas applicable,
# mais pas une chaine vide ni un objet vide.
MEASURED_REQUIRED: dict[str, list[str] | None] = {
    "setup": None,
    "fixture": None,
    "instrument": [
        "brand",
        "model",
        "firmware",
        "probe",
        "calibre",
        "bandwidth",
        "parameters",
    ],
    "dut": ["description", "hardware_revision"],
    "build_manifest": [
        "git_commit",
        "git_dirty",
        "toolchain",
        "esp_idf_version",
        "target",
        "build_configuration_hash",
        "artifact_sha256",
        "tooling_version",
        "measurement_tooling_commit",
    ],
    "conditions": ["temperature", "supply", "environment"],
    "calibration": ["reference", "verified_at"],
    "baseline": ["status", "record"],
}


def _is_filled(value: Any) -> bool:
    if isinstance(value, bool):
        return True
    return isinstance(value, str) and value.strip() != ""


def missing_measured_fields(context: dict[str, Any]) -> list[str]:
    """Liste les champs obligatoires manquants/vides d'un contexte reel."""
    missing: list[str] = []
    for key, subkeys in MEASURED_REQUIRED.items():
        if key not in context:
            missing.append(key)
            continue
        value = context[key]
        if subkeys is None:
            if not _is_filled(value):
                missing.append(key)
            continue
        if not isinstance(value, dict):
            missing.append(key)
            continue
        for sub in subkeys:
            if sub not in value or not _is_filled(value[sub]):
                missing.append(f"{key}.{sub}")
    return missing


def require_complete_context(context: dict[str, Any]) -> None:
    """Leve ``IncompleteMetadataError`` si un contexte reel est incomplet."""
    missing = missing_measured_fields(context)
    if missing:
        raise IncompleteMetadataError(
            "metadonnees obligatoires manquantes (campagne reelle) : " + ", ".join(missing)
        )

# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Orchestration d'une campagne.

Flux : definition (description) + contexte (implementation) -> acquisition ->
archivage autosuffisant -> (analyse/verdict/revue/promotion, operations
explicites). Une acquisition REELLE sort en statut BRUT (``RAW``) : elle ne
devient une preuve ``[M]`` que par ``promote_to_measured`` (operation controlee).
Une acquisition SIMULEE est terminale en ``S`` et ne peut jamais devenir ``[M]``.

Aucune logique specifique a une campagne : le comportement depend uniquement de
la definition, du contexte et du driver choisi.
"""

from __future__ import annotations

import csv
import io
from datetime import UTC, datetime
from pathlib import Path
from typing import Any

from ..acquisition import get_driver
from ..analysis import schema as schema_mod
from ..common.canonical import canonical_json
from ..common.hashing import sha256_file, sha256_text
from ..common.ids import campaign_definition_id, new_run_id
from ..model import (
    DECISIVE_VERDICTS,
    EVIDENCE_MEASURED,
    EVIDENCE_RAW,
    EVIDENCE_REVIEWED,
    EVIDENCE_SIMULATED,
    TOOLING_VERSION,
    VERDICTS,
    ensure_safe_id,
    ensure_series_name,
    require_complete_context,
)
from ..reporting.report import render_report


class GuardrailError(Exception):
    """Violation d'un garde-fou (ex. promouvoir une donnee non qualifiee)."""


def _now_iso() -> str:
    return datetime.now(UTC).isoformat()


def _series_csv_text(series: dict[str, Any]) -> str:
    """Genere le CSV d'une serie via le module standard (quoting/newlines surs)."""
    buffer = io.StringIO()
    writer = csv.writer(buffer, lineterminator="\n")
    writer.writerow([column["name"] for column in series["columns"]])
    for row in series["rows"]:
        writer.writerow(row)
    return buffer.getvalue()


def _tooling_context() -> dict[str, Any]:
    """Contexte d'execution par defaut pour une SIMULATION (outillage seul)."""
    na = "N/A (simulation)"
    return {
        "setup": na,
        "fixture": na,
        "instrument": {
            "brand": na,
            "model": na,
            "firmware": na,
            "probe": na,
            "calibre": na,
            "bandwidth": na,
            "parameters": na,
        },
        "dut": {"description": na, "hardware_revision": na},
        "build_manifest": {
            "git_commit": na,
            "git_dirty": False,
            "toolchain": na,
            "esp_idf_version": na,
            "target": na,
            "build_configuration_hash": na,
            "artifact_sha256": na,
            "tooling_version": TOOLING_VERSION,
            "measurement_tooling_commit": na,
        },
        "conditions": {"temperature": na, "supply": na, "environment": na},
        "calibration": {"reference": na, "verified_at": na},
        "baseline": {"status": "N/A (simulation)", "record": na},
    }


def _load_json(path: Path) -> dict[str, Any]:
    import json

    return json.loads(path.read_text(encoding="utf-8"))


def _write_json(path: Path, obj: dict[str, Any]) -> str:
    text = canonical_json(obj) + "\n"
    path.write_text(text, encoding="utf-8")
    return sha256_text(text)


def run_campaign(
    definition: dict[str, Any],
    out_dir: str | Path,
    *,
    context: dict[str, Any] | None = None,
    run_id: str | None = None,
    generated_at: str | None = None,
) -> tuple[Path, dict[str, Any]]:
    """Execute une campagne et ecrit un run AUTOSUFFISANT sous ``out_dir``.

    Pour une acquisition REELLE, ``context`` est obligatoire et doit etre complet
    (sinon ``IncompleteMetadataError``). Pour une SIMULATION, un contexte
    d'outillage est genere si ``context`` est absent. ``run_id``/``generated_at``
    sont injectables (reproductibilite).
    """
    schema_mod.validate(definition, schema_mod.load_schema("campaign-definition.schema.json"))
    experiment_id = ensure_safe_id(definition["experiment_id"], kind="experiment_id")
    definition_id = campaign_definition_id(definition)

    acquisition = definition["acquisition"]
    driver = get_driver(acquisition["driver"])()
    nature = driver.nature

    if nature == "simulated":
        context = context or _tooling_context()
        schema_mod.validate(context, schema_mod.load_schema("execution-context.schema.json"))
        evidence_status = EVIDENCE_SIMULATED
    else:  # measured
        if context is None:
            from ..model import IncompleteMetadataError

            raise IncompleteMetadataError(
                "campagne reelle : contexte d'execution obligatoire (absent)"
            )
        schema_mod.validate(context, schema_mod.load_schema("execution-context.schema.json"))
        require_complete_context(context)
        evidence_status = EVIDENCE_RAW

    series_list = driver.acquire(definition_id, acquisition.get("config", {}))

    run_id = ensure_safe_id(run_id, kind="run_id") if run_id else new_run_id()
    generated_at = generated_at or _now_iso()

    run_dir = Path(out_dir) / definition_id / run_id
    series_dir = run_dir / "series"
    series_dir.mkdir(parents=True, exist_ok=True)

    # Archive autosuffisante : definition + contexte exacts (canoniques).
    definition_sha256 = _write_json(run_dir / "campaign-definition.json", definition)
    context_sha256 = _write_json(run_dir / "execution-context.json", context)

    artifacts: list[dict[str, str]] = []
    for series in series_list:
        name = ensure_series_name(series["name"])
        schema_mod.validate(series, schema_mod.load_schema("measurement-series.schema.json"))
        csv_text = _series_csv_text(series)
        (series_dir / f"{name}.csv").write_text(csv_text, encoding="utf-8")
        artifacts.append({"path": f"series/{name}.csv", "sha256": sha256_text(csv_text)})
    artifacts.sort(key=lambda entry: entry["path"])

    manifest: dict[str, Any] = {
        "experiment_id": experiment_id,
        "campaign_definition_id": definition_id,
        "definition_sha256": definition_sha256,
        "context_sha256": context_sha256,
        "run_id": run_id,
        "generated_at": generated_at,
        "acquisition_nature": nature,
        "evidence_status": evidence_status,
        "verdict": "NOT_RUN",
        "verdict_reason": "analyse non executee",
        "protocol_ref": definition["protocol_ref"],
        "dec": definition["dec"],
        "tooling_version": TOOLING_VERSION,
        "artifacts": artifacts,
    }
    schema_mod.validate(manifest, schema_mod.load_schema("run-manifest.schema.json"))

    _write_json(run_dir / "manifest.json", manifest)
    _write_verdict(run_dir, manifest)
    (run_dir / "report.md").write_text(render_report(manifest, context), encoding="utf-8")
    return run_dir, manifest


def _verdict_record(manifest: dict[str, Any]) -> dict[str, Any]:
    return {
        "experiment_id": manifest["experiment_id"],
        "verdict": manifest["verdict"],
        "verdict_reason": manifest["verdict_reason"],
        "evidence_status": manifest["evidence_status"],
        "acquisition_nature": manifest["acquisition_nature"],
    }


def _write_verdict(run_dir: Path, manifest: dict[str, Any]) -> None:
    record = _verdict_record(manifest)
    schema_mod.validate(record, schema_mod.load_schema("verdict.schema.json"))
    _write_json(run_dir / "verdict.json", record)


def _persist(run_dir: Path, manifest: dict[str, Any], context: dict[str, Any]) -> None:
    schema_mod.validate(manifest, schema_mod.load_schema("run-manifest.schema.json"))
    _write_json(run_dir / "manifest.json", manifest)
    _write_verdict(run_dir, manifest)
    (run_dir / "report.md").write_text(render_report(manifest, context), encoding="utf-8")


def record_verdict(run_dir: str | Path, verdict: str, reason: str) -> dict[str, Any]:
    """Enregistre un verdict explicite (PASS/FAIL/INCONCLUSIVE/INVALID/NOT_RUN)."""
    if verdict not in VERDICTS:
        raise ValueError(f"verdict inconnu: {verdict!r} (attendu {VERDICTS})")
    run_dir = Path(run_dir)
    manifest = _load_json(run_dir / "manifest.json")
    context = _load_json(run_dir / "execution-context.json")
    manifest["verdict"] = verdict
    manifest["verdict_reason"] = reason
    _persist(run_dir, manifest, context)
    return manifest


def record_analysis(run_dir: str | Path, analysis: dict[str, Any]) -> None:
    """Archive un resultat d'analyse (preuve que l'analyse a ete executee)."""
    run_dir = Path(run_dir)
    _write_json(run_dir / "analysis-result.json", analysis)


def mark_reviewed(run_dir: str | Path) -> dict[str, Any]:
    """Passe une acquisition REELLE de RAW a REVIEWED (revue requise par le cadre)."""
    run_dir = Path(run_dir)
    manifest = _load_json(run_dir / "manifest.json")
    context = _load_json(run_dir / "execution-context.json")
    if manifest["acquisition_nature"] != "measured":
        raise GuardrailError("revue applicable uniquement a une acquisition reelle")
    if manifest["evidence_status"] != EVIDENCE_RAW:
        raise GuardrailError(
            f"revue impossible depuis le statut {manifest['evidence_status']!r}"
        )
    manifest["evidence_status"] = EVIDENCE_REVIEWED
    _persist(run_dir, manifest, context)
    return manifest


def verify_run(run_dir: str | Path) -> None:
    """Verifie l'integrite (SHA-256) de la definition, du contexte et des series.

    Leve ``GuardrailError`` a la premiere divergence.
    """
    run_dir = Path(run_dir)
    manifest = _load_json(run_dir / "manifest.json")
    if sha256_file(run_dir / "campaign-definition.json") != manifest["definition_sha256"]:
        raise GuardrailError("empreinte de la definition divergente")
    if sha256_file(run_dir / "execution-context.json") != manifest["context_sha256"]:
        raise GuardrailError("empreinte du contexte divergente")
    for artifact in manifest["artifacts"]:
        if sha256_file(run_dir / artifact["path"]) != artifact["sha256"]:
            raise GuardrailError(f"empreinte divergente: {artifact['path']}")


def promote_to_measured(run_dir: str | Path) -> dict[str, Any]:
    """Promeut un run REEL, REVU et QUALIFIE vers la preuve ``[M]``.

    Operation explicite et controlee. Verifie : campagne reelle ; revue faite ;
    baseline approuvee ; metadonnees completes ; artefacts integres ; analyse
    executee ; verdict decisif (PASS/FAIL). Une SIMULATION ne peut JAMAIS etre
    promue.
    """
    run_dir = Path(run_dir)
    manifest = _load_json(run_dir / "manifest.json")
    context = _load_json(run_dir / "execution-context.json")

    if manifest["acquisition_nature"] == "simulated":
        raise GuardrailError("une acquisition simulee ne peut jamais devenir [M]")
    if manifest["evidence_status"] != EVIDENCE_REVIEWED:
        raise GuardrailError("promotion refusee : revue prealable requise (REVIEWED)")
    if context.get("baseline", {}).get("status") != "approved":
        raise GuardrailError("promotion refusee : baseline non approuvee")
    require_complete_context(context)  # metadonnees obligatoires
    verify_run(run_dir)  # integrite des artefacts
    if not (run_dir / "analysis-result.json").is_file():
        raise GuardrailError("promotion refusee : analyse non executee")
    if manifest["verdict"] not in DECISIVE_VERDICTS:
        raise GuardrailError(
            f"promotion refusee : verdict non decisif ({manifest['verdict']})"
        )

    manifest["evidence_status"] = EVIDENCE_MEASURED
    _persist(run_dir, manifest, context)
    return manifest


def normalize_manifest(manifest: dict[str, Any]) -> dict[str, Any]:
    """Copie sans les champs volatils (run_id, generated_at) pour comparaison."""
    from ..common.ids import VOLATILE_FIELDS

    normalized = dict(manifest)
    for field in VOLATILE_FIELDS:
        normalized.pop(field, None)
    return normalized

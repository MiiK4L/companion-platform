# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Orchestration d'une campagne (historique append-only, immuabilite a M).

Le RESULTAT est immuable : les transitions RAW -> REVIEWED -> M et les verdicts
sont consignes dans un HISTORIQUE APPEND-ONLY (``evidence-events/``), chaine par
``previous_event_sha256``. L'``acquisition-manifest.json`` est immuable ;
``evidence-state.json`` et ``archive-index.json`` sont des vues DERIVEES. Une fois
``M`` atteint, le run est VERROUILLE : toute correction cree un nouveau run.

Aucune logique specifique a une campagne.
"""

from __future__ import annotations

import csv
import io
import json
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
    PROMOTION_REQUIRED_FIELDS,
    REVIEW_REQUIRED_FIELDS,
    TOOLING_VERSION,
    VERDICT_LINK_FIELDS,
    VERDICTS,
    IncompleteMetadataError,
    ensure_safe_id,
    ensure_series_name,
    require_complete_context,
)
from ..reporting.report import render_report

# Artefacts IMMUABLES (candidats) référencés par l'index d'intégrité.
_IMMUTABLE_TOPLEVEL = (
    "acquisition-manifest.json",
    "campaign-definition.json",
    "execution-context.json",
    "baseline-record.json",
    "analysis-result.json",
    "dirty-diff.patch",
)

# Vues DERIVEES (regenerables depuis les artefacts autoritaires).
_DERIVED_FILES = ("evidence-state.json", "archive-index.json", "report.md")

# Schema de details par type d'evenement (validation semantique).
_EVENT_DETAILS_SCHEMA = {
    "acquisition": "event-details-acquisition.schema.json",
    "review": "event-details-review.schema.json",
    "verdict": "event-details-verdict.schema.json",
    "promotion": "event-details-promotion.schema.json",
}

# Transitions de statut legales par type d'evenement (from -> to autorisees).
_LEGAL_TRANSITIONS = {
    "acquisition": {("NONE", "RAW"), ("NONE", "S")},
    "verdict": {("RAW", "RAW"), ("REVIEWED", "REVIEWED"), ("S", "S")},
    "review": {("RAW", "REVIEWED")},
    "promotion": {("REVIEWED", "M")},
}


class GuardrailError(Exception):
    """Violation d'un garde-fou (promotion non qualifiee, run verrouille…)."""


def _validate_event_details(event: dict[str, Any]) -> None:
    """Valide les details d'un evenement selon son type (sous-schema dedie)."""
    schema_name = _EVENT_DETAILS_SCHEMA[event["event_type"]]
    schema_mod.validate(event["details"], schema_mod.load_schema(schema_name))


def _now_iso() -> str:
    return datetime.now(UTC).isoformat()


def _series_csv_text(series: dict[str, Any]) -> str:
    buffer = io.StringIO()
    writer = csv.writer(buffer, lineterminator="\n")
    writer.writerow([column["name"] for column in series["columns"]])
    for row in series["rows"]:
        writer.writerow(row)
    return buffer.getvalue()


def _tooling_context() -> dict[str, Any]:
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
        "baseline": {"status": "not_applicable", "record": na},
    }


def _load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def _write_json(path: Path, obj: dict[str, Any]) -> str:
    text = canonical_json(obj) + "\n"
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")
    return sha256_text(text)


def _events_dir(run_dir: Path) -> Path:
    return run_dir / "evidence-events"


def _event_files(run_dir: Path) -> list[Path]:
    directory = _events_dir(run_dir)
    if not directory.is_dir():
        return []
    return sorted(directory.glob("[0-9][0-9][0-9][0-9]-*.json"))


def _immutable_paths(run_dir: Path) -> list[str]:
    paths = [p for p in _IMMUTABLE_TOPLEVEL if (run_dir / p).is_file()]
    paths += [f"series/{p.name}" for p in sorted((run_dir / "series").glob("*.csv"))]
    paths += [f"evidence-events/{p.name}" for p in _event_files(run_dir)]
    return sorted(paths)


def _rebuild_archive_index(run_dir: Path) -> None:
    entries = [
        {"path": rel, "sha256": sha256_file(run_dir / rel)}
        for rel in _immutable_paths(run_dir)
    ]
    index = {"entries": entries}
    schema_mod.validate(index, schema_mod.load_schema("archive-index.schema.json"))
    _write_json(run_dir / "archive-index.json", index)


def _append_event(
    run_dir: Path,
    *,
    event_type: str,
    from_status: str,
    to_status: str,
    actor: str,
    timestamp: str,
    reason: str,
    inputs: list[dict[str, str]],
    details: dict[str, Any],
) -> tuple[str, str]:
    existing = _event_files(run_dir)
    event_id = f"{len(existing) + 1:04d}"
    previous = sha256_file(existing[-1]) if existing else ""
    event = {
        "event_id": event_id,
        "event_type": event_type,
        "from_status": from_status,
        "to_status": to_status,
        "actor": actor,
        "timestamp": timestamp,
        "reason": reason,
        "inputs": inputs,
        "previous_event_sha256": previous,
        "details": details,
    }
    schema_mod.validate(event, schema_mod.load_schema("evidence-event.schema.json"))
    _validate_event_details(event)
    path = _events_dir(run_dir) / f"{event_id}-{event_type}.json"
    event_sha = _write_json(path, event)
    return event_id, event_sha


def _write_state(
    run_dir: Path,
    *,
    experiment_id: str,
    evidence_status: str,
    verdict: str,
    verdict_reason: str,
    latest_event_id: str,
    latest_event_sha256: str,
) -> None:
    state = {
        "experiment_id": experiment_id,
        "evidence_status": evidence_status,
        "verdict": verdict,
        "verdict_reason": verdict_reason,
        "locked": evidence_status == "M",
        "latest_event_id": latest_event_id,
        "latest_event_sha256": latest_event_sha256,
    }
    schema_mod.validate(state, schema_mod.load_schema("evidence-state.schema.json"))
    _write_json(run_dir / "evidence-state.json", state)


def current_view(run_dir: str | Path) -> dict[str, Any]:
    """Vue pratique fusionnant l'acquisition-manifest (immuable) et l'etat courant."""
    run_dir = Path(run_dir)
    manifest = _load_json(run_dir / "acquisition-manifest.json")
    state = _load_json(run_dir / "evidence-state.json")
    return {**manifest, **state}


def _render(run_dir: Path) -> None:
    manifest = _load_json(run_dir / "acquisition-manifest.json")
    state = _load_json(run_dir / "evidence-state.json")
    context = _load_json(run_dir / "execution-context.json")
    (run_dir / "report.md").write_text(
        render_report({**manifest, **state}, context), encoding="utf-8"
    )


def _ensure_not_locked(run_dir: Path) -> None:
    state = _load_json(run_dir / "evidence-state.json")
    if state.get("locked"):
        raise GuardrailError("run verrouille (statut M) : creer un nouveau run")


def run_campaign(
    definition: dict[str, Any],
    out_dir: str | Path,
    *,
    context: dict[str, Any] | None = None,
    baseline_record: dict[str, Any] | None = None,
    dirty_diff: str | None = None,
    run_id: str | None = None,
    generated_at: str | None = None,
    actor: str = "tooling",
) -> tuple[Path, dict[str, Any]]:
    """Execute une campagne et ecrit un run avec historique append-only."""
    schema_mod.validate(definition, schema_mod.load_schema("campaign-definition.schema.json"))
    experiment_id = ensure_safe_id(definition["experiment_id"], kind="experiment_id")
    definition_id = campaign_definition_id(definition)

    acquisition = definition["acquisition"]
    driver = get_driver(acquisition["driver"])()
    nature = driver.nature

    if nature == "simulated":
        context = context or _tooling_context()
        evidence_status = "S"
    else:
        if context is None:
            raise IncompleteMetadataError(
                "campagne reelle : contexte d'execution obligatoire (absent)"
            )
        evidence_status = "RAW"
    schema_mod.validate(context, schema_mod.load_schema("execution-context.schema.json"))
    if nature == "measured":
        require_complete_context(context)
        # Build reel : contrat durci (hashes 64 hex, champs non vides).
        schema_mod.validate(
            context["build_manifest"], schema_mod.load_schema("build-manifest.schema.json")
        )

    series_list = driver.acquire(definition_id, acquisition.get("config", {}))
    run_id = ensure_safe_id(run_id, kind="run_id") if run_id else new_run_id()
    generated_at = generated_at or _now_iso()

    run_dir = Path(out_dir) / definition_id / run_id
    # Un run_id identifie une execution IMMUABLE : jamais d'ecrasement.
    if run_dir.exists():
        raise GuardrailError(
            f"run deja existant (run_id reutilise) : {run_dir} — creer un nouveau run"
        )
    (run_dir / "series").mkdir(parents=True, exist_ok=True)

    definition_sha = _write_json(run_dir / "campaign-definition.json", definition)
    context_sha = _write_json(run_dir / "execution-context.json", context)

    baseline_sha = "N/A"
    if baseline_record is not None:
        schema_mod.validate(
            baseline_record, schema_mod.load_schema("baseline-record.schema.json")
        )
        baseline_sha = _write_json(run_dir / "baseline-record.json", baseline_record)

    artifacts: list[dict[str, str]] = []
    inputs = [
        {"name": "campaign-definition.json", "sha256": definition_sha},
        {"name": "execution-context.json", "sha256": context_sha},
    ]
    for series in series_list:
        name = ensure_series_name(series["name"])
        schema_mod.validate(series, schema_mod.load_schema("measurement-series.schema.json"))
        csv_text = _series_csv_text(series)
        (run_dir / "series" / f"{name}.csv").write_text(csv_text, encoding="utf-8")
        sha = sha256_text(csv_text)
        artifacts.append({"path": f"series/{name}.csv", "sha256": sha})
        inputs.append({"name": f"series/{name}.csv", "sha256": sha})
    artifacts.sort(key=lambda entry: entry["path"])
    if baseline_sha != "N/A":
        inputs.append({"name": "baseline-record.json", "sha256": baseline_sha})

    if dirty_diff is not None:
        (run_dir / "dirty-diff.patch").write_text(dirty_diff, encoding="utf-8")
        diff_sha = sha256_text(dirty_diff)
        declared = context["build_manifest"].get("dirty_diff_sha256")
        if declared != diff_sha:
            raise GuardrailError(
                "dirty_diff_sha256 du contexte ne correspond pas au diff archive"
            )
        inputs.append({"name": "dirty-diff.patch", "sha256": diff_sha})

    manifest = {
        "experiment_id": experiment_id,
        "campaign_definition_id": definition_id,
        "definition_sha256": definition_sha,
        "context_sha256": context_sha,
        "baseline_sha256": baseline_sha,
        "run_id": run_id,
        "generated_at": generated_at,
        "acquisition_nature": nature,
        "protocol_ref": definition["protocol_ref"],
        "dec": definition["dec"],
        "tooling_version": TOOLING_VERSION,
        "artifacts": artifacts,
    }
    schema_mod.validate(manifest, schema_mod.load_schema("acquisition-manifest.schema.json"))
    _write_json(run_dir / "acquisition-manifest.json", manifest)

    event_id, event_sha = _append_event(
        run_dir,
        event_type="acquisition",
        from_status="NONE",
        to_status=evidence_status,
        actor=actor,
        timestamp=generated_at,
        reason="acquisition",
        inputs=sorted(inputs, key=lambda entry: entry["name"]),
        details={"acquisition_nature": nature},
    )
    _write_state(
        run_dir,
        experiment_id=experiment_id,
        evidence_status=evidence_status,
        verdict="NOT_RUN",
        verdict_reason="analyse non executee",
        latest_event_id=event_id,
        latest_event_sha256=event_sha,
    )
    _rebuild_archive_index(run_dir)
    _render(run_dir)
    return run_dir, current_view(run_dir)


def record_verdict(
    run_dir: str | Path,
    verdict: str,
    reason: str,
    *,
    link: dict[str, Any] | None = None,
    actor: str = "tooling",
    timestamp: str | None = None,
) -> dict[str, Any]:
    """Enregistre un verdict, relie a la regle et a l'analyse (link) pour un run reel."""
    if verdict not in VERDICTS:
        raise ValueError(f"verdict inconnu: {verdict!r} (attendu {VERDICTS})")
    run_dir = Path(run_dir)
    _ensure_not_locked(run_dir)
    state = _load_json(run_dir / "evidence-state.json")
    details = {"verdict": verdict, "verdict_reason": reason, **(link or {})}
    event_id, event_sha = _append_event(
        run_dir,
        event_type="verdict",
        from_status=state["evidence_status"],
        to_status=state["evidence_status"],
        actor=actor,
        timestamp=timestamp or _now_iso(),
        reason="verdict",
        inputs=[],
        details=details,
    )
    _write_state(
        run_dir,
        experiment_id=state["experiment_id"],
        evidence_status=state["evidence_status"],
        verdict=verdict,
        verdict_reason=reason,
        latest_event_id=event_id,
        latest_event_sha256=event_sha,
    )
    _rebuild_archive_index(run_dir)
    _render(run_dir)
    return current_view(run_dir)


def record_analysis(run_dir: str | Path, analysis: dict[str, Any]) -> None:
    """Archive un resultat d'analyse IMMUABLE (interdit de reecrire)."""
    run_dir = Path(run_dir)
    _ensure_not_locked(run_dir)
    if (run_dir / "analysis-result.json").is_file():
        raise GuardrailError("analysis-result.json existe deja (immuable)")
    schema_mod.validate(analysis, schema_mod.load_schema("analysis-result.schema.json"))
    _write_json(run_dir / "analysis-result.json", analysis)
    _rebuild_archive_index(run_dir)


def mark_reviewed(
    run_dir: str | Path,
    *,
    reviewer: str,
    reviewed_at: str,
    review_reason: str,
    review_checklist: list[str],
    actor: str | None = None,
) -> dict[str, Any]:
    """Passe RAW -> REVIEWED avec identite de revue (obligatoire, run reel)."""
    run_dir = Path(run_dir)
    _ensure_not_locked(run_dir)
    manifest = _load_json(run_dir / "acquisition-manifest.json")
    state = _load_json(run_dir / "evidence-state.json")
    if manifest["acquisition_nature"] != "measured":
        raise GuardrailError("revue applicable uniquement a une acquisition reelle")
    if state["evidence_status"] != "RAW":
        raise GuardrailError(f"revue impossible depuis {state['evidence_status']!r}")
    details = {
        "reviewer": reviewer,
        "reviewed_at": reviewed_at,
        "review_reason": review_reason,
        "review_checklist": review_checklist,
    }
    for field in REVIEW_REQUIRED_FIELDS:
        if not details.get(field):
            raise GuardrailError(f"revue : champ obligatoire manquant {field!r}")
    event_id, event_sha = _append_event(
        run_dir,
        event_type="review",
        from_status="RAW",
        to_status="REVIEWED",
        actor=actor or reviewer,
        timestamp=reviewed_at,
        reason=review_reason,
        inputs=[],
        details=details,
    )
    _write_state(
        run_dir,
        experiment_id=state["experiment_id"],
        evidence_status="REVIEWED",
        verdict=state["verdict"],
        verdict_reason=state["verdict_reason"],
        latest_event_id=event_id,
        latest_event_sha256=event_sha,
    )
    _rebuild_archive_index(run_dir)
    _render(run_dir)
    return current_view(run_dir)


def _latest_verdict_event(run_dir: Path) -> dict[str, Any] | None:
    latest = None
    for path in _event_files(run_dir):
        event = _load_json(path)
        if event["event_type"] == "verdict":
            latest = event
    return latest


def promote_to_measured(
    run_dir: str | Path,
    *,
    promoted_by: str,
    promoted_at: str,
    promotion_reason: str,
) -> dict[str, Any]:
    """Promeut un run REEL, REVU et QUALIFIE vers ``M`` (operation controlee)."""
    run_dir = Path(run_dir)
    _ensure_not_locked(run_dir)
    manifest = _load_json(run_dir / "acquisition-manifest.json")
    context = _load_json(run_dir / "execution-context.json")
    state = _load_json(run_dir / "evidence-state.json")

    if manifest["acquisition_nature"] == "simulated":
        raise GuardrailError("une acquisition simulee ne peut jamais devenir M")
    if state["evidence_status"] != "REVIEWED":
        raise GuardrailError("promotion refusee : revue prealable requise (REVIEWED)")

    # Baseline reellement archivee et approuvee.
    baseline_path = run_dir / "baseline-record.json"
    if not baseline_path.is_file():
        raise GuardrailError("promotion refusee : baseline-record.json absent")
    baseline = _load_json(baseline_path)
    schema_mod.validate(baseline, schema_mod.load_schema("baseline-record.schema.json"))
    if baseline["status"] != "approved" or context["baseline"]["status"] != "approved":
        raise GuardrailError("promotion refusee : baseline non approuvee")

    # Coherence : la baseline doit appartenir AU protocole execute.
    definition = _load_json(run_dir / "campaign-definition.json")
    if not (
        baseline["protocol_ref"] == definition["protocol_ref"] == manifest["protocol_ref"]
    ):
        raise GuardrailError("promotion refusee : baseline d'un autre protocole")
    if context["baseline"]["record"] != baseline["baseline_id"]:
        raise GuardrailError("promotion refusee : baseline_id incoherent (contexte vs record)")

    require_complete_context(context)

    # Build reproductible : arbre git propre, ou diff archive + hashe + justifie.
    build = context["build_manifest"]
    if build.get("git_dirty"):
        diff = run_dir / "dirty-diff.patch"
        recorded = build.get("dirty_diff_sha256")
        if not diff.is_file() or not recorded or recorded == "N/A":
            raise GuardrailError("promotion refusee : build git sale sans diff archive/hashe")
        if sha256_file(diff) != recorded:
            raise GuardrailError("promotion refusee : empreinte du diff sale divergente")
        if not promotion_reason:
            raise GuardrailError("promotion refusee : justification requise (build sale)")

    verify_run(run_dir)  # integrite complete de l'archive

    if not (run_dir / "analysis-result.json").is_file():
        raise GuardrailError("promotion refusee : analyse non executee")
    analysis = _load_json(run_dir / "analysis-result.json")
    if analysis["experiment_id"] != manifest["experiment_id"]:
        raise GuardrailError("promotion refusee : analyse d'une autre experience")
    verdict_event = _latest_verdict_event(run_dir)
    if verdict_event is None:
        raise GuardrailError("promotion refusee : aucun verdict enregistre")
    link = verdict_event["details"]
    for field in VERDICT_LINK_FIELDS:
        if not link.get(field) or link.get(field) == "N/A":
            raise GuardrailError(f"promotion refusee : lien de verdict manquant {field!r}")
    if link["analysis_result_sha256"] != sha256_file(run_dir / "analysis-result.json"):
        raise GuardrailError("promotion refusee : analyse non liee au verdict")
    if state["verdict"] not in DECISIVE_VERDICTS:
        raise GuardrailError(f"promotion refusee : verdict non decisif ({state['verdict']})")

    details = {
        "promoted_by": promoted_by,
        "promoted_at": promoted_at,
        "promotion_reason": promotion_reason,
    }
    for field in PROMOTION_REQUIRED_FIELDS:
        if not details.get(field):
            raise GuardrailError(f"promotion : champ obligatoire manquant {field!r}")
    event_id, event_sha = _append_event(
        run_dir,
        event_type="promotion",
        from_status="REVIEWED",
        to_status="M",
        actor=promoted_by,
        timestamp=promoted_at,
        reason=promotion_reason,
        inputs=[],
        details=details,
    )
    _write_state(
        run_dir,
        experiment_id=state["experiment_id"],
        evidence_status="M",
        verdict=state["verdict"],
        verdict_reason=state["verdict_reason"],
        latest_event_id=event_id,
        latest_event_sha256=event_sha,
    )
    _rebuild_archive_index(run_dir)
    _render(run_dir)
    return current_view(run_dir)


def _reconstruct_state(run_dir: Path) -> dict[str, Any]:
    """Rejoue l'historique depuis NONE et renvoie l'etat derive ATTENDU.

    Verifie la legalite des transitions (acquisition unique, review depuis RAW,
    promotion depuis REVIEWED, aucun evenement apres M, pas de retour arriere).
    Leve ``GuardrailError`` sur toute transition illegale.
    """
    manifest = _load_json(run_dir / "acquisition-manifest.json")
    files = _event_files(run_dir)
    if not files:
        raise GuardrailError("aucun evenement d'evidence")
    status = "NONE"
    verdict = "NOT_RUN"
    verdict_reason = "analyse non executee"
    seen_acquisition = False
    for path in files:
        event = _load_json(path)
        etype = event["event_type"]
        if status == "M":
            raise GuardrailError("evenement posterieur au statut M")
        if event["from_status"] != status:
            raise GuardrailError(
                f"transition illegale (from={event['from_status']}, attendu {status})"
            )
        if (event["from_status"], event["to_status"]) not in _LEGAL_TRANSITIONS[etype]:
            raise GuardrailError(
                f"transition illegale pour {etype}: "
                f"{event['from_status']} -> {event['to_status']}"
            )
        if etype == "acquisition":
            if seen_acquisition:
                raise GuardrailError("acquisition multiple interdite")
            seen_acquisition = True
        if etype == "verdict":
            verdict = event["details"]["verdict"]
            verdict_reason = event["details"]["verdict_reason"]
        status = event["to_status"]
    if not seen_acquisition:
        raise GuardrailError("evenement d'acquisition initial manquant")
    return {
        "experiment_id": manifest["experiment_id"],
        "evidence_status": status,
        "verdict": verdict,
        "verdict_reason": verdict_reason,
        "locked": status == "M",
        "latest_event_id": _load_json(files[-1])["event_id"],
        "latest_event_sha256": sha256_file(files[-1]),
    }


def rebuild_derived(run_dir: str | Path) -> None:
    """Regenere les vues DERIVEES (etat, index, rapport) depuis l'autoritaire."""
    run_dir = Path(run_dir)
    expected = _reconstruct_state(run_dir)
    _write_state(
        run_dir,
        experiment_id=expected["experiment_id"],
        evidence_status=expected["evidence_status"],
        verdict=expected["verdict"],
        verdict_reason=expected["verdict_reason"],
        latest_event_id=expected["latest_event_id"],
        latest_event_sha256=expected["latest_event_sha256"],
    )
    _rebuild_archive_index(run_dir)
    _render(run_dir)


def verify_run(run_dir: str | Path) -> None:
    """Verifie l'INTEGRALITE de l'archive : artefacts autoritaires (schemas +
    empreintes + chaine + transitions + coherence des references) et, si presentes,
    exactitude des vues DERIVEES (etat, index) vis-a-vis de leur reconstruction.

    Leve ``GuardrailError`` a la premiere divergence.
    """
    run_dir = Path(run_dir)
    manifest = _load_json(run_dir / "acquisition-manifest.json")
    schema_mod.validate(manifest, schema_mod.load_schema("acquisition-manifest.schema.json"))

    # Empreintes des artefacts IMMUABLES enregistrees dans le manifeste.
    if sha256_file(run_dir / "campaign-definition.json") != manifest["definition_sha256"]:
        raise GuardrailError("empreinte de la definition divergente")
    if sha256_file(run_dir / "execution-context.json") != manifest["context_sha256"]:
        raise GuardrailError("empreinte du contexte divergente")
    if manifest["baseline_sha256"] != "N/A":
        if not (run_dir / "baseline-record.json").is_file():
            raise GuardrailError("baseline-record.json attendu mais absent")
        if sha256_file(run_dir / "baseline-record.json") != manifest["baseline_sha256"]:
            raise GuardrailError("empreinte de la baseline divergente")
    for artifact in manifest["artifacts"]:
        if sha256_file(run_dir / artifact["path"]) != artifact["sha256"]:
            raise GuardrailError(f"empreinte divergente: {artifact['path']}")

    # Coherence des references essentielles entre artefacts autoritaires.
    definition = _load_json(run_dir / "campaign-definition.json")
    if definition["experiment_id"] != manifest["experiment_id"]:
        raise GuardrailError("experiment_id incoherent (definition vs manifeste)")
    if definition["protocol_ref"] != manifest["protocol_ref"]:
        raise GuardrailError("protocol_ref incoherent (definition vs manifeste)")
    if definition["dec"] != manifest["dec"]:
        raise GuardrailError("dec incoherent (definition vs manifeste)")
    if (run_dir / "analysis-result.json").is_file():
        analysis = _load_json(run_dir / "analysis-result.json")
        schema_mod.validate(analysis, schema_mod.load_schema("analysis-result.schema.json"))
        if analysis["experiment_id"] != manifest["experiment_id"]:
            raise GuardrailError("experiment_id incoherent (analyse vs manifeste)")
    if (run_dir / "baseline-record.json").is_file():
        baseline = _load_json(run_dir / "baseline-record.json")
        schema_mod.validate(baseline, schema_mod.load_schema("baseline-record.schema.json"))
        if baseline["protocol_ref"] != manifest["protocol_ref"]:
            raise GuardrailError("protocol_ref incoherent (baseline vs manifeste)")

    # Chaine d'evenements append-only + details valides selon le type. Une
    # non-conformite de schema d'un evenement est un defaut d'INTEGRITE.
    previous = ""
    for path in _event_files(run_dir):
        event = _load_json(path)
        try:
            schema_mod.validate(event, schema_mod.load_schema("evidence-event.schema.json"))
            _validate_event_details(event)
        except schema_mod.SchemaError as error:
            raise GuardrailError(f"evenement non conforme ({path.name}): {error}") from error
        if event["previous_event_sha256"] != previous:
            raise GuardrailError(f"chaine d'evenements rompue: {path.name}")
        previous = sha256_file(path)

    # Etat reconstruit depuis l'historique (valide aussi la legalite des transitions).
    expected_state = _reconstruct_state(run_dir)

    # Vues DERIVEES : tolerees absentes (regenerables) ; si presentes, EXACTES.
    state_path = run_dir / "evidence-state.json"
    if state_path.is_file():
        state = _load_json(state_path)
        schema_mod.validate(state, schema_mod.load_schema("evidence-state.schema.json"))
        if state != expected_state:
            raise GuardrailError("evidence-state incoherent avec l'historique reconstruit")

    expected_index = {rel: sha256_file(run_dir / rel) for rel in _immutable_paths(run_dir)}
    index_path = run_dir / "archive-index.json"
    if index_path.is_file():
        index = _load_json(index_path)
        schema_mod.validate(index, schema_mod.load_schema("archive-index.schema.json"))
        indexed = {entry["path"]: entry["sha256"] for entry in index["entries"]}
        if indexed != expected_index:
            raise GuardrailError("archive-index incoherent avec les artefacts immuables")


def normalize_manifest(manifest: dict[str, Any]) -> dict[str, Any]:
    """Copie sans les champs volatils (run_id, generated_at) pour comparaison."""
    from ..common.ids import VOLATILE_FIELDS

    normalized = dict(manifest)
    for field in VOLATILE_FIELDS:
        normalized.pop(field, None)
    return normalized

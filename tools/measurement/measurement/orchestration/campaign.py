# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Orchestration d'une campagne : definition -> acquisition -> analyse ->
rapport -> archivage, avec identifiants deterministe/unique et garde-fou
simulation.

Aucune logique specifique a une campagne : le comportement depend uniquement de
la definition et du driver choisi.
"""

from __future__ import annotations

from datetime import UTC, datetime
from pathlib import Path
from typing import Any

from ..acquisition import get_driver
from ..analysis import schema as schema_mod
from ..common.canonical import canonical_json
from ..common.hashing import sha256_text
from ..common.ids import campaign_definition_id, new_run_id
from ..reporting.report import render_report


class GuardrailError(Exception):
    """Violation d'un garde-fou (ex. marquer 'mesure' une donnee simulee)."""


def _now_iso() -> str:
    return datetime.now(UTC).isoformat()


def _csv_from_series(series: dict[str, Any]) -> str:
    header = ",".join(column["name"] for column in series["columns"])
    lines = [header]
    for row in series["rows"]:
        lines.append(",".join(str(value) for value in row))
    return "\n".join(lines) + "\n"


def run_campaign(
    definition: dict[str, Any],
    out_dir: str | Path,
    *,
    run_id: str | None = None,
    generated_at: str | None = None,
) -> tuple[Path, dict[str, Any]]:
    """Execute une campagne et ecrit son run sous ``out_dir``.

    ``run_id`` et ``generated_at`` sont injectables (reproductibilite) ; laisses
    a ``None``, ils sont generes (uniques/volatils). Retourne ``(run_dir,
    manifest)``.
    """
    schema_mod.validate(definition, schema_mod.load_schema("campaign-definition.schema.json"))

    definition_id = campaign_definition_id(definition)

    acquisition = definition["acquisition"]
    driver = get_driver(acquisition["driver"])()
    series_list = driver.acquire(definition_id, acquisition.get("config", {}))
    nature = driver.nature
    # Garde-fou : une acquisition simulee ne produit jamais de donnee [M].
    data_status = "M" if nature == "measured" else "S"

    run_id = run_id or new_run_id()
    generated_at = generated_at or _now_iso()

    run_dir = Path(out_dir) / definition_id / run_id
    series_dir = run_dir / "series"
    series_dir.mkdir(parents=True, exist_ok=True)

    artifacts: list[dict[str, str]] = []
    for series in series_list:
        schema_mod.validate(series, schema_mod.load_schema("measurement-series.schema.json"))
        csv_text = _csv_from_series(series)
        filename = f"{series['name']}.csv"
        (series_dir / filename).write_text(csv_text, encoding="utf-8")
        artifacts.append({"path": f"series/{filename}", "sha256": sha256_text(csv_text)})
    artifacts.sort(key=lambda entry: entry["path"])

    manifest: dict[str, Any] = {
        "campaign_definition_id": definition_id,
        "run_id": run_id,
        "generated_at": generated_at,
        "nature": nature,
        "data_status": data_status,
        "protocol_ref": definition["protocol_ref"],
        "dec": definition["dec"],
        "instrument": definition.get("instrument", {"used": {}, "configuration": {}}),
        "firmware_under_test": definition.get("firmware_under_test", ""),
        "dut_hardware_revision": definition["dut"].get("hardware_revision", ""),
        "conditions": definition.get("conditions", {}),
        "artifacts": artifacts,
    }
    schema_mod.validate(manifest, schema_mod.load_schema("run-manifest.schema.json"))

    (run_dir / "manifest.json").write_text(canonical_json(manifest) + "\n", encoding="utf-8")
    (run_dir / "report.md").write_text(render_report(manifest), encoding="utf-8")
    return run_dir, manifest


def mark_measured(manifest: dict[str, Any]) -> dict[str, Any]:
    """Marque un manifeste comme mesure ([M]) — INTERDIT si nature simulee.

    Garde-fou central : une campagne simulee ne peut jamais devenir une mesure.
    """
    if manifest["nature"] == "simulated":
        raise GuardrailError(
            "un artefact simule ne peut pas etre marque mesure ([M] interdit)"
        )
    manifest["data_status"] = "M"
    return manifest


def normalize_manifest(manifest: dict[str, Any]) -> dict[str, Any]:
    """Retourne une copie sans les champs volatils (run_id, generated_at).

    Sert a comparer le determinisme : memes entrees -> memes artefacts, hors
    champs explicitement isoles.
    """
    from ..common.ids import VOLATILE_FIELDS

    normalized = dict(manifest)
    for field in VOLATILE_FIELDS:
        normalized.pop(field, None)
    return normalized

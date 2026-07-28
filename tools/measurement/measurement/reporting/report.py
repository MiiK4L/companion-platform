# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Rendu d'un rapport de mesure — EXTENSION structuree du modele
docs/phase-1/templates/measurement-report-template.md (renvoi, pas copie).

Le rapport MATERIALISE le modele complet : il cree toutes les sections, meme
vides avant analyse (le socle ne les calcule pas necessairement maintenant). Le
rendu est deterministe (seuls les champs du manifeste/contexte sont utilises).
Il pointe vers l'artefact EXACT (hash), pas vers un simple nom.
"""

from __future__ import annotations

from typing import Any

_TEMPLATE_REF = "docs/phase-1/templates/measurement-report-template.md"

# Sections analytiques, creees meme vides avant analyse.
_ANALYTIC_SECTIONS = [
    "Question experimentale",
    "Hypothese",
    "Baseline / protocole applique",
    "Resultats analyses",
    "Incertitude",
    "Ecarts au protocole",
    "Verdict",
    "Motif du verdict",
    "Limitations",
    "Suites proposees",
]


def render_report(manifest: dict[str, Any], context: dict[str, Any]) -> str:
    """Rend un rapport Markdown structure et deterministe."""
    build = context.get("build_manifest", {})
    instrument = context.get("instrument", {})
    lines: list[str] = []

    lines.append(f"# Rapport de mesure — {manifest['experiment_id']}")
    lines.append("")
    lines.append(f"> Extension du modele `{_TEMPLATE_REF}` (renvoi, pas copie).")
    if manifest["acquisition_nature"] == "simulated":
        lines.append(">")
        lines.append("> **CAMPAGNE SIMULEE — validation d'outillage uniquement.**")
        lines.append("> Aucune donnee `[M]` ; n'alimente aucune ADR ;")
        lines.append("> ne satisfait aucun critere experimental.")
    lines.append("")

    lines.append("## Identite")
    lines.append(f"- Experience : `{manifest['experiment_id']}`")
    lines.append(f"- Definition : `{manifest['campaign_definition_id']}`")
    lines.append(f"  - SHA-256 : `{manifest['definition_sha256']}`")
    lines.append(f"- Contexte SHA-256 : `{manifest['context_sha256']}`")
    lines.append(f"- Run : `{manifest['run_id']}`")
    lines.append(f"- Genere le : `{manifest['generated_at']}`")
    lines.append(f"- Nature d'acquisition : **{manifest['acquisition_nature']}**")
    lines.append(f"- Statut de preuve : `{manifest['evidence_status']}`")
    lines.append(f"- Outillage : `{manifest['tooling_version']}`")
    lines.append(f"- Protocole : {manifest['protocol_ref']}")
    lines.append(f"- Decision alimentee : {manifest['dec']}")
    lines.append("")

    artifact = build.get("artifact_sha256", "N/A")
    commit = build.get("git_commit", "N/A")
    dirty = build.get("git_dirty", "N/A")
    toolchain = build.get("toolchain", "N/A")
    idf = build.get("esp_idf_version", "N/A")
    target = build.get("target", "N/A")
    cfg = build.get("build_configuration_hash", "N/A")
    mtc = build.get("measurement_tooling_commit", "N/A")
    inst = f"{instrument.get('brand', 'N/A')} {instrument.get('model', 'N/A')}"
    inst_fw = instrument.get("firmware", "N/A")
    lines.append("## Implementation (build & instrument)")
    lines.append(f"- Firmware/build — artefact SHA-256 : `{artifact}`")
    lines.append(f"- Commit : `{commit}` (dirty: {dirty})")
    lines.append(f"- Toolchain : {toolchain} · ESP-IDF : {idf}")
    lines.append(f"- Cible : {target} · config : `{cfg}`")
    lines.append(f"- Outillage de mesure (commit) : `{mtc}`")
    lines.append(f"- Instrument : {inst} (fw {inst_fw})")
    lines.append(
        f"- Setup : {context.get('setup', 'N/A')} · fixture : {context.get('fixture', 'N/A')}"
    )
    lines.append("")

    lines.append("## Artefacts (SHA-256)")
    for artifact in manifest["artifacts"]:
        lines.append(f"- `{artifact['path']}` — `{artifact['sha256']}`")
    lines.append("")

    for section in _ANALYTIC_SECTIONS:
        lines.append(f"## {section}")
        if section == "Verdict":
            lines.append(f"`{manifest['verdict']}`")
        elif section == "Motif du verdict":
            lines.append(manifest["verdict_reason"] or "_(a renseigner)_")
        else:
            lines.append("_(a renseigner)_")
        lines.append("")

    return "\n".join(lines)

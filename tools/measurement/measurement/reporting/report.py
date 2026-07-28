# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Rendu d'un rapport de mesure a partir d'un manifeste de run.

Le rapport est une EXTENSION du modele
docs/phase-1/templates/measurement-report-template.md (renvoi, pas copie) : il
ajoute identite de run, nature/statut, tracabilite et empreintes. Le rendu est
deterministe (aucune source de temps propre : seuls les champs du manifeste sont
utilises, dont l'horodatage volatil deja isole).
"""

from __future__ import annotations

from typing import Any

_TEMPLATE_REF = "docs/phase-1/templates/measurement-report-template.md"


def render_report(manifest: dict[str, Any]) -> str:
    """Rend un rapport Markdown deterministe pour ``manifest``."""
    instrument = manifest.get("instrument", {})
    used = instrument.get("used", {})
    configuration = instrument.get("configuration", {})

    lines: list[str] = []
    lines.append(f"# Rapport de mesure — {manifest['dec']}")
    lines.append("")
    lines.append(
        f"> Rapport genere par le socle experimental — extension du modele `{_TEMPLATE_REF}`."
    )
    if manifest["nature"] == "simulated":
        lines.append(">")
        lines.append("> **CAMPAGNE SIMULEE — validation d'outillage uniquement.**")
        lines.append("> Aucune donnee `[M]` ; n'alimente aucune ADR ;")
        lines.append("> ne satisfait aucun critere experimental.")
    lines.append("")

    lines.append("## Identite")
    lines.append(f"- Definition : `{manifest['campaign_definition_id']}`")
    lines.append(f"- Run : `{manifest['run_id']}`")
    lines.append(f"- Genere le : `{manifest['generated_at']}`")
    lines.append(
        f"- Nature : **{manifest['nature']}** — statut donnees : `{manifest['data_status']}`"
    )
    lines.append(f"- Protocole : {manifest['protocol_ref']}")
    lines.append(f"- Decision alimentee : {manifest['dec']}")
    lines.append("")

    lines.append("## Tracabilite")
    lines.append(f"- Instrument utilise : {used or '(a renseigner)'}")
    lines.append(f"- Configuration : {configuration or '(a renseigner)'}")
    lines.append(f"- Firmware teste : {manifest['firmware_under_test'] or '(a renseigner)'}")
    lines.append(
        f"- Revision materielle DUT : {manifest['dut_hardware_revision'] or '(a renseigner)'}"
    )
    lines.append(f"- Conditions : {manifest['conditions'] or '(a renseigner)'}")
    lines.append("")

    lines.append("## Artefacts (SHA-256)")
    for artifact in manifest["artifacts"]:
        lines.append(f"- `{artifact['path']}` — `{artifact['sha256']}`")
    lines.append("")
    return "\n".join(lines)

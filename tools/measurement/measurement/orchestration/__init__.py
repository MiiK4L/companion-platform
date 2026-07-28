# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Couche orchestration : cycle de vie d'une campagne."""

from .campaign import (
    GuardrailError,
    mark_reviewed,
    normalize_manifest,
    promote_to_measured,
    record_analysis,
    record_verdict,
    run_campaign,
    verify_run,
)

__all__ = [
    "GuardrailError",
    "mark_reviewed",
    "normalize_manifest",
    "promote_to_measured",
    "record_analysis",
    "record_verdict",
    "run_campaign",
    "verify_run",
]

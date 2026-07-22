#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Companion Platform contributors
# SPDX-License-Identifier: Apache-2.0
#
# Vérifie la conformité REUSE du dépôt (en-têtes SPDX sur chaque fichier).
# Reproductible en local et en CI (workflow "Checks", job license-check).
#
# Usage :
#   ./tools/check_licenses.sh
#
# Prérequis : Python 3 + l'outil REUSE (version épinglée) :
#   pip install reuse==6.2.0
set -euo pipefail

if ! command -v reuse >/dev/null 2>&1; then
  echo "REUSE introuvable. Installez-le : pip install reuse==6.2.0" >&2
  exit 127
fi

cd "$(dirname "$0")/.."
exec reuse lint

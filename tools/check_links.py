#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Vérificateur de liens Markdown relatifs — couvre TOUT le dépôt.

Contrairement à `mkdocs build --strict` (qui ne voit que `docs/`), ce contrôle
valide les liens relatifs de **tous** les fichiers Markdown, y compris ceux qui
pointent vers le code source (`firmware/`, `standards/`, `SECURITY.md`, …).

Règles :
- ignore les liens externes (http/https/mailto), absolus (`/…`) et ancres pures (`#…`) ;
- résout chaque lien relatif contre le dossier du fichier ;
- signale les cibles inexistantes ;
- code de sortie 1 s'il existe au moins un lien cassé (utilisable en CI).

Usage : python3 tools/check_links.py
"""
import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SKIP_DIRS = {".git", "site", "node_modules", ".venv"}
LINK_RE = re.compile(r"\]\(([^)]+)\)")
EXTERNAL_RE = re.compile(r"^(https?:|mailto:|tel:|#|/)")


def md_files():
    for root, dirs, files in os.walk(REPO):
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        for fn in files:
            if fn.endswith(".md"):
                yield os.path.join(root, fn)


def main():
    broken = []
    checked = 0
    for path in md_files():
        base = os.path.dirname(path)
        with open(path, encoding="utf-8") as fh:
            text = fh.read()
        for m in LINK_RE.finditer(text):
            target = m.group(1).strip()
            if EXTERNAL_RE.match(target):
                continue
            pathpart = target.split("#", 1)[0]
            if not pathpart:
                continue
            checked += 1
            resolved = os.path.normpath(os.path.join(base, pathpart))
            if not os.path.exists(resolved):
                broken.append((os.path.relpath(path, REPO), target))
    if broken:
        print(f"❌ {len(broken)} lien(s) relatif(s) cassé(s) :")
        for src, tgt in broken:
            print(f"   {src} -> {tgt}")
        return 1
    print(f"✅ {checked} liens relatifs vérifiés, aucun cassé.")
    return 0


if __name__ == "__main__":
    sys.exit(main())

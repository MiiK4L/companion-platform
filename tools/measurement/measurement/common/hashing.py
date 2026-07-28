# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Empreintes SHA-256 des artefacts (texte, octets, fichiers)."""

from __future__ import annotations

import hashlib
from pathlib import Path


def sha256_bytes(data: bytes) -> str:
    """Empreinte SHA-256 hexadecimale d'un bloc d'octets."""
    return hashlib.sha256(data).hexdigest()


def sha256_text(text: str) -> str:
    """Empreinte SHA-256 d'un texte (encode en UTF-8)."""
    return sha256_bytes(text.encode("utf-8"))


def sha256_file(path: str | Path) -> str:
    """Empreinte SHA-256 d'un fichier lu par blocs."""
    digest = hashlib.sha256()
    with open(path, "rb") as handle:
        for chunk in iter(lambda: handle.read(65536), b""):
            digest.update(chunk)
    return digest.hexdigest()

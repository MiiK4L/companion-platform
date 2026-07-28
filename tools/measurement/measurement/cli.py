# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Interface en ligne de commande du socle experimental.

Usage :
    python -m measurement.cli drivers
    python -m measurement.cli run --definition DEF.json --out DIR \\
        [--run-id ID --generated-at TS]   # injection pour reproductibilite
    python -m measurement.cli verify --run DIR   # revalide les empreintes
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

from .acquisition import available_drivers
from .analysis import schema as schema_mod
from .common.hashing import sha256_file
from .orchestration import run_campaign


def _cmd_drivers(_args: argparse.Namespace) -> int:
    for name in available_drivers():
        print(name)
    return 0


def _cmd_run(args: argparse.Namespace) -> int:
    definition = json.loads(Path(args.definition).read_text(encoding="utf-8"))
    run_dir, manifest = run_campaign(
        definition,
        args.out,
        run_id=args.run_id,
        generated_at=args.generated_at,
    )
    print(f"run: {run_dir}")
    print(f"nature: {manifest['nature']} (statut donnees: {manifest['data_status']})")
    return 0


def _cmd_verify(args: argparse.Namespace) -> int:
    run_dir = Path(args.run)
    manifest = json.loads((run_dir / "manifest.json").read_text(encoding="utf-8"))
    schema_mod.validate(manifest, schema_mod.load_schema("run-manifest.schema.json"))
    ok = True
    for artifact in manifest["artifacts"]:
        actual = sha256_file(run_dir / artifact["path"])
        status = "OK" if actual == artifact["sha256"] else "ECHEC"
        if actual != artifact["sha256"]:
            ok = False
        print(f"{status} {artifact['path']}")
    return 0 if ok else 1


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(prog="measurement")
    sub = parser.add_subparsers(dest="command", required=True)

    sub.add_parser("drivers", help="liste les drivers enregistres").set_defaults(
        func=_cmd_drivers
    )

    run = sub.add_parser("run", help="execute une campagne")
    run.add_argument("--definition", required=True)
    run.add_argument("--out", required=True)
    run.add_argument("--run-id", default=None)
    run.add_argument("--generated-at", default=None)
    run.set_defaults(func=_cmd_run)

    verify = sub.add_parser("verify", help="revalide les empreintes d'un run")
    verify.add_argument("--run", required=True)
    verify.set_defaults(func=_cmd_verify)

    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())

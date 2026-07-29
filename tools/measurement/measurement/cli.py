# SPDX-FileCopyrightText: 2026 Companion Platform contributors
#
# SPDX-License-Identifier: Apache-2.0
"""Interface en ligne de commande du socle experimental.

Usage :
    python -m measurement.cli drivers
    python -m measurement.cli run --definition DEF.json --out DIR \\
        [--context CTX.json --run-id ID --generated-at TS]
    python -m measurement.cli verify --run RUN_DIR
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

from .acquisition import available_drivers
from .orchestration import (
    GuardrailError,
    rebuild_derived,
    run_campaign,
    verify_run,
)


def _cmd_drivers(_args: argparse.Namespace) -> int:
    for name in available_drivers():
        print(name)
    return 0


def _cmd_run(args: argparse.Namespace) -> int:
    definition = json.loads(Path(args.definition).read_text(encoding="utf-8"))
    context = None
    if args.context:
        context = json.loads(Path(args.context).read_text(encoding="utf-8"))
    overrides = {"import_dir": args.import_dir} if args.import_dir else None
    run_dir, manifest = run_campaign(
        definition,
        args.out,
        context=context,
        acquisition_overrides=overrides,
        run_id=args.run_id,
        generated_at=args.generated_at,
    )
    print(f"run: {run_dir}")
    print(
        f"nature: {manifest['acquisition_nature']} "
        f"statut: {manifest['evidence_status']} "
        f"verdict: {manifest['verdict']}"
    )
    return 0


def _cmd_verify(args: argparse.Namespace) -> int:
    try:
        verify_run(args.run)
    except GuardrailError as error:
        print(f"ECHEC: {error}")
        return 1
    print("OK: archive integre (schemas, empreintes, chaine d'evenements)")
    return 0


def _cmd_rebuild_derived(args: argparse.Namespace) -> int:
    rebuild_derived(args.run)
    print("OK: vues derivees regenerees (etat, index, rapport)")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(prog="measurement")
    sub = parser.add_subparsers(dest="command", required=True)

    sub.add_parser("drivers", help="liste les drivers enregistres").set_defaults(
        func=_cmd_drivers
    )

    run = sub.add_parser("run", help="execute une campagne")
    run.add_argument("--definition", required=True)
    run.add_argument("--out", required=True)
    run.add_argument("--context", default=None)
    run.add_argument(
        "--import-dir",
        default=None,
        help="repertoire d'import du driver manuel (ephemere, jamais archive)",
    )
    run.add_argument("--run-id", default=None)
    run.add_argument("--generated-at", default=None)
    run.set_defaults(func=_cmd_run)

    verify = sub.add_parser("verify", help="verifie l'integrite d'un run")
    verify.add_argument("--run", required=True)
    verify.set_defaults(func=_cmd_verify)

    rebuild = sub.add_parser(
        "rebuild-derived", help="regenere etat/index/rapport depuis l'autoritaire"
    )
    rebuild.add_argument("--run", required=True)
    rebuild.set_defaults(func=_cmd_rebuild_derived)

    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())

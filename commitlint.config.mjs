// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Configuration commitlint — Companion Platform
//
// Applique la convention Conventional Commits utilisée dans tout le monorepo,
// avec le type additionnel "adr" pour les propositions et mises en œuvre de
// décisions d'architecture (Architecture Decision Records).
//
// Fichier au format ESM (.mjs) : requis par commitlint v19+.

export default {
  extends: ["@commitlint/config-conventional"],
  rules: {
    "type-enum": [
      2,
      "always",
      [
        "feat",
        "fix",
        "docs",
        "chore",
        "refactor",
        "test",
        "ci",
        "adr",
        "build",
        "perf",
        "revert",
        "style",
      ],
    ],
  },
};

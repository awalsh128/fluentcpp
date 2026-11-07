#!/usr/bin/env bash
set -euo pipefail

# Installs pre-commit hooks (requires Python + pip)
if ! command -v pre-commit >/dev/null 2>&1; then
	echo "pre-commit not found; installing..."
	pip install --user pre-commit
fi

pre-commit install
pre-commit autoupdate || true

echo "pre-commit installed. Run 'pre-commit run --all-files' to format and lint all files."

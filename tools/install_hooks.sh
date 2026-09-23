#!/usr/bin/env bash
# Install repo hooks into .git/hooks.
set -euo pipefail
root=$(git rev-parse --show-toplevel)
src="$root/tools/hooks/pre-commit"
chmod +x "$src"
for h in pre-commit commit-msg; do
    ln -sf ../../tools/hooks/pre-commit "$root/.git/hooks/$h"
done
echo "installed: pre-commit, commit-msg -> tools/hooks/pre-commit"

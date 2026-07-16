#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
GODOT="${GODOT_BIN:-/Applications/Godot.app/Contents/MacOS/Godot}"
mkdir -p "$ROOT/.godot"
echo "res://addons/milestro/milestro.gdextension" > "$ROOT/.godot/extension_list.cfg"
export DYLD_LIBRARY_PATH="$ROOT/addons/milestro/bin:${DYLD_LIBRARY_PATH:-}"
export MILESTRO_FONT_PATH="${MILESTRO_FONT_PATH:-}"
"$GODOT" --path "$ROOT" --headless --quit-after 60

#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
GODOT="${GODOT_BIN:-/Applications/Godot.app/Contents/MacOS/Godot}"
mkdir -p "$ROOT/.godot"
echo "res://addons/milestro/milestro.gdextension" > "$ROOT/.godot/extension_list.cfg"
export DYLD_LIBRARY_PATH="$ROOT/addons/milestro/bin:${DYLD_LIBRARY_PATH:-}"
if [[ "$(uname -s)" == "Darwin" ]]; then
  case "$(uname -m)" in
    arm64) export DYLD_LIBRARY_PATH="$ROOT/addons/milestro/bin/macos-arm64:$DYLD_LIBRARY_PATH" ;;
    x86_64) export DYLD_LIBRARY_PATH="$ROOT/addons/milestro/bin/macos-x86_64:$DYLD_LIBRARY_PATH" ;;
  esac
fi
export MILESTRO_FONT_PATH="${MILESTRO_FONT_PATH:-}"
export MILESTRO_VERIFY_LOG="${MILESTRO_VERIFY_LOG:-$ROOT/.godot/verify.log}"
"$GODOT" --path "$ROOT" --headless --rendering-method gl_compatibility --rendering-driver opengl3 \
	--scene "$ROOT/verify_main.tscn" --quit-after 15

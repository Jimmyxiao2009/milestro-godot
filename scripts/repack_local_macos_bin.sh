#!/usr/bin/env bash
# Move previously flat macOS binaries into bin/macos-x86_64/ for the new layout.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$ROOT/addons/milestro/bin"
DST="$SRC/macos-x86_64"
mkdir -p "$DST"
for f in libmilestro.macos.template_debug.dylib libMilestro.dylib icudtl.dat \
         libmilestro.macos.template_debug.x86_64.dylib; do
  if [[ -f "$SRC/$f" ]]; then
    cp -f "$SRC/$f" "$DST/"
    echo "copied $f"
  fi
done
# Prefer arch-suffixed name if only un-suffixed exists
if [[ -f "$DST/libmilestro.macos.template_debug.dylib" && ! -f "$DST/libmilestro.macos.template_debug.x86_64.dylib" ]]; then
  cp -f "$DST/libmilestro.macos.template_debug.dylib" \
        "$DST/libmilestro.macos.template_debug.x86_64.dylib"
  echo "aliased un-suffixed dylib -> x86_64 name"
fi
ls -la "$DST"

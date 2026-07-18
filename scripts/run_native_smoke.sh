#!/usr/bin/env bash
# Drives the real shipped C API via the milestro_smoke binary (same symbols as the GDExtension).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SCRATCH="${MILESTRO_SCRATCH:-$(mktemp -d "${TMPDIR:-/tmp}/milestro-smoke.XXXXXX")}"
MILESTRO_ROOT="${MILESTRO_ROOT:-/Users/JimmyXiao/Milestro}"
BIN="$ROOT/native_smoke/build/milestro_smoke"
LIBDIR="${MILESTRO_ROOT}/cmake-build-relwithdebinfo/lib"

if [[ ! -x "$BIN" ]]; then
  echo "Building milestro_smoke..."
  cmake -S "$ROOT/native_smoke" -B "$ROOT/native_smoke/build" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release -DMILESTRO_ROOT="$MILESTRO_ROOT" \
    -DMILESTRO_LIB_DIR="$LIBDIR"
  cmake --build "$ROOT/native_smoke/build"
fi

export DYLD_LIBRARY_PATH="${LIBDIR}:${DYLD_LIBRARY_PATH:-}"
mkdir -p "$SCRATCH"
"$BIN" \
  "${MILESTRO_FONT_PATH:-$MILESTRO_ROOT/tests/data/font/SourceHanSans-VF.otf.woff2.bytes}" \
  "Source Han Sans VF" \
  "Hello 你好 Milestro" \
  "$SCRATCH" | tee "$SCRATCH/text-render-verify.log"

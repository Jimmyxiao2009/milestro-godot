#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
GODOT="${GODOT_BIN:-/Applications/Godot.app/Contents/MacOS/Godot}"
MILESTRO_ROOT="${MILESTRO_ROOT:-/Users/JimmyXiao/Milestro}"
SCRATCH="${MILESTRO_SCRATCH:-$(mktemp -d "${TMPDIR:-/tmp}/milestro-full.XXXXXX")}"
LOG="$SCRATCH/godot-full-test.log"
CONSOLE_LOG="$SCRATCH/godot-full-test-console.log"

mkdir -p "$SCRATCH" "$ROOT/.godot"

# GDExtensions in addons/ are discovered by Godot after this import hint is
# written.  Keep this local to the test runner so the compatibility demos do
# not need to change their project configuration.
printf '%s\n' "res://addons/milestro/milestro.gdextension" > "$ROOT/.godot/extension_list.cfg"

case "$(uname -s)" in
	Darwin)
		case "$(uname -m)" in
			arm64) EXT_BIN="$ROOT/addons/milestro/bin/macos-arm64" ;;
			x86_64) EXT_BIN="$ROOT/addons/milestro/bin/macos-x86_64" ;;
			*) EXT_BIN="$ROOT/addons/milestro/bin" ;;
		esac
		export DYLD_LIBRARY_PATH="$EXT_BIN:$ROOT/addons/milestro/bin:${MILESTRO_ROOT}/cmake-build-relwithdebinfo/lib:${DYLD_LIBRARY_PATH:-}"
		;;
	Linux)
		export LD_LIBRARY_PATH="$ROOT/addons/milestro/bin/linux-x86_64:${MILESTRO_ROOT}/cmake-build-relwithdebinfo/lib:${LD_LIBRARY_PATH:-}"
		;;
	*)
		export PATH="$ROOT/addons/milestro/bin/windows-x86_64:${PATH}"
		;;
esac
export MILESTRO_FONT_PATH="${MILESTRO_FONT_PATH:-$MILESTRO_ROOT/tests/data/font/SourceHanSans-VF.otf.woff2.bytes}"

echo "Running Godot full API test headlessly..."
set +e
"$GODOT" --path "$ROOT" --headless \
	--rendering-method gl_compatibility --rendering-driver opengl3 \
	--scene "$ROOT/tests/milestro_full_test.tscn" \
	--quit-after "${MILESTRO_FULL_TEST_TIMEOUT:-30}" 2>&1 | tee "$CONSOLE_LOG"
RC=${PIPESTATUS[0]}
set -e

cp "$CONSOLE_LOG" "$LOG"
echo "GODOT_EXIT=$RC"
echo "FULL_TEST_LOG=$LOG"

# A scene crash or a timeout can otherwise leave a misleading zero exit
# status in older Godot versions.  Require the explicit summary marker too.
if [[ "$RC" -eq 0 ]] && ! grep -Fq '[test] PASS: 全部通过! 12/12' "$CONSOLE_LOG"; then
	echo "Full API test did not emit the 12/12 pass marker" >&2
	exit 1
fi

exit "$RC"

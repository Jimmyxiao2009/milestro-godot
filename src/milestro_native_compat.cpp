#include "milestro_native_compat.h"

#include <Milestro/game/milestro_game_retcode.h>
#include <Milestro/skia/Canvas.h>
#include <Milestro/skia/SlimTextDrawSnapshot.h>
#include <Milestro/skia/textlayout/InputBox.h>

int64_t MilestroGodotTextDrawSnapshotPaint(
		milestro::skia::SlimTextDrawSnapshot* snapshot,
		milestro::skia::Canvas* canvas,
		float x,
		float baseline_y) {
	if (!snapshot || !canvas) return MILESTRO_API_RET_FAILED;
	snapshot->paintText(canvas->unwrap(), x, baseline_y);
	return MILESTRO_API_RET_OK;
}

int64_t MilestroGodotInputBoxDrawSnapshotPaint(
		milestro::skia::textlayout::InputBoxDrawSnapshot* snapshot,
		milestro::skia::Canvas* canvas,
		float x,
		float y,
		float width,
		float height) {
	if (!snapshot || !canvas) return MILESTRO_API_RET_FAILED;
	snapshot->paint(canvas->unwrap(), x, y, width, height);
	return MILESTRO_API_RET_OK;
}

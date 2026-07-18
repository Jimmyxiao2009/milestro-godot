#pragma once

#include <cstdint>

namespace milestro::skia {
class Canvas;
class SlimTextDrawSnapshot;
namespace textlayout {
class InputBoxDrawSnapshot;
}
}

// These adapters intentionally live in the GDExtension rather than relying on
// newer C ABI exports that may not exist in an older Milestro checkout.
int64_t MilestroGodotTextDrawSnapshotPaint(
		milestro::skia::SlimTextDrawSnapshot* snapshot,
		milestro::skia::Canvas* canvas,
		float x,
		float baseline_y);

int64_t MilestroGodotInputBoxDrawSnapshotPaint(
		milestro::skia::textlayout::InputBoxDrawSnapshot* snapshot,
		milestro::skia::Canvas* canvas,
		float x,
		float y,
		float width,
		float height);

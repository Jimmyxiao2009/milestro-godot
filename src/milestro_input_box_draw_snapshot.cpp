#include "milestro_input_box_draw_snapshot.h"

#include "milestro_canvas.h"

#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroInputBoxDrawSnapshot::MilestroInputBoxDrawSnapshot() = default;

MilestroInputBoxDrawSnapshot::~MilestroInputBoxDrawSnapshot() {
	if (native_snapshot) {
		MilestroSkiaTextlayoutInputBoxDrawSnapshotDestroy(native_snapshot);
		native_snapshot = nullptr;
	}
}

void MilestroInputBoxDrawSnapshot::_bind_methods() {
	ClassDB::bind_method(D_METHOD("paint", "canvas", "x", "y", "width", "height"), &MilestroInputBoxDrawSnapshot::paint);
}

void MilestroInputBoxDrawSnapshot::set_native(milestro::skia::textlayout::InputBoxDrawSnapshot* snapshot) {
	native_snapshot = snapshot;
}

bool MilestroInputBoxDrawSnapshot::paint(Ref<MilestroCanvas> canvas, float x, float y, float width, float height) {
	if (!native_snapshot || !canvas.is_valid() || !canvas->get_native()) return false;
	return MilestroSkiaTextlayoutInputBoxDrawSnapshotPaint(native_snapshot, canvas->get_native(), x, y, width, height) == 0;
}

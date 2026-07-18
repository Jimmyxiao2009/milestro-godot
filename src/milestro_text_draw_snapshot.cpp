#include "milestro_text_draw_snapshot.h"

#include "milestro_canvas.h"
#include "milestro_font.h"
#include "milestro_native_compat.h"
#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroTextDrawSnapshot::MilestroTextDrawSnapshot() = default;

MilestroTextDrawSnapshot::~MilestroTextDrawSnapshot() {
	if (native_snapshot) {
		MilestroSkiaTextDrawSnapshotDestroy(native_snapshot);
		native_snapshot = nullptr;
	}
}

void MilestroTextDrawSnapshot::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create", "font", "text", "r", "g", "b", "a"), &MilestroTextDrawSnapshot::create);
	ClassDB::bind_method(D_METHOD("measure_text"), &MilestroTextDrawSnapshot::measure_text);
	ClassDB::bind_method(D_METHOD("paint_text", "canvas", "x", "y"), &MilestroTextDrawSnapshot::paint_text);
}

bool MilestroTextDrawSnapshot::create(Ref<MilestroFont> font, const String& text, int32_t r, int32_t g, int32_t b, int32_t a) {
	if (!font.is_valid() || !font->get_native() || text.is_empty()) return false;
	if (native_snapshot) {
		MilestroSkiaTextDrawSnapshotDestroy(native_snapshot);
		native_snapshot = nullptr;
	}
	CharString cs = text.utf8();
	int64_t rc = MilestroSkiaTextDrawSnapshotCreate(native_snapshot, font->get_native(),
			reinterpret_cast<const uint8_t*>(cs.get_data()),
			static_cast<uint64_t>(cs.length()),
			r, g, b, a);
	return rc == 0 && native_snapshot != nullptr;
}

Dictionary MilestroTextDrawSnapshot::measure_text() const {
	Dictionary d;
	d["bounds_left"] = 0.0f;
	d["bounds_top"] = 0.0f;
	d["bounds_right"] = 0.0f;
	d["bounds_bottom"] = 0.0f;
	d["advance_x"] = 0.0f;
	if (!native_snapshot) return d;
	float bl = 0.0f, bt = 0.0f, br = 0.0f, bb = 0.0f, ax = 0.0f;
	MilestroSkiaReusableTextDrawSnapshotMeasureText(native_snapshot, bl, bt, br, bb, ax);
	d["bounds_left"] = bl;
	d["bounds_top"] = bt;
	d["bounds_right"] = br;
	d["bounds_bottom"] = bb;
	d["advance_x"] = ax;
	return d;
}

void MilestroTextDrawSnapshot::paint_text(Ref<MilestroCanvas> canvas, float x, float y) {
	if (!native_snapshot || !canvas.is_valid() || !canvas->get_native()) return;
	MilestroGodotTextDrawSnapshotPaint(native_snapshot, canvas->get_native(), x, y);
}

#include "milestro_reusable_text_draw_snapshot.h"

#include "milestro_canvas.h"
#include "milestro_font.h"
#include "milestro_native_compat.h"
#include "milestro_text_draw_snapshot.h"
#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroReusableTextDrawSnapshot::MilestroReusableTextDrawSnapshot() = default;

MilestroReusableTextDrawSnapshot::~MilestroReusableTextDrawSnapshot() {
	if (native_snapshot) {
		MilestroSkiaReusableTextDrawSnapshotDestroy(native_snapshot);
		native_snapshot = nullptr;
	}
}

void MilestroReusableTextDrawSnapshot::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create", "font", "capacity", "r", "g", "b", "a"), &MilestroReusableTextDrawSnapshot::create);
	ClassDB::bind_method(D_METHOD("update_text", "text"), &MilestroReusableTextDrawSnapshot::update_text);
	ClassDB::bind_method(D_METHOD("copy_text_from", "source"), &MilestroReusableTextDrawSnapshot::copy_text_from);
	ClassDB::bind_method(D_METHOD("measure_text"), &MilestroReusableTextDrawSnapshot::measure_text);
	ClassDB::bind_method(D_METHOD("paint_text", "canvas", "x", "y"), &MilestroReusableTextDrawSnapshot::paint_text);
}

bool MilestroReusableTextDrawSnapshot::create(Ref<MilestroFont> font, int64_t capacity, int32_t r, int32_t g, int32_t b, int32_t a) {
	if (!font.is_valid() || !font->get_native() || capacity <= 0) return false;
	if (native_snapshot) {
		MilestroSkiaReusableTextDrawSnapshotDestroy(native_snapshot);
		native_snapshot = nullptr;
	}
	int64_t rc = MilestroSkiaReusableTextDrawSnapshotCreate(native_snapshot, font->get_native(),
			static_cast<uint64_t>(capacity), r, g, b, a);
	return rc == 0 && native_snapshot != nullptr;
}

bool MilestroReusableTextDrawSnapshot::update_text(const String& text) {
	if (!native_snapshot || text.is_empty()) return false;
	CharString cs = text.utf8();
	int64_t rc = MilestroSkiaReusableTextDrawSnapshotUpdateText(native_snapshot,
			reinterpret_cast<const uint8_t*>(cs.get_data()),
			static_cast<uint64_t>(cs.length()));
	return rc == 0;
}

bool MilestroReusableTextDrawSnapshot::copy_text_from(Ref<MilestroTextDrawSnapshot> source) {
	if (!native_snapshot || !source.is_valid() || !source->get_native()) return false;
	int64_t rc = MilestroSkiaReusableTextDrawSnapshotCopyTextFrom(native_snapshot, source->get_native());
	return rc == 0;
}

Dictionary MilestroReusableTextDrawSnapshot::measure_text() const {
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

void MilestroReusableTextDrawSnapshot::paint_text(Ref<MilestroCanvas> canvas, float x, float y) {
	if (!native_snapshot || !canvas.is_valid() || !canvas->get_native()) return;
	MilestroGodotTextDrawSnapshotPaint(native_snapshot, canvas->get_native(), x, y);
}

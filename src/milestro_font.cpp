#include "milestro_font.h"

#include "milestro_path.h"
#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroFont::MilestroFont() = default;

MilestroFont::~MilestroFont() {
	if (native_font) {
		MilestroSkiaFontDestroy(native_font);
		native_font = nullptr;
	}
}

void MilestroFont::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_path", "glyph_id"), &MilestroFont::get_path);
	ClassDB::bind_method(D_METHOD("get_metrics"), &MilestroFont::get_metrics);
	ClassDB::bind_method(D_METHOD("measure_text", "text"), &MilestroFont::measure_text);
}

Ref<MilestroPath> MilestroFont::get_path(uint16_t glyph_id) {
	if (!native_font) return Ref<MilestroPath>();
	milestro::skia::Path* path = nullptr;
	int64_t rc = MilestroSkiaFontGetPath(native_font, path, glyph_id);
	if (rc != 0 || !path) return Ref<MilestroPath>();
	Ref<MilestroPath> result;
	result.instantiate();
	result->set_native(path);
	return result;
}

Dictionary MilestroFont::get_metrics() const {
	Dictionary d;
	d["ascent"] = 0.0f;
	d["descent"] = 0.0f;
	d["leading"] = 0.0f;
	if (!native_font) return d;
	float ascent = 0.0f, descent = 0.0f, leading = 0.0f;
	MilestroSkiaFontGetMetrics(native_font, ascent, descent, leading);
	d["ascent"] = ascent;
	d["descent"] = descent;
	d["leading"] = leading;
	return d;
}

Dictionary MilestroFont::measure_text(const String& text) {
	Dictionary d;
	d["bounds_left"] = 0.0f;
	d["bounds_top"] = 0.0f;
	d["bounds_right"] = 0.0f;
	d["bounds_bottom"] = 0.0f;
	d["advance_x"] = 0.0f;
	if (!native_font || text.is_empty()) return d;

	CharString cs = text.utf8();
	float bl = 0.0f, bt = 0.0f, br = 0.0f, bb = 0.0f, ax = 0.0f;
	MilestroSkiaFontMeasureText(native_font,
			reinterpret_cast<const uint8_t*>(cs.get_data()),
			static_cast<uint64_t>(cs.length()),
			bl, bt, br, bb, ax);
	d["bounds_left"] = bl;
	d["bounds_top"] = bt;
	d["bounds_right"] = br;
	d["bounds_bottom"] = bb;
	d["advance_x"] = ax;
	return d;
}

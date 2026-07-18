#include "milestro_strut_style.h"

#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroStrutStyle::MilestroStrutStyle() {
	MilestroSkiaTextlayoutStrutStyleCreate(native_style);
}

MilestroStrutStyle::~MilestroStrutStyle() {
	if (native_style) {
		MilestroSkiaTextlayoutStrutStyleDestroy(native_style);
		native_style = nullptr;
	}
}

void MilestroStrutStyle::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_font_families", "families"), &MilestroStrutStyle::set_font_families);
	ClassDB::bind_method(D_METHOD("set_font_style", "weight", "width", "slant"), &MilestroStrutStyle::set_font_style);
	ClassDB::bind_method(D_METHOD("set_font_size", "size"), &MilestroStrutStyle::set_font_size);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &MilestroStrutStyle::set_height);
	ClassDB::bind_method(D_METHOD("set_leading", "leading"), &MilestroStrutStyle::set_leading);
	ClassDB::bind_method(D_METHOD("set_strut_enabled", "enabled"), &MilestroStrutStyle::set_strut_enabled);
	ClassDB::bind_method(D_METHOD("set_force_strut_height", "enabled"), &MilestroStrutStyle::set_force_strut_height);
	ClassDB::bind_method(D_METHOD("set_height_override", "enabled"), &MilestroStrutStyle::set_height_override);
	ClassDB::bind_method(D_METHOD("set_half_leading", "enabled"), &MilestroStrutStyle::set_half_leading);
}

void MilestroStrutStyle::set_font_families(const PackedStringArray& families) {
	if (!native_style || families.is_empty()) return;

	std::vector<std::string> storage;
	std::vector<uint8_t*> ptrs;
	storage.reserve(families.size());
	for (int i = 0; i < families.size(); ++i) {
		CharString cs = families[i].utf8();
		storage.emplace_back(cs.get_data(), cs.length());
	}
	for (auto& s : storage) {
		ptrs.push_back(reinterpret_cast<uint8_t*>(s.data()));
	}
	MilestroSkiaTextlayoutStrutStyleSetFontFamilies(native_style, ptrs.data(), static_cast<uint32_t>(ptrs.size()));
}

void MilestroStrutStyle::set_font_style(int32_t weight, int32_t width, int32_t slant) {
	if (native_style) {
		MilestroSkiaTextlayoutStrutStyleSetFontStyle(native_style, weight, width, slant);
	}
}

void MilestroStrutStyle::set_font_size(float size) {
	if (native_style) {
		MilestroSkiaTextlayoutStrutStyleSetFontSize(native_style, size);
	}
}

void MilestroStrutStyle::set_height(float height) {
	if (native_style) {
		MilestroSkiaTextlayoutStrutStyleSetHeight(native_style, height);
	}
}

void MilestroStrutStyle::set_leading(float leading) {
	if (native_style) {
		MilestroSkiaTextlayoutStrutStyleSetLeading(native_style, leading);
	}
}

void MilestroStrutStyle::set_strut_enabled(bool enabled) {
	if (native_style) {
		MilestroSkiaTextlayoutStrutStyleSetStrutEnabled(native_style, enabled ? 1 : 0);
	}
}

void MilestroStrutStyle::set_force_strut_height(bool enabled) {
	if (native_style) {
		MilestroSkiaTextlayoutStrutStyleSetForceStrutHeight(native_style, enabled ? 1 : 0);
	}
}

void MilestroStrutStyle::set_height_override(bool enabled) {
	if (native_style) {
		MilestroSkiaTextlayoutStrutStyleSetHeightOverride(native_style, enabled ? 1 : 0);
	}
}

void MilestroStrutStyle::set_half_leading(bool enabled) {
	if (native_style) {
		MilestroSkiaTextlayoutStrutStyleSetHalfLeading(native_style, enabled ? 1 : 0);
	}
}

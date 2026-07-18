#include "milestro_text_style.h"

#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroTextStyle::MilestroTextStyle() {
	MilestroSkiaTextlayoutTextStyleCreate(native_style);
}

MilestroTextStyle::~MilestroTextStyle() {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleDestroy(native_style);
		native_style = nullptr;
	}
}

void MilestroTextStyle::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_color", "r", "g", "b", "a"), &MilestroTextStyle::set_color);
	ClassDB::bind_method(D_METHOD("set_font_size", "size"), &MilestroTextStyle::set_font_size);
	ClassDB::bind_method(D_METHOD("set_font_families", "families"), &MilestroTextStyle::set_font_families);
	ClassDB::bind_method(D_METHOD("set_font_style", "weight", "width", "slant"), &MilestroTextStyle::set_font_style);
	ClassDB::bind_method(D_METHOD("add_shadow", "color_r", "color_g", "color_b", "color_a", "offset_x", "offset_y", "blur_sigma"), &MilestroTextStyle::add_shadow);
	ClassDB::bind_method(D_METHOD("reset_shadow"), &MilestroTextStyle::reset_shadow);
	ClassDB::bind_method(D_METHOD("set_baseline_shift", "shift"), &MilestroTextStyle::set_baseline_shift);
	ClassDB::bind_method(D_METHOD("set_letter_spacing", "spacing"), &MilestroTextStyle::set_letter_spacing);
	ClassDB::bind_method(D_METHOD("set_word_spacing", "spacing"), &MilestroTextStyle::set_word_spacing);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &MilestroTextStyle::set_height);
	ClassDB::bind_method(D_METHOD("set_height_override", "enabled"), &MilestroTextStyle::set_height_override);
	ClassDB::bind_method(D_METHOD("set_half_leading", "enabled"), &MilestroTextStyle::set_half_leading);
	ClassDB::bind_method(D_METHOD("set_decoration", "decoration"), &MilestroTextStyle::set_decoration);
	ClassDB::bind_method(D_METHOD("set_decoration_mode", "mode"), &MilestroTextStyle::set_decoration_mode);
	ClassDB::bind_method(D_METHOD("set_decoration_color", "r", "g", "b", "a"), &MilestroTextStyle::set_decoration_color);
	ClassDB::bind_method(D_METHOD("set_decoration_style", "style"), &MilestroTextStyle::set_decoration_style);
	ClassDB::bind_method(D_METHOD("set_decoration_thickness_multiplier", "multiplier"), &MilestroTextStyle::set_decoration_thickness_multiplier);
	ClassDB::bind_method(D_METHOD("add_font_feature", "feature", "value"), &MilestroTextStyle::add_font_feature);
	ClassDB::bind_method(D_METHOD("reset_font_features"), &MilestroTextStyle::reset_font_features);
	ClassDB::bind_method(D_METHOD("set_locale", "locale"), &MilestroTextStyle::set_locale);
	ClassDB::bind_method(D_METHOD("set_text_baseline", "baseline"), &MilestroTextStyle::set_text_baseline);
	ClassDB::bind_method(D_METHOD("set_placeholder"), &MilestroTextStyle::set_placeholder);
}

void MilestroTextStyle::set_color(int32_t r, int32_t g, int32_t b, int32_t a) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetColor(native_style, r, g, b, a);
	}
}

void MilestroTextStyle::set_font_size(double size) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetFontSize(native_style, static_cast<float>(size));
	}
}

void MilestroTextStyle::set_font_families(const PackedStringArray& families) {
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
	MilestroSkiaTextlayoutTextStyleSetFontFamilies(native_style, ptrs.data(), static_cast<uint32_t>(ptrs.size()));
}

void MilestroTextStyle::set_font_style(int32_t weight, int32_t width, int32_t slant) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetFontStyle(native_style, weight, width, slant);
	}
}

void MilestroTextStyle::add_shadow(int32_t color_r, int32_t color_g, int32_t color_b, int32_t color_a,
		float offset_x, float offset_y, double blur_sigma) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleAddShadow(native_style, color_r, color_g, color_b, color_a,
				offset_x, offset_y, blur_sigma);
	}
}

void MilestroTextStyle::reset_shadow() {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleResetShadow(native_style);
	}
}

void MilestroTextStyle::set_baseline_shift(float shift) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetBaselineShift(native_style, shift);
	}
}

void MilestroTextStyle::set_letter_spacing(float spacing) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetLetterSpacing(native_style, spacing);
	}
}

void MilestroTextStyle::set_word_spacing(float spacing) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetWordSpacing(native_style, spacing);
	}
}

void MilestroTextStyle::set_height(float height) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetHeight(native_style, height);
	}
}

void MilestroTextStyle::set_height_override(bool enabled) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetHeightOverride(native_style, enabled ? 1 : 0);
	}
}

void MilestroTextStyle::set_half_leading(bool enabled) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetHalfLeading(native_style, enabled ? 1 : 0);
	}
}

void MilestroTextStyle::set_decoration(int32_t decoration) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetDecoration(native_style, decoration);
	}
}

void MilestroTextStyle::set_decoration_mode(int32_t mode) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetDecorationMode(native_style, mode);
	}
}

void MilestroTextStyle::set_decoration_color(int32_t r, int32_t g, int32_t b, int32_t a) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetDecorationColor(native_style, r, g, b, a);
	}
}

void MilestroTextStyle::set_decoration_style(int32_t style) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetDecorationStyle(native_style, style);
	}
}

void MilestroTextStyle::set_decoration_thickness_multiplier(float multiplier) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetDecorationThicknessMultiplier(native_style, multiplier);
	}
}

void MilestroTextStyle::add_font_feature(const String& feature, int32_t value) {
	if (!native_style || feature.is_empty()) return;
	CharString cs = feature.utf8();
	MilestroSkiaTextlayoutTextStyleAddFontFeature(native_style,
			reinterpret_cast<uint8_t*>(const_cast<char*>(cs.get_data())), value);
}

void MilestroTextStyle::reset_font_features() {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleResetFontFeatures(native_style);
	}
}

void MilestroTextStyle::set_locale(const String& locale) {
	if (!native_style || locale.is_empty()) return;
	CharString cs = locale.utf8();
	MilestroSkiaTextlayoutTextStyleSetLocale(native_style, reinterpret_cast<uint8_t*>(const_cast<char*>(cs.get_data())));
}

void MilestroTextStyle::set_text_baseline(int32_t baseline) {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetTextBaseline(native_style, baseline);
	}
}

void MilestroTextStyle::set_placeholder() {
	if (native_style) {
		MilestroSkiaTextlayoutTextStyleSetPlaceholder(native_style);
	}
}

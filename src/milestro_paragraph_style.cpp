#include "milestro_paragraph_style.h"

#include "milestro_text_style.h"
#include "milestro_strut_style.h"
#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroParagraphStyle::MilestroParagraphStyle() {
	MilestroSkiaTextlayoutParagraphStyleCreate(native_style);
}

MilestroParagraphStyle::~MilestroParagraphStyle() {
	if (native_style) {
		MilestroSkiaTextlayoutParagraphStyleDestroy(native_style);
		native_style = nullptr;
	}
}

void MilestroParagraphStyle::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_text_style", "style"), &MilestroParagraphStyle::set_text_style);
	ClassDB::bind_method(D_METHOD("set_strut_style", "style"), &MilestroParagraphStyle::set_strut_style);
	ClassDB::bind_method(D_METHOD("set_text_direction", "direction"), &MilestroParagraphStyle::set_text_direction);
	ClassDB::bind_method(D_METHOD("set_text_align", "align"), &MilestroParagraphStyle::set_text_align);
	ClassDB::bind_method(D_METHOD("set_max_lines", "lines"), &MilestroParagraphStyle::set_max_lines);
	ClassDB::bind_method(D_METHOD("clear_max_lines"), &MilestroParagraphStyle::clear_max_lines);
	ClassDB::bind_method(D_METHOD("set_ellipsis", "text"), &MilestroParagraphStyle::set_ellipsis);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &MilestroParagraphStyle::set_height);
	ClassDB::bind_method(D_METHOD("set_text_height_behavior", "behavior"), &MilestroParagraphStyle::set_text_height_behavior);
	ClassDB::bind_method(D_METHOD("set_replace_tab_characters", "enabled"), &MilestroParagraphStyle::set_replace_tab_characters);
	ClassDB::bind_method(D_METHOD("set_apply_rounding_hack", "enabled"), &MilestroParagraphStyle::set_apply_rounding_hack);
}

void MilestroParagraphStyle::set_text_style(Ref<MilestroTextStyle> style) {
	if (native_style && style.is_valid() && style->get_native()) {
		MilestroSkiaTextlayoutParagraphStyleSetTextStyle(native_style, style->get_native());
	}
}

void MilestroParagraphStyle::set_strut_style(Ref<MilestroStrutStyle> style) {
	if (native_style && style.is_valid() && style->get_native()) {
		MilestroSkiaTextlayoutParagraphStyleSetStrutStyle(native_style, style->get_native());
	}
}

void MilestroParagraphStyle::set_text_direction(int32_t direction) {
	if (native_style) {
		MilestroSkiaTextlayoutParagraphStyleSetTextDirection(native_style, direction);
	}
}

void MilestroParagraphStyle::set_text_align(int32_t align) {
	if (native_style) {
		MilestroSkiaTextlayoutParagraphStyleSetTextAlign(native_style, align);
	}
}

void MilestroParagraphStyle::set_max_lines(int64_t lines) {
	if (native_style) {
		MilestroSkiaTextlayoutParagraphStyleSetMaxLines(native_style, static_cast<uint64_t>(lines));
	}
}

void MilestroParagraphStyle::clear_max_lines() {
	if (native_style) {
		MilestroSkiaTextlayoutParagraphStyleClearMaxLines(native_style);
	}
}

void MilestroParagraphStyle::set_ellipsis(const String& text) {
	if (!native_style || text.is_empty()) return;
	CharString cs = text.utf8();
	MilestroSkiaTextlayoutParagraphStyleSetEllipsis(native_style, reinterpret_cast<uint8_t*>(const_cast<char*>(cs.get_data())));
}

void MilestroParagraphStyle::set_height(float height) {
	if (native_style) {
		MilestroSkiaTextlayoutParagraphStyleSetHeight(native_style, height);
	}
}

void MilestroParagraphStyle::set_text_height_behavior(int32_t behavior) {
	if (native_style) {
		MilestroSkiaTextlayoutParagraphStyleSetTextHeightBehavior(native_style, behavior);
	}
}

void MilestroParagraphStyle::set_replace_tab_characters(bool enabled) {
	if (native_style) {
		MilestroSkiaTextlayoutParagraphStyleSetReplaceTabCharacters(native_style, enabled ? 1 : 0);
	}
}

void MilestroParagraphStyle::set_apply_rounding_hack(bool enabled) {
	if (native_style) {
		MilestroSkiaTextlayoutParagraphStyleSetApplyRoundingHack(native_style, enabled ? 1 : 0);
	}
}

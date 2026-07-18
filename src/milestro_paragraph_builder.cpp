#include "milestro_paragraph_builder.h"

#include "milestro_text_style.h"
#include "milestro_paragraph_style.h"
#include "milestro_paragraph.h"
#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroParagraphBuilder::MilestroParagraphBuilder() {
	// native_builder is created in init()
}

MilestroParagraphBuilder::~MilestroParagraphBuilder() {
	if (native_builder) {
		MilestroSkiaTextlayoutParagraphBuilderDestroy(native_builder);
		native_builder = nullptr;
	}
}

void MilestroParagraphBuilder::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init", "style"), &MilestroParagraphBuilder::init);
	ClassDB::bind_method(D_METHOD("push_style", "style"), &MilestroParagraphBuilder::push_style);
	ClassDB::bind_method(D_METHOD("pop"), &MilestroParagraphBuilder::pop);
	ClassDB::bind_method(D_METHOD("add_text", "text"), &MilestroParagraphBuilder::add_text);
	ClassDB::bind_method(D_METHOD("build"), &MilestroParagraphBuilder::build);
}

bool MilestroParagraphBuilder::init(Ref<MilestroParagraphStyle> style) {
	if (!style.is_valid() || !style->get_native()) return false;
	if (native_builder) {
		MilestroSkiaTextlayoutParagraphBuilderDestroy(native_builder);
	}
	int64_t rc = MilestroSkiaTextlayoutParagraphBuilderCreate(native_builder, style->get_native());
	return rc == 0 && native_builder != nullptr;
}

void MilestroParagraphBuilder::push_style(Ref<MilestroTextStyle> style) {
	if (native_builder && style.is_valid() && style->get_native()) {
		MilestroSkiaTextlayoutParagraphBuilderPushStyle(native_builder, style->get_native());
	}
}

void MilestroParagraphBuilder::pop() {
	if (native_builder) {
		MilestroSkiaTextlayoutParagraphBuilderPop(native_builder);
	}
}

void MilestroParagraphBuilder::add_text(const String& text) {
	if (!native_builder || text.is_empty()) return;
	CharString cs = text.utf8();
	MilestroSkiaTextlayoutParagraphBuilderAddText(native_builder,
			reinterpret_cast<uint8_t*>(const_cast<char*>(cs.get_data())),
			static_cast<size_t>(cs.length()));
}

Ref<MilestroParagraph> MilestroParagraphBuilder::build() {
	if (!native_builder) return Ref<MilestroParagraph>();
	milestro::skia::textlayout::Paragraph* para = nullptr;
	int64_t rc = MilestroSkiaTextlayoutParagraphBuilderBuild(native_builder, para);
	if (rc != 0 || !para) return Ref<MilestroParagraph>();
	Ref<MilestroParagraph> result;
	result.instantiate();
	result->set_native(para);
	return result;
}

#include "milestro_paragraph.h"

#include "milestro_canvas.h"
#include "milestro_path.h"
#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <vector>

using namespace godot;

MilestroParagraph::MilestroParagraph() = default;

MilestroParagraph::~MilestroParagraph() {
	if (native_paragraph) {
		MilestroSkiaTextlayoutParagraphDestroy(native_paragraph);
		native_paragraph = nullptr;
	}
}

void MilestroParagraph::_bind_methods() {
	ClassDB::bind_method(D_METHOD("layout", "width"), &MilestroParagraph::layout);
	ClassDB::bind_method(D_METHOD("get_height"), &MilestroParagraph::get_height);
	ClassDB::bind_method(D_METHOD("get_longest_line"), &MilestroParagraph::get_longest_line);
	ClassDB::bind_method(D_METHOD("get_max_intrinsic_width"), &MilestroParagraph::get_max_intrinsic_width);
	ClassDB::bind_method(D_METHOD("paint", "canvas", "x", "y"), &MilestroParagraph::paint);
	ClassDB::bind_method(D_METHOD("to_sdf", "sdf_width", "sdf_height", "sdf_scale", "x", "y"), &MilestroParagraph::to_sdf);
	ClassDB::bind_method(D_METHOD("to_path", "x", "y"), &MilestroParagraph::to_path);
}

bool MilestroParagraph::layout(float width) {
	if (!native_paragraph) return false;
	return MilestroSkiaTextlayoutParagraphLayout(native_paragraph, width) == 0;
}

float MilestroParagraph::get_height() const {
	if (!native_paragraph) return 0.0f;
	float h = 0.0f;
	MilestroSkiaTextlayoutParagraphGetHeight(native_paragraph, h);
	return h;
}

float MilestroParagraph::get_longest_line() const {
	if (!native_paragraph) return 0.0f;
	float l = 0.0f;
	MilestroSkiaTextlayoutParagraphGetLongestLine(native_paragraph, l);
	return l;
}

float MilestroParagraph::get_max_intrinsic_width() const {
	if (!native_paragraph) return 0.0f;
	float w = 0.0f;
	MilestroSkiaTextlayoutParagraphGetMaxIntrinsicWidth(native_paragraph, w);
	return w;
}

bool MilestroParagraph::paint(Ref<MilestroCanvas> canvas, float x, float y) {
	if (!native_paragraph || !canvas.is_valid() || !canvas->get_native()) return false;
	return MilestroSkiaTextlayoutParagraphPaint(native_paragraph, canvas->get_native(), x, y) == 0;
}

PackedByteArray MilestroParagraph::to_sdf(int32_t sdf_width, int32_t sdf_height, float sdf_scale, float x, float y) {
	if (!native_paragraph || sdf_width <= 0 || sdf_height <= 0) return PackedByteArray();
	PackedByteArray distance_field;
	distance_field.resize(static_cast<int64_t>(sdf_width) * static_cast<int64_t>(sdf_height));
	if (MilestroSkiaTextlayoutParagraphToSDF(native_paragraph, sdf_width, sdf_height, sdf_scale, x, y,
			distance_field.ptrw()) != 0) {
		return PackedByteArray();
	}
	return distance_field;
}

Ref<MilestroPath> MilestroParagraph::to_path(float x, float y) {
	if (!native_paragraph) return Ref<MilestroPath>();
	milestro::skia::Path* path = nullptr;
	int64_t rc = MilestroSkiaTextlayoutParagraphToPath(native_paragraph, path, x, y);
	if (rc != 0 || !path) return Ref<MilestroPath>();
	Ref<MilestroPath> result;
	result.instantiate();
	result->set_native(path);
	return result;
}

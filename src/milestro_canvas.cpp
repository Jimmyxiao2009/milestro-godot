#include "milestro_canvas.h"

#include "milestro_image.h"
#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

MilestroCanvas::MilestroCanvas() = default;

MilestroCanvas::~MilestroCanvas() {
	if (native_canvas) {
		MilestroSkiaCanvasDestroy(native_canvas);
		native_canvas = nullptr;
	}
}

void MilestroCanvas::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create", "width", "height"), &MilestroCanvas::create);
	ClassDB::bind_method(D_METHOD("create_with_memory", "width", "height", "pixels"), &MilestroCanvas::create_with_memory);
	ClassDB::bind_method(D_METHOD("draw_image_simple", "image", "x", "y"), &MilestroCanvas::draw_image_simple);
	ClassDB::bind_method(D_METHOD("draw_image", "image", "src_left", "src_top", "src_right", "src_bottom", "dst_left", "dst_top", "dst_right", "dst_bottom"), &MilestroCanvas::draw_image);
	ClassDB::bind_method(D_METHOD("get_texture"), &MilestroCanvas::get_texture);
	ClassDB::bind_method(D_METHOD("save_to_png", "path"), &MilestroCanvas::save_to_png);
}

bool MilestroCanvas::create(int32_t width, int32_t height) {
	if (width <= 0 || height <= 0) return false;
	if (native_canvas) {
		MilestroSkiaCanvasDestroy(native_canvas);
	}
	int64_t rc = MilestroSkiaCanvasCreate(native_canvas, width, height);
	if (rc != 0 || !native_canvas) return false;
	canvas_width = width;
	canvas_height = height;
	pixel_buffer.resize(width * height * 4);
	return true;
}

bool MilestroCanvas::create_with_memory(int32_t width, int32_t height, const PackedByteArray& pixels) {
	if (width <= 0 || height <= 0) return false;
	const int64_t required_size = static_cast<int64_t>(width) * static_cast<int64_t>(height) * 4;
	if (pixels.size() < required_size) return false;
	if (native_canvas) {
		MilestroSkiaCanvasDestroy(native_canvas);
	}
	pixel_buffer = pixels;
	void* pixel_ptr = pixel_buffer.ptrw();
	int64_t rc = MilestroSkiaCanvasCreateWithMemory(native_canvas, width, height, pixel_ptr, 0, 0);
	if (rc != 0 || !native_canvas) return false;
	canvas_width = width;
	canvas_height = height;
	return true;
}

void MilestroCanvas::draw_image_simple(Ref<MilestroImage> image, float x, float y) {
	if (!native_canvas || !image.is_valid() || !image->get_native()) return;
	MilestroSkiaCanvasDrawImageSimple(native_canvas, image->get_native(), x, y);
}

void MilestroCanvas::draw_image(Ref<MilestroImage> image,
		float src_left, float src_top, float src_right, float src_bottom,
		float dst_left, float dst_top, float dst_right, float dst_bottom) {
	if (!native_canvas || !image.is_valid() || !image->get_native()) return;
	MilestroSkiaCanvasDrawImage(native_canvas, image->get_native(),
			src_left, src_top, src_right, src_bottom,
			dst_left, dst_top, dst_right, dst_bottom);
}

PackedByteArray MilestroCanvas::get_texture() const {
	if (!native_canvas || pixel_buffer.is_empty()) return PackedByteArray();
	MilestroSkiaCanvasGetTexture(native_canvas, pixel_buffer.ptrw());
	return pixel_buffer;
}

bool MilestroCanvas::save_to_png(const String& path) {
	if (!native_canvas || path.is_empty() || canvas_width <= 0 || canvas_height <= 0) return false;
	PackedByteArray pixels = get_texture();
	if (pixels.is_empty()) return false;
	Ref<Image> image = Image::create_from_data(canvas_width, canvas_height, false,
			Image::FORMAT_RGBA8, pixels);
	if (image.is_null() || image->is_empty()) return false;
	return image->save_png(path) == Error::OK;
}

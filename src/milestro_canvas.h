#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::skia {
	class Canvas;
}

namespace godot {

class MilestroImage;

/// Canvas wrapper for Milestro Skia.
class MilestroCanvas : public RefCounted {
	GDCLASS(MilestroCanvas, RefCounted)

protected:
	static void _bind_methods();

	friend class MilestroParagraph;
	friend class MilestroSvg;
	friend class MilestroTextDrawSnapshot;
	friend class MilestroReusableTextDrawSnapshot;
	friend class MilestroInputBoxDrawSnapshot;

	milestro::skia::Canvas* get_native() const { return native_canvas; }

public:
	MilestroCanvas();
	~MilestroCanvas() override;

	/// Create a canvas with the given dimensions.
	bool create(int32_t width, int32_t height);

	/// Create a canvas with existing pixel memory.
	bool create_with_memory(int32_t width, int32_t height, const PackedByteArray& pixels);

	/// Draw an image at position (x, y).
	void draw_image_simple(Ref<MilestroImage> image, float x, float y);

	/// Draw an image with source and destination rectangles.
	void draw_image(Ref<MilestroImage> image,
			float src_left, float src_top, float src_right, float src_bottom,
			float dst_left, float dst_top, float dst_right, float dst_bottom);

	/// Get the pixel data as a PackedByteArray.
	PackedByteArray get_texture() const;

	/// Save the canvas to a PNG file.
	bool save_to_png(const String& path);

private:
	milestro::skia::Canvas* native_canvas = nullptr;
	mutable PackedByteArray pixel_buffer;
	int32_t canvas_width = 0;
	int32_t canvas_height = 0;
};

} // namespace godot

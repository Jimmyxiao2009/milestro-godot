#pragma once

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

/// Thin Godot facade over the native Milestro C plugin API.
class Milestro : public RefCounted {
	GDCLASS(Milestro, RefCounted)

protected:
	static void _bind_methods();

public:
	Milestro() = default;
	~Milestro() override = default;

	/// Returns { "major", "minor", "patch", "ok" } from real MilestroGetVersion.
	Dictionary get_version() const;

	/// Register a font file with the native FontRegistry. Returns native status code (0 = OK).
	int64_t register_font_from_file(const String &path) const;

	/// Layout + paint text into an RGBA Image (CPU raster path).
	Ref<Image> render_text(const String &text, const String &font_family, double font_size,
			int32_t width, int32_t height) const;

	/// Same as render_text but also returns metrics for verification scripts.
	Dictionary render_text_metrics(const String &text, const String &font_family, double font_size,
			int32_t width, int32_t height) const;

	/// Multi-family text (e.g. CJK + emoji) plus optional image overlays on the same canvas.
	/// image_paths: filesystem paths (png/jpg/avif/svg supported by Skia build).
	/// image_positions: top-left of each image in canvas space.
	/// image_heights: target draw height (width keeps aspect); use 0 for native size.
	Dictionary render_mixed(const String &text, const PackedStringArray &font_families, double font_size,
			int32_t width, int32_t height, const PackedStringArray &image_paths,
			const PackedVector2Array &image_positions, const PackedFloat32Array &image_heights) const;
};

} // namespace godot

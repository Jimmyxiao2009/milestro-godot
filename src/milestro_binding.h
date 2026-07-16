#pragma once

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
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

	/// Layout + paint sample text into an RGBA Image (CPU raster path).
	/// Returns empty Image on failure.
	Ref<Image> render_text(const String &text, const String &font_family, double font_size,
			int32_t width, int32_t height) const;

	/// Same as render_text but also returns metrics for verification scripts.
	Dictionary render_text_metrics(const String &text, const String &font_family, double font_size,
			int32_t width, int32_t height) const;
};

} // namespace godot

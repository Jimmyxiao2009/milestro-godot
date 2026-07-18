#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

// Forward declaration
namespace milestro::skia::textlayout {
	class StrutStyle;
}

namespace godot {

/// StrutStyle wrapper for Milestro Skia text layout.
class MilestroStrutStyle : public RefCounted {
	GDCLASS(MilestroStrutStyle, RefCounted)

protected:
	static void _bind_methods();

	friend class MilestroParagraphStyle;

	milestro::skia::textlayout::StrutStyle* get_native() const { return native_style; }

public:
	MilestroStrutStyle();
	~MilestroStrutStyle() override;

	/// Set font families.
	void set_font_families(const PackedStringArray& families);

	/// Set font style.
	void set_font_style(int32_t weight, int32_t width, int32_t slant);

	/// Set font size.
	void set_font_size(float size);

	/// Set strut height.
	void set_height(float height);

	/// Set leading.
	void set_leading(float leading);

	/// Enable/disable strut.
	void set_strut_enabled(bool enabled);

	/// Force strut height.
	void set_force_strut_height(bool enabled);

	/// Enable/disable height override.
	void set_height_override(bool enabled);

	/// Enable/disable half leading.
	void set_half_leading(bool enabled);

private:
	milestro::skia::textlayout::StrutStyle* native_style = nullptr;
};

} // namespace godot

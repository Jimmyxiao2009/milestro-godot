#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

// Forward declaration for Milestro C API
namespace milestro::skia::textlayout {
	class TextStyle;
}

namespace godot {

/// TextStyle wrapper for Milestro Skia text layout.
class MilestroTextStyle : public RefCounted {
	GDCLASS(MilestroTextStyle, RefCounted)

protected:
	static void _bind_methods();

	friend class MilestroParagraphStyle;
	friend class MilestroParagraphBuilder;
	friend class MilestroParagraph;
	friend class MilestroInputBox;
	friend class MilestroStrutStyle;

	milestro::skia::textlayout::TextStyle* get_native() const { return native_style; }

public:
	MilestroTextStyle();
	~MilestroTextStyle() override;

	/// Set text color (RGBA, 0-255).
	void set_color(int32_t r, int32_t g, int32_t b, int32_t a);

	/// Set font size in points.
	void set_font_size(double size);

	/// Set font families by name.
	void set_font_families(const PackedStringArray& families);

	/// Set font style (weight, width, slant).
	void set_font_style(int32_t weight, int32_t width, int32_t slant);

	/// Add a text shadow.
	void add_shadow(int32_t color_r, int32_t color_g, int32_t color_b, int32_t color_a,
			float offset_x, float offset_y, double blur_sigma);

	/// Reset (clear) all shadows.
	void reset_shadow();

	/// Set baseline shift.
	void set_baseline_shift(float shift);

	/// Set letter spacing.
	void set_letter_spacing(float spacing);

	/// Set word spacing.
	void set_word_spacing(float spacing);

	/// Set line height multiplier.
	void set_height(float height);

	/// Enable/disable height override.
	void set_height_override(bool enabled);

	/// Enable/disable half leading.
	void set_half_leading(bool enabled);

	/// Set text decoration (underline, overline, line-through).
	void set_decoration(int32_t decoration);

	/// Set decoration mode.
	void set_decoration_mode(int32_t mode);

	/// Set decoration color.
	void set_decoration_color(int32_t r, int32_t g, int32_t b, int32_t a);

	/// Set decoration style.
	void set_decoration_style(int32_t style);

	/// Set decoration thickness multiplier.
	void set_decoration_thickness_multiplier(float multiplier);

	/// Add a font feature (e.g., "liga", "kern").
	void add_font_feature(const String& feature, int32_t value);

	/// Reset all font features.
	void reset_font_features();

	/// Set locale (e.g., "zh-CN", "en-US").
	void set_locale(const String& locale);

	/// Set text baseline (alphabetic, ideographic, etc.).
	void set_text_baseline(int32_t baseline);

	/// Mark this style as a placeholder.
	void set_placeholder();

private:
	milestro::skia::textlayout::TextStyle* native_style = nullptr;
};

} // namespace godot

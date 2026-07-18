#pragma once

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

// Forward declarations for new classes
class MilestroFont;
class MilestroTextStyle;
class MilestroParagraphStyle;
class MilestroStrutStyle;
class MilestroParagraphBuilder;
class MilestroParagraph;
class MilestroCanvas;
class MilestroImage;
class MilestroPath;
class MilestroVertexData;
class MilestroSvg;
class MilestroTextDrawSnapshot;
class MilestroReusableTextDrawSnapshot;
class MilestroInputBox;
class MilestroInputBoxDrawSnapshot;
class MilestroFontFamilyInfo;
class MilestroFontFaceInfo;

/// Main entry point for Milestro GDExtension.
/// Provides factory methods for all Milestro objects and backward-compatible render methods.
class Milestro : public RefCounted {
	GDCLASS(Milestro, RefCounted)

protected:
	static void _bind_methods();

public:
	Milestro() = default;
	~Milestro() override = default;

	// --- Version ---
	Dictionary get_version() const;
	bool is_icu_loaded() const;
	bool set_font_fallback_enabled(bool enabled) const;
	bool is_font_fallback_enabled() const;
	bool clear_font_caches() const;
	String to_upper(const String &locale, const String &text) const;
	String to_lower(const String &locale, const String &text) const;

	// --- Font Registry ---
	int64_t register_font_from_file(const String &path) const;
	TypedArray<MilestroFontFamilyInfo> get_registered_font_families() const;
	TypedArray<MilestroFontFaceInfo> get_registered_font_faces() const;

	// --- Factory Methods ---
	Ref<MilestroFont> create_font(const String& family, int32_t weight, float size) const;
	Ref<MilestroTextStyle> create_text_style() const;
	Ref<MilestroParagraphStyle> create_paragraph_style() const;
	Ref<MilestroStrutStyle> create_strut_style() const;
	Ref<MilestroParagraphBuilder> create_paragraph_builder(Ref<MilestroParagraphStyle> style) const;
	Ref<MilestroCanvas> create_canvas(int32_t width, int32_t height) const;
	Ref<MilestroImage> create_image_from_file(const String& path) const;
	Ref<MilestroSvg> create_svg_from_file(const String& path) const;
	Ref<MilestroTextDrawSnapshot> create_text_draw_snapshot(Ref<MilestroFont> font, const String& text, int32_t r, int32_t g, int32_t b, int32_t a) const;
	Ref<MilestroReusableTextDrawSnapshot> create_reusable_text_draw_snapshot(Ref<MilestroFont> font, int64_t capacity, int32_t r, int32_t g, int32_t b, int32_t a) const;
	Ref<MilestroInputBox> create_input_box(Ref<MilestroParagraphStyle> para_style, Ref<MilestroTextStyle> text_style) const;

	// --- Backward-compatible render methods ---
	Ref<Image> render_text(const String &text, const String &font_family, double font_size,
			int32_t width, int32_t height) const;

	Dictionary render_text_metrics(const String &text, const String &font_family, double font_size,
			int32_t width, int32_t height) const;

	Dictionary render_mixed(const String &text, const PackedStringArray &font_families, double font_size,
			int32_t width, int32_t height, const PackedStringArray &image_paths,
			const PackedVector2Array &image_positions, const PackedFloat32Array &image_heights) const;
};

} // namespace godot

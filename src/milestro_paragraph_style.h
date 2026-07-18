#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::skia::textlayout {
	class ParagraphStyle;
}

namespace godot {

class MilestroTextStyle;
class MilestroStrutStyle;

/// ParagraphStyle wrapper for Milestro Skia text layout.
class MilestroParagraphStyle : public RefCounted {
	GDCLASS(MilestroParagraphStyle, RefCounted)

protected:
	static void _bind_methods();

	friend class MilestroParagraphBuilder;
	friend class MilestroParagraph;
	friend class MilestroInputBox;

	milestro::skia::textlayout::ParagraphStyle* get_native() const { return native_style; }

public:
	MilestroParagraphStyle();
	~MilestroParagraphStyle() override;

	/// Set the text style for this paragraph.
	void set_text_style(Ref<MilestroTextStyle> style);

	/// Set the strut style.
	void set_strut_style(Ref<MilestroStrutStyle> style);

	/// Set text direction (0 = ltr, 1 = rtl).
	void set_text_direction(int32_t direction);

	/// Set text alignment (0 = left, 1 = right, 2 = center, 3 = justify, 4 = start, 5 = end).
	void set_text_align(int32_t align);

	/// Set maximum number of lines.
	void set_max_lines(int64_t lines);

	/// Clear max lines limit.
	void clear_max_lines();

	/// Set ellipsis text (e.g., "...").
	void set_ellipsis(const String& text);

	/// Set line height multiplier.
	void set_height(float height);

	/// Set text height behavior.
	void set_text_height_behavior(int32_t behavior);

	/// Enable/disable tab character replacement.
	void set_replace_tab_characters(bool enabled);

	/// Enable/disable rounding hack.
	void set_apply_rounding_hack(bool enabled);

private:
	milestro::skia::textlayout::ParagraphStyle* native_style = nullptr;
};

} // namespace godot

#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::skia::textlayout {
	class InputBox;
	class InputBoxDrawSnapshot;
}

namespace godot {

class MilestroTextStyle;
class MilestroParagraphStyle;
class MilestroCanvas;
class MilestroInputBoxDrawSnapshot;

/// InputBox wrapper for Milestro Skia text layout.
class MilestroInputBox : public RefCounted {
	GDCLASS(MilestroInputBox, RefCounted)

protected:
	static void _bind_methods();

	friend class MilestroInputBoxDrawSnapshot;

	milestro::skia::textlayout::InputBox* get_native() const { return native_input_box; }

public:
	MilestroInputBox();
	~MilestroInputBox() override;

	/// Initialize with paragraph style and text style.
	bool init(Ref<MilestroParagraphStyle> para_style, Ref<MilestroTextStyle> text_style);

	// --- Text content ---
	void set_text(const String& text);
	String get_text() const;

	// --- Viewport ---
	void set_viewport(float width, float height);

	// --- Soft wrap ---
	void set_soft_wrap(bool enabled);
	bool get_soft_wrap() const;

	// --- Focus ---
	void set_focused(bool focused);
	bool get_focused() const;

	// --- Text overflow ---
	void set_text_overflow(int32_t overflow);
	int32_t get_text_overflow() const;

	// --- Ellipsis ---
	void set_ellipsis(const String& text);

	// --- Mask input ---
	void set_mask_input(bool enabled);
	bool get_mask_input() const;

	// --- Mask character ---
	void set_mask_char(const String& text);

	// --- Caret ---
	void set_caret_color(int32_t r, int32_t g, int32_t b, int32_t a);
	void set_selection_color(int32_t r, int32_t g, int32_t b, int32_t a);
	void set_caret_width(float width);
	void set_caret_visible(bool visible);

	// --- Auto margin ---
	void set_auto_margin(bool left, bool top, bool right, bool bottom);
	Dictionary get_auto_margin() const;

	// --- Text editing ---
	bool insert_text(const String& text);
	bool set_composition(const String& text);
	bool commit_composition(const String& text);
	bool clear_composition();
	bool delete_backward();
	bool delete_forward();
	bool undo();
	bool redo();
	bool break_undo_group();

	// --- Cursor movement ---
	bool move_previous();
	bool move_next();
	bool move_previous_extending_selection(bool extend);
	bool move_next_extending_selection(bool extend);
	bool move_up_extending_selection(bool extend);
	bool move_down_extending_selection(bool extend);
	bool move_line_start_extending_selection(bool extend);
	bool move_line_end_extending_selection(bool extend);
	bool move_document_start_extending_selection(bool extend);
	bool move_document_end_extending_selection(bool extend);

	// --- Hit testing ---
	bool hit_test(float x, float y);
	bool hit_test_extending_selection(float x, float y, bool extend);
	bool ensure_caret_visible();
	bool scroll_by_x(float delta);
	bool scroll_by_y(float delta);

	// --- Cursor / Selection ---
	Dictionary get_cursor() const;
	bool set_cursor_utf8(int64_t offset, int32_t affinity);
	Dictionary get_selection() const;
	String get_selected_text() const;
	bool set_selection_utf8(int64_t anchor, int64_t focus, int32_t anchor_affinity, int32_t focus_affinity);
	bool clear_selection();
	bool select_all();

	// --- Conversion ---
	int64_t utf8_to_utf16(int64_t offset) const;
	int64_t utf16_to_utf8(int64_t offset) const;
	int64_t snap_utf8(int64_t offset, int32_t mode) const;

	// --- Geometry ---
	Dictionary get_caret_rect() const;
	Dictionary get_composition_rect() const;
	Dictionary get_metrics() const;
	int64_t get_line_count() const;
	Dictionary get_line_metrics(int64_t line_number) const;

	// --- Draw snapshot ---
	Ref<MilestroInputBoxDrawSnapshot> create_draw_snapshot();

private:
	milestro::skia::textlayout::InputBox* native_input_box = nullptr;
};

} // namespace godot

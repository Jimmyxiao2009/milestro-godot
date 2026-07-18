#include "milestro_input_box.h"

#include "milestro_text_style.h"
#include "milestro_paragraph_style.h"
#include "milestro_canvas.h"
#include "milestro_input_box_draw_snapshot.h"
#include <Milestro/game/milestro_game_interface.h>
#include <Milestro/game/milestro_game_model.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

namespace {

String take_bytes(milestro::game::model::BytesWrapper *wrapper) {
	if (!wrapper) return String();
	uint8_t *ptr = nullptr;
	uint64_t size = 0;
	const int64_t rc = MilestroGameModelBytesWrapperCStr(wrapper, ptr, size);
	String result;
	if (rc == 0 && ptr && size > 0) {
		result = String::utf8(reinterpret_cast<const char *>(ptr), static_cast<int>(size));
	}
	milestro::game::model::DataEnvelop *envelop = wrapper;
	MilestroGameModelDataEnvelopDestroy(envelop);
	return result;
}

} // namespace

MilestroInputBox::MilestroInputBox() = default;

MilestroInputBox::~MilestroInputBox() {
	if (native_input_box) {
		MilestroSkiaTextlayoutInputBoxDestroy(native_input_box);
		native_input_box = nullptr;
	}
}

void MilestroInputBox::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init", "para_style", "text_style"), &MilestroInputBox::init);
	ClassDB::bind_method(D_METHOD("set_text", "text"), &MilestroInputBox::set_text);
	ClassDB::bind_method(D_METHOD("get_text"), &MilestroInputBox::get_text);
	ClassDB::bind_method(D_METHOD("set_viewport", "width", "height"), &MilestroInputBox::set_viewport);
	ClassDB::bind_method(D_METHOD("set_soft_wrap", "enabled"), &MilestroInputBox::set_soft_wrap);
	ClassDB::bind_method(D_METHOD("get_soft_wrap"), &MilestroInputBox::get_soft_wrap);
	ClassDB::bind_method(D_METHOD("set_focused", "focused"), &MilestroInputBox::set_focused);
	ClassDB::bind_method(D_METHOD("get_focused"), &MilestroInputBox::get_focused);
	ClassDB::bind_method(D_METHOD("set_text_overflow", "overflow"), &MilestroInputBox::set_text_overflow);
	ClassDB::bind_method(D_METHOD("get_text_overflow"), &MilestroInputBox::get_text_overflow);
	ClassDB::bind_method(D_METHOD("set_ellipsis", "text"), &MilestroInputBox::set_ellipsis);
	ClassDB::bind_method(D_METHOD("set_mask_input", "enabled"), &MilestroInputBox::set_mask_input);
	ClassDB::bind_method(D_METHOD("get_mask_input"), &MilestroInputBox::get_mask_input);
	ClassDB::bind_method(D_METHOD("set_mask_char", "text"), &MilestroInputBox::set_mask_char);
	ClassDB::bind_method(D_METHOD("set_caret_color", "r", "g", "b", "a"), &MilestroInputBox::set_caret_color);
	ClassDB::bind_method(D_METHOD("set_selection_color", "r", "g", "b", "a"), &MilestroInputBox::set_selection_color);
	ClassDB::bind_method(D_METHOD("set_caret_width", "width"), &MilestroInputBox::set_caret_width);
	ClassDB::bind_method(D_METHOD("set_caret_visible", "visible"), &MilestroInputBox::set_caret_visible);
	ClassDB::bind_method(D_METHOD("set_auto_margin", "left", "top", "right", "bottom"), &MilestroInputBox::set_auto_margin);
	ClassDB::bind_method(D_METHOD("get_auto_margin"), &MilestroInputBox::get_auto_margin);
	ClassDB::bind_method(D_METHOD("insert_text", "text"), &MilestroInputBox::insert_text);
	ClassDB::bind_method(D_METHOD("set_composition", "text"), &MilestroInputBox::set_composition);
	ClassDB::bind_method(D_METHOD("commit_composition", "text"), &MilestroInputBox::commit_composition);
	ClassDB::bind_method(D_METHOD("clear_composition"), &MilestroInputBox::clear_composition);
	ClassDB::bind_method(D_METHOD("delete_backward"), &MilestroInputBox::delete_backward);
	ClassDB::bind_method(D_METHOD("delete_forward"), &MilestroInputBox::delete_forward);
	ClassDB::bind_method(D_METHOD("undo"), &MilestroInputBox::undo);
	ClassDB::bind_method(D_METHOD("redo"), &MilestroInputBox::redo);
	ClassDB::bind_method(D_METHOD("break_undo_group"), &MilestroInputBox::break_undo_group);
	ClassDB::bind_method(D_METHOD("move_previous"), &MilestroInputBox::move_previous);
	ClassDB::bind_method(D_METHOD("move_next"), &MilestroInputBox::move_next);
	ClassDB::bind_method(D_METHOD("move_previous_extending_selection", "extend"), &MilestroInputBox::move_previous_extending_selection);
	ClassDB::bind_method(D_METHOD("move_next_extending_selection", "extend"), &MilestroInputBox::move_next_extending_selection);
	ClassDB::bind_method(D_METHOD("move_up_extending_selection", "extend"), &MilestroInputBox::move_up_extending_selection);
	ClassDB::bind_method(D_METHOD("move_down_extending_selection", "extend"), &MilestroInputBox::move_down_extending_selection);
	ClassDB::bind_method(D_METHOD("move_line_start_extending_selection", "extend"), &MilestroInputBox::move_line_start_extending_selection);
	ClassDB::bind_method(D_METHOD("move_line_end_extending_selection", "extend"), &MilestroInputBox::move_line_end_extending_selection);
	ClassDB::bind_method(D_METHOD("move_document_start_extending_selection", "extend"), &MilestroInputBox::move_document_start_extending_selection);
	ClassDB::bind_method(D_METHOD("move_document_end_extending_selection", "extend"), &MilestroInputBox::move_document_end_extending_selection);
	ClassDB::bind_method(D_METHOD("hit_test", "x", "y"), &MilestroInputBox::hit_test);
	ClassDB::bind_method(D_METHOD("hit_test_extending_selection", "x", "y", "extend"), &MilestroInputBox::hit_test_extending_selection);
	ClassDB::bind_method(D_METHOD("ensure_caret_visible"), &MilestroInputBox::ensure_caret_visible);
	ClassDB::bind_method(D_METHOD("scroll_by_x", "delta"), &MilestroInputBox::scroll_by_x);
	ClassDB::bind_method(D_METHOD("scroll_by_y", "delta"), &MilestroInputBox::scroll_by_y);
	ClassDB::bind_method(D_METHOD("get_cursor"), &MilestroInputBox::get_cursor);
	ClassDB::bind_method(D_METHOD("set_cursor_utf8", "offset", "affinity"), &MilestroInputBox::set_cursor_utf8);
	ClassDB::bind_method(D_METHOD("get_selection"), &MilestroInputBox::get_selection);
	ClassDB::bind_method(D_METHOD("get_selected_text"), &MilestroInputBox::get_selected_text);
	ClassDB::bind_method(D_METHOD("set_selection_utf8", "anchor", "focus", "anchor_affinity", "focus_affinity"), &MilestroInputBox::set_selection_utf8);
	ClassDB::bind_method(D_METHOD("clear_selection"), &MilestroInputBox::clear_selection);
	ClassDB::bind_method(D_METHOD("select_all"), &MilestroInputBox::select_all);
	ClassDB::bind_method(D_METHOD("utf8_to_utf16", "offset"), &MilestroInputBox::utf8_to_utf16);
	ClassDB::bind_method(D_METHOD("utf16_to_utf8", "offset"), &MilestroInputBox::utf16_to_utf8);
	ClassDB::bind_method(D_METHOD("snap_utf8", "offset", "mode"), &MilestroInputBox::snap_utf8);
	ClassDB::bind_method(D_METHOD("get_caret_rect"), &MilestroInputBox::get_caret_rect);
	ClassDB::bind_method(D_METHOD("get_composition_rect"), &MilestroInputBox::get_composition_rect);
	ClassDB::bind_method(D_METHOD("get_metrics"), &MilestroInputBox::get_metrics);
	ClassDB::bind_method(D_METHOD("get_line_count"), &MilestroInputBox::get_line_count);
	ClassDB::bind_method(D_METHOD("get_line_metrics", "line_number"), &MilestroInputBox::get_line_metrics);
	ClassDB::bind_method(D_METHOD("create_draw_snapshot"), &MilestroInputBox::create_draw_snapshot);
}

bool MilestroInputBox::init(Ref<MilestroParagraphStyle> para_style, Ref<MilestroTextStyle> text_style) {
	if (!para_style.is_valid() || !para_style->get_native() ||
		!text_style.is_valid() || !text_style->get_native()) return false;
	if (native_input_box) {
		MilestroSkiaTextlayoutInputBoxDestroy(native_input_box);
	}
	int64_t rc = MilestroSkiaTextlayoutInputBoxCreate(native_input_box, para_style->get_native(), text_style->get_native());
	return rc == 0 && native_input_box != nullptr;
}

void MilestroInputBox::set_text(const String& text) {
	if (!native_input_box) return;
	CharString cs = text.utf8();
	MilestroSkiaTextlayoutInputBoxSetText(native_input_box,
			const_cast<void*>(static_cast<const void*>(cs.get_data())),
			static_cast<uint64_t>(cs.length()));
}

String MilestroInputBox::get_text() const {
	if (!native_input_box) return String();
	milestro::game::model::BytesWrapper* wrapper = nullptr;
	int64_t rc = MilestroSkiaTextlayoutInputBoxGetText(native_input_box, wrapper);
	if (rc != 0 || !wrapper) return String();
	return take_bytes(wrapper);
}

void MilestroInputBox::set_viewport(float width, float height) {
	if (native_input_box) {
		MilestroSkiaTextlayoutInputBoxSetViewport(native_input_box, width, height);
	}
}

void MilestroInputBox::set_soft_wrap(bool enabled) {
	if (native_input_box) {
		MilestroSkiaTextlayoutInputBoxSetSoftWrap(native_input_box, enabled ? 1 : 0);
	}
}

bool MilestroInputBox::get_soft_wrap() const {
	if (!native_input_box) return false;
	int32_t enabled = 0;
	MilestroSkiaTextlayoutInputBoxGetSoftWrap(native_input_box, enabled);
	return enabled != 0;
}

void MilestroInputBox::set_focused(bool focused) {
	if (native_input_box) {
		MilestroSkiaTextlayoutInputBoxSetFocused(native_input_box, focused ? 1 : 0);
	}
}

bool MilestroInputBox::get_focused() const {
	if (!native_input_box) return false;
	int32_t focused = 0;
	MilestroSkiaTextlayoutInputBoxGetFocused(native_input_box, focused);
	return focused != 0;
}

void MilestroInputBox::set_text_overflow(int32_t overflow) {
	if (native_input_box) {
		MilestroSkiaTextlayoutInputBoxSetTextOverflow(native_input_box, overflow);
	}
}

int32_t MilestroInputBox::get_text_overflow() const {
	if (!native_input_box) return 0;
	int32_t overflow = 0;
	MilestroSkiaTextlayoutInputBoxGetTextOverflow(native_input_box, overflow);
	return overflow;
}

void MilestroInputBox::set_ellipsis(const String& text) {
	if (!native_input_box) return;
	CharString cs = text.utf8();
	MilestroSkiaTextlayoutInputBoxSetEllipsis(native_input_box,
			const_cast<void*>(static_cast<const void*>(cs.get_data())),
			static_cast<uint64_t>(cs.length()));
}

void MilestroInputBox::set_mask_input(bool enabled) {
	if (native_input_box) {
		MilestroSkiaTextlayoutInputBoxSetMaskInput(native_input_box, enabled ? 1 : 0);
	}
}

bool MilestroInputBox::get_mask_input() const {
	if (!native_input_box) return false;
	int32_t enabled = 0;
	MilestroSkiaTextlayoutInputBoxGetMaskInput(native_input_box, enabled);
	return enabled != 0;
}

void MilestroInputBox::set_mask_char(const String& text) {
	if (!native_input_box) return;
	CharString cs = text.utf8();
	MilestroSkiaTextlayoutInputBoxSetMaskChar(native_input_box,
			const_cast<void*>(static_cast<const void*>(cs.get_data())),
			static_cast<uint64_t>(cs.length()));
}

void MilestroInputBox::set_caret_color(int32_t r, int32_t g, int32_t b, int32_t a) {
	if (native_input_box) {
		MilestroSkiaTextlayoutInputBoxSetCaretColor(native_input_box, r, g, b, a);
	}
}

void MilestroInputBox::set_selection_color(int32_t r, int32_t g, int32_t b, int32_t a) {
	if (native_input_box) {
		MilestroSkiaTextlayoutInputBoxSetSelectionColor(native_input_box, r, g, b, a);
	}
}

void MilestroInputBox::set_caret_width(float width) {
	if (native_input_box) {
		MilestroSkiaTextlayoutInputBoxSetCaretWidth(native_input_box, width);
	}
}

void MilestroInputBox::set_caret_visible(bool visible) {
	if (native_input_box) {
		MilestroSkiaTextlayoutInputBoxSetCaretVisible(native_input_box, visible ? 1 : 0);
	}
}

void MilestroInputBox::set_auto_margin(bool left, bool top, bool right, bool bottom) {
	if (native_input_box) {
		MilestroSkiaTextlayoutInputBoxSetAutoMargin(native_input_box,
				left ? 1 : 0, top ? 1 : 0, right ? 1 : 0, bottom ? 1 : 0);
	}
}

Dictionary MilestroInputBox::get_auto_margin() const {
	Dictionary d;
	d["left"] = false;
	d["top"] = false;
	d["right"] = false;
	d["bottom"] = false;
	if (!native_input_box) return d;
	int32_t left = 0, top = 0, right = 0, bottom = 0;
	MilestroSkiaTextlayoutInputBoxGetAutoMargin(native_input_box, left, top, right, bottom);
	d["left"] = left != 0;
	d["top"] = top != 0;
	d["right"] = right != 0;
	d["bottom"] = bottom != 0;
	return d;
}

bool MilestroInputBox::insert_text(const String& text) {
	if (!native_input_box || text.is_empty()) return false;
	CharString cs = text.utf8();
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxInsertText(native_input_box,
			const_cast<void*>(static_cast<const void*>(cs.get_data())),
			static_cast<uint64_t>(cs.length()));
	return rc == 0;
}

bool MilestroInputBox::set_composition(const String& text) {
	if (!native_input_box || text.is_empty()) return false;
	CharString cs = text.utf8();
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxSetComposition(native_input_box,
			const_cast<void*>(static_cast<const void*>(cs.get_data())),
			static_cast<uint64_t>(cs.length()), changed);
	return rc == 0;
}

bool MilestroInputBox::commit_composition(const String& text) {
	if (!native_input_box || text.is_empty()) return false;
	CharString cs = text.utf8();
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxCommitComposition(native_input_box,
			const_cast<void*>(static_cast<const void*>(cs.get_data())),
			static_cast<uint64_t>(cs.length()), changed);
	return rc == 0;
}

bool MilestroInputBox::clear_composition() {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxClearComposition(native_input_box, changed);
	return rc == 0;
}

bool MilestroInputBox::delete_backward() {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxDeleteBackward(native_input_box, changed);
	return rc == 0;
}

bool MilestroInputBox::delete_forward() {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxDeleteForward(native_input_box, changed);
	return rc == 0;
}

bool MilestroInputBox::undo() {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxUndo(native_input_box, changed);
	return rc == 0;
}

bool MilestroInputBox::redo() {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxRedo(native_input_box, changed);
	return rc == 0;
}

bool MilestroInputBox::break_undo_group() {
	if (!native_input_box) return false;
	int64_t rc = MilestroSkiaTextlayoutInputBoxBreakUndoGroup(native_input_box);
	return rc == 0;
}

bool MilestroInputBox::move_previous() {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxMovePrevious(native_input_box, changed);
	return rc == 0;
}

bool MilestroInputBox::move_next() {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxMoveNext(native_input_box, changed);
	return rc == 0;
}

bool MilestroInputBox::move_previous_extending_selection(bool extend) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxMovePreviousExtendingSelection(native_input_box, extend ? 1 : 0, changed);
	return rc == 0;
}

bool MilestroInputBox::move_next_extending_selection(bool extend) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxMoveNextExtendingSelection(native_input_box, extend ? 1 : 0, changed);
	return rc == 0;
}

bool MilestroInputBox::move_up_extending_selection(bool extend) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxMoveUpExtendingSelection(native_input_box, extend ? 1 : 0, changed);
	return rc == 0;
}

bool MilestroInputBox::move_down_extending_selection(bool extend) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxMoveDownExtendingSelection(native_input_box, extend ? 1 : 0, changed);
	return rc == 0;
}

bool MilestroInputBox::move_line_start_extending_selection(bool extend) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxMoveLineStartExtendingSelection(native_input_box, extend ? 1 : 0, changed);
	return rc == 0;
}

bool MilestroInputBox::move_line_end_extending_selection(bool extend) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxMoveLineEndExtendingSelection(native_input_box, extend ? 1 : 0, changed);
	return rc == 0;
}

bool MilestroInputBox::move_document_start_extending_selection(bool extend) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxMoveDocumentStartExtendingSelection(native_input_box, extend ? 1 : 0, changed);
	return rc == 0;
}

bool MilestroInputBox::move_document_end_extending_selection(bool extend) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxMoveDocumentEndExtendingSelection(native_input_box, extend ? 1 : 0, changed);
	return rc == 0;
}

bool MilestroInputBox::hit_test(float x, float y) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxHitTest(native_input_box, x, y, changed);
	return rc == 0;
}

bool MilestroInputBox::hit_test_extending_selection(float x, float y, bool extend) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxHitTestExtendingSelection(native_input_box, x, y, extend ? 1 : 0, changed);
	return rc == 0;
}

bool MilestroInputBox::ensure_caret_visible() {
	if (!native_input_box) return false;
	int64_t rc = MilestroSkiaTextlayoutInputBoxEnsureCaretVisible(native_input_box);
	return rc == 0;
}

bool MilestroInputBox::scroll_by_x(float delta) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxScrollByX(native_input_box, delta, changed);
	return rc == 0;
}

bool MilestroInputBox::scroll_by_y(float delta) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxScrollByY(native_input_box, delta, changed);
	return rc == 0;
}

Dictionary MilestroInputBox::get_cursor() const {
	Dictionary d;
	d["utf8_offset"] = 0;
	d["utf16_offset"] = 0;
	d["affinity"] = 0;
	if (!native_input_box) return d;
	uint64_t utf8_offset = 0, utf16_offset = 0;
	int32_t affinity = 0;
	MilestroSkiaTextlayoutInputBoxGetCursor(native_input_box, utf8_offset, utf16_offset, affinity);
	d["utf8_offset"] = static_cast<int64_t>(utf8_offset);
	d["utf16_offset"] = static_cast<int64_t>(utf16_offset);
	d["affinity"] = affinity;
	return d;
}

bool MilestroInputBox::set_cursor_utf8(int64_t offset, int32_t affinity) {
	if (!native_input_box) return false;
	int64_t rc = MilestroSkiaTextlayoutInputBoxSetCursorUtf8(native_input_box, static_cast<uint64_t>(offset), affinity);
	return rc == 0;
}

Dictionary MilestroInputBox::get_selection() const {
	Dictionary d;
	d["anchor_utf8"] = 0;
	d["focus_utf8"] = 0;
	d["start_utf8"] = 0;
	d["end_utf8"] = 0;
	d["anchor_affinity"] = 0;
	d["focus_affinity"] = 0;
	d["has_selection"] = false;
	if (!native_input_box) return d;
	uint64_t anchor = 0, focus = 0, start = 0, end = 0;
	int32_t anchor_affinity = 0, focus_affinity = 0, has_selection = 0;
	MilestroSkiaTextlayoutInputBoxGetSelection(native_input_box, anchor, focus, start, end,
			anchor_affinity, focus_affinity, has_selection);
	d["anchor_utf8"] = static_cast<int64_t>(anchor);
	d["focus_utf8"] = static_cast<int64_t>(focus);
	d["start_utf8"] = static_cast<int64_t>(start);
	d["end_utf8"] = static_cast<int64_t>(end);
	d["anchor_affinity"] = anchor_affinity;
	d["focus_affinity"] = focus_affinity;
	d["has_selection"] = has_selection != 0;
	return d;
}

String MilestroInputBox::get_selected_text() const {
	if (!native_input_box) return String();
	milestro::game::model::BytesWrapper *wrapper = nullptr;
	const int64_t rc = MilestroSkiaTextlayoutInputBoxGetSelectedText(native_input_box, wrapper);
	if (rc != 0 || !wrapper) return String();
	return take_bytes(wrapper);
}

bool MilestroInputBox::set_selection_utf8(int64_t anchor, int64_t focus, int32_t anchor_affinity, int32_t focus_affinity) {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxSetSelectionUtf8(native_input_box,
			static_cast<uint64_t>(anchor), static_cast<uint64_t>(focus),
			anchor_affinity, focus_affinity, changed);
	return rc == 0;
}

bool MilestroInputBox::clear_selection() {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxClearSelection(native_input_box, changed);
	return rc == 0;
}

bool MilestroInputBox::select_all() {
	if (!native_input_box) return false;
	int32_t changed = 0;
	int64_t rc = MilestroSkiaTextlayoutInputBoxSelectAll(native_input_box, changed);
	return rc == 0;
}

int64_t MilestroInputBox::utf8_to_utf16(int64_t offset) const {
	if (!native_input_box) return 0;
	uint64_t utf16_offset = 0;
	MilestroSkiaTextlayoutInputBoxUtf8ToUtf16(native_input_box, static_cast<uint64_t>(offset), utf16_offset);
	return static_cast<int64_t>(utf16_offset);
}

int64_t MilestroInputBox::utf16_to_utf8(int64_t offset) const {
	if (!native_input_box) return 0;
	uint64_t utf8_offset = 0;
	MilestroSkiaTextlayoutInputBoxUtf16ToUtf8(native_input_box, static_cast<uint64_t>(offset), utf8_offset);
	return static_cast<int64_t>(utf8_offset);
}

int64_t MilestroInputBox::snap_utf8(int64_t offset, int32_t mode) const {
	if (!native_input_box) return 0;
	uint64_t snapped = 0;
	MilestroSkiaTextlayoutInputBoxSnapUtf8(native_input_box, static_cast<uint64_t>(offset), mode, snapped);
	return static_cast<int64_t>(snapped);
}

Dictionary MilestroInputBox::get_caret_rect() const {
	Dictionary d;
	d["left"] = 0.0f;
	d["top"] = 0.0f;
	d["right"] = 0.0f;
	d["bottom"] = 0.0f;
	if (!native_input_box) return d;
	float left = 0.0f, top = 0.0f, right = 0.0f, bottom = 0.0f;
	MilestroSkiaTextlayoutInputBoxGetCaretRect(native_input_box, left, top, right, bottom);
	d["left"] = left;
	d["top"] = top;
	d["right"] = right;
	d["bottom"] = bottom;
	return d;
}

Dictionary MilestroInputBox::get_composition_rect() const {
	Dictionary d;
	d["left"] = 0.0f;
	d["top"] = 0.0f;
	d["right"] = 0.0f;
	d["bottom"] = 0.0f;
	if (!native_input_box) return d;
	float left = 0.0f, top = 0.0f, right = 0.0f, bottom = 0.0f;
	MilestroSkiaTextlayoutInputBoxGetCompositionRect(native_input_box, left, top, right, bottom);
	d["left"] = left;
	d["top"] = top;
	d["right"] = right;
	d["bottom"] = bottom;
	return d;
}

Dictionary MilestroInputBox::get_metrics() const {
	Dictionary d;
	d["height"] = 0.0f;
	d["longest_line"] = 0.0f;
	d["min_intrinsic_width"] = 0.0f;
	d["max_intrinsic_width"] = 0.0f;
	d["content_width"] = 0.0f;
	d["scroll_x"] = 0.0f;
	d["scroll_y"] = 0.0f;
	d["viewport_width"] = 0.0f;
	d["viewport_height"] = 0.0f;
	if (!native_input_box) return d;
	float height = 0.0f, longest_line = 0.0f, min_intrinsic_width = 0.0f, max_intrinsic_width = 0.0f;
	float content_width = 0.0f, scroll_x = 0.0f, scroll_y = 0.0f, viewport_width = 0.0f, viewport_height = 0.0f;
	MilestroSkiaTextlayoutInputBoxGetMetrics(native_input_box, height, longest_line, min_intrinsic_width,
			max_intrinsic_width, content_width, scroll_x, scroll_y, viewport_width, viewport_height);
	d["height"] = height;
	d["longest_line"] = longest_line;
	d["min_intrinsic_width"] = min_intrinsic_width;
	d["max_intrinsic_width"] = max_intrinsic_width;
	d["content_width"] = content_width;
	d["scroll_x"] = scroll_x;
	d["scroll_y"] = scroll_y;
	d["viewport_width"] = viewport_width;
	d["viewport_height"] = viewport_height;
	return d;
}

int64_t MilestroInputBox::get_line_count() const {
	if (!native_input_box) return 0;
	uint64_t count = 0;
	MilestroSkiaTextlayoutInputBoxGetLineCount(native_input_box, count);
	return static_cast<int64_t>(count);
}

Dictionary MilestroInputBox::get_line_metrics(int64_t line_number) const {
	Dictionary d;
	d["start_utf8"] = 0;
	d["end_utf8"] = 0;
	d["ascent"] = 0.0f;
	d["descent"] = 0.0f;
	d["unscaled_ascent"] = 0.0f;
	d["height"] = 0.0f;
	d["width"] = 0.0f;
	d["left"] = 0.0f;
	d["baseline"] = 0.0f;
	if (!native_input_box) return d;
	uint64_t start_utf8 = 0, end_utf8 = 0;
	float ascent = 0.0f, descent = 0.0f, unscaled_ascent = 0.0f, height = 0.0f, width = 0.0f, left = 0.0f, baseline = 0.0f;
	MilestroSkiaTextlayoutInputBoxGetLineMetrics(native_input_box, static_cast<uint64_t>(line_number),
			start_utf8, end_utf8, ascent, descent, unscaled_ascent, height, width, left, baseline);
	d["start_utf8"] = static_cast<int64_t>(start_utf8);
	d["end_utf8"] = static_cast<int64_t>(end_utf8);
	d["ascent"] = ascent;
	d["descent"] = descent;
	d["unscaled_ascent"] = unscaled_ascent;
	d["height"] = height;
	d["width"] = width;
	d["left"] = left;
	d["baseline"] = baseline;
	return d;
}

Ref<MilestroInputBoxDrawSnapshot> MilestroInputBox::create_draw_snapshot() {
	if (!native_input_box) return Ref<MilestroInputBoxDrawSnapshot>();
	milestro::skia::textlayout::InputBoxDrawSnapshot* snapshot = nullptr;
	int64_t rc = MilestroSkiaTextlayoutInputBoxCreateDrawSnapshot(native_input_box, snapshot);
	if (rc != 0 || !snapshot) return Ref<MilestroInputBoxDrawSnapshot>();
	Ref<MilestroInputBoxDrawSnapshot> result;
	result.instantiate();
	result->set_native(snapshot);
	return result;
}

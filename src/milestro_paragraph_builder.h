#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::skia::textlayout {
	class ParagraphBuilder;
}

namespace godot {

class MilestroParagraphStyle;
class MilestroTextStyle;
class MilestroParagraph;

/// ParagraphBuilder wrapper for Milestro Skia text layout.
class MilestroParagraphBuilder : public RefCounted {
	GDCLASS(MilestroParagraphBuilder, RefCounted)

protected:
	static void _bind_methods();

	friend class MilestroParagraph;

	milestro::skia::textlayout::ParagraphBuilder* get_native() const { return native_builder; }

public:
	MilestroParagraphBuilder();
	~MilestroParagraphBuilder() override;

	/// Initialize with a paragraph style. Must be called before other methods.
	bool init(Ref<MilestroParagraphStyle> style);

	/// Push a text style onto the stack.
	void push_style(Ref<MilestroTextStyle> style);

	/// Pop the last text style.
	void pop();

	/// Add text to the paragraph.
	void add_text(const String& text);

	/// Build the paragraph. Returns null on failure.
	Ref<MilestroParagraph> build();

private:
	milestro::skia::textlayout::ParagraphBuilder* native_builder = nullptr;
};

} // namespace godot

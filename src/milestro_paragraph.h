#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

// Forward declaration
namespace milestro::skia::textlayout {
	class Paragraph;
}

namespace godot {

class MilestroCanvas;
class MilestroPath;

/// Paragraph wrapper for Milestro Skia text layout.
class MilestroParagraph : public RefCounted {
	GDCLASS(MilestroParagraph, RefCounted)

protected:
	static void _bind_methods();

	friend class MilestroParagraphBuilder;

	milestro::skia::textlayout::Paragraph* get_native() const { return native_paragraph; }
	/// Set native paragraph (used by ParagraphBuilder).
	void set_native(milestro::skia::textlayout::Paragraph* p) { native_paragraph = p; }

public:
	MilestroParagraph();
	~MilestroParagraph() override;

	/// Layout the paragraph with the given width.
	bool layout(float width);

	/// Get the measured height after layout.
	float get_height() const;

	/// Get the longest line width.
	float get_longest_line() const;

	/// Get the maximum intrinsic width.
	float get_max_intrinsic_width() const;

	/// Paint the paragraph onto a canvas at position (x, y).
	bool paint(Ref<MilestroCanvas> canvas, float x, float y);

	/// Convert paragraph to SDF (signed distance field).
	PackedByteArray to_sdf(int32_t sdf_width, int32_t sdf_height, float sdf_scale, float x, float y);

	/// Convert paragraph to a path at position (x, y).
	Ref<MilestroPath> to_path(float x, float y);

private:
	milestro::skia::textlayout::Paragraph* native_paragraph = nullptr;
};

} // namespace godot

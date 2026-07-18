#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::unicode {
	class Segmenter;
}

namespace godot {

/// Unicode Segmenter wrapper for Milestro.
class MilestroUnicodeSegmenter : public RefCounted {
	GDCLASS(MilestroUnicodeSegmenter, RefCounted)

protected:
	static void _bind_methods();

	milestro::unicode::Segmenter* get_native() const { return native_segmenter; }

public:
	MilestroUnicodeSegmenter();
	~MilestroUnicodeSegmenter() override;

	/// Initialize with locale and text.
	bool init(const String& locale, const String& text);

	/// Get first boundary.
	int32_t first();

	/// Get next boundary.
	int32_t next();

	/// Get current boundary.
	int32_t current();

	/// Get previous boundary.
	int32_t previous();

	/// Get substring.
	String substring(int32_t start, int32_t len);

private:
	milestro::unicode::Segmenter* native_segmenter = nullptr;
};

} // namespace godot

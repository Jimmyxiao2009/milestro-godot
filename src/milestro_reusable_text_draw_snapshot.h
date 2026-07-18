#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/dictionary.hpp>

// Forward declaration
namespace milestro::skia {
	class SlimTextDrawSnapshot;
}

namespace godot {

class MilestroCanvas;
class MilestroFont;
class MilestroTextDrawSnapshot;

/// ReusableTextDrawSnapshot wrapper for Milestro Skia.
class MilestroReusableTextDrawSnapshot : public RefCounted {
	GDCLASS(MilestroReusableTextDrawSnapshot, RefCounted)

protected:
	static void _bind_methods();

	milestro::skia::SlimTextDrawSnapshot* get_native() const { return native_snapshot; }

public:
	MilestroReusableTextDrawSnapshot();
	~MilestroReusableTextDrawSnapshot() override;

	/// Create a reusable snapshot with font, capacity, and color.
	bool create(Ref<MilestroFont> font, int64_t capacity, int32_t r, int32_t g, int32_t b, int32_t a);

	/// Update the text content.
	bool update_text(const String& text);

	/// Copy text from another snapshot.
	bool copy_text_from(Ref<MilestroTextDrawSnapshot> source);

	/// Measure text and return bounds.
	Dictionary measure_text() const;

	/// Paint text onto canvas at position (x, y).
	void paint_text(Ref<MilestroCanvas> canvas, float x, float y);

private:
	milestro::skia::SlimTextDrawSnapshot* native_snapshot = nullptr;
};

} // namespace godot

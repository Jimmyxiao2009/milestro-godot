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

/// TextDrawSnapshot wrapper for Milestro Skia.
class MilestroTextDrawSnapshot : public RefCounted {
	GDCLASS(MilestroTextDrawSnapshot, RefCounted)

protected:
	static void _bind_methods();

	friend class MilestroReusableTextDrawSnapshot;

	milestro::skia::SlimTextDrawSnapshot* get_native() const { return native_snapshot; }

public:
	MilestroTextDrawSnapshot();
	~MilestroTextDrawSnapshot() override;

	/// Create a snapshot with font, text, and color.
	bool create(Ref<MilestroFont> font, const String& text, int32_t r, int32_t g, int32_t b, int32_t a);

	/// Measure text and return bounds.
	Dictionary measure_text() const;

	/// Paint text onto canvas at position (x, y).
	void paint_text(Ref<MilestroCanvas> canvas, float x, float y);

private:
	milestro::skia::SlimTextDrawSnapshot* native_snapshot = nullptr;
};

} // namespace godot

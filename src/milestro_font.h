#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::skia {
	class Font;
}

namespace godot {

class MilestroPath;
class Milestro;

/// Font wrapper for Milestro Skia.
class MilestroFont : public RefCounted {
	GDCLASS(MilestroFont, RefCounted)

protected:
	static void _bind_methods();

	friend class MilestroTextDrawSnapshot;
	friend class MilestroReusableTextDrawSnapshot;
	friend class Milestro;

	void set_native(milestro::skia::Font *font) { native_font = font; }

	milestro::skia::Font* get_native() const { return native_font; }

public:
	MilestroFont();
	~MilestroFont() override;

	/// Get the path for a specific glyph.
	Ref<MilestroPath> get_path(uint16_t glyph_id);

	/// Get font metrics (ascent, descent, leading).
	Dictionary get_metrics() const;

	/// Measure text and return bounds.
	Dictionary measure_text(const String& text);

private:
	milestro::skia::Font* native_font = nullptr;
};

} // namespace godot

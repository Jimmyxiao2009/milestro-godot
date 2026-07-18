#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::skia {
	class Svg;
}

namespace godot {

class MilestroCanvas;

/// SVG wrapper for Milestro Skia.
class MilestroSvg : public RefCounted {
	GDCLASS(MilestroSvg, RefCounted)

protected:
	static void _bind_methods();

	milestro::skia::Svg* get_native() const { return native_svg; }

public:
	MilestroSvg();
	~MilestroSvg() override;

	/// Load SVG from file.
	bool load_from_file(const String& path);

	/// Render SVG onto a canvas.
	void render(Ref<MilestroCanvas> canvas);

private:
	milestro::skia::Svg* native_svg = nullptr;
};

} // namespace godot

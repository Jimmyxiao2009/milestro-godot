#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>

// Forward declaration
namespace milestro::skia {
	class Path;
}

namespace godot {

class MilestroVertexData;

/// Path wrapper for Milestro Skia.
class MilestroPath : public RefCounted {
	GDCLASS(MilestroPath, RefCounted)

protected:
	static void _bind_methods();

	friend class MilestroFont;
	friend class MilestroParagraph;

	milestro::skia::Path* get_native() const { return native_path; }
	void set_native(milestro::skia::Path* path);

public:
	MilestroPath();
	~MilestroPath() override;

	/// Convert path to anti-aliased triangles.
	Ref<MilestroVertexData> to_aa_triangles(float tolerance);

private:
	milestro::skia::Path* native_path = nullptr;
};

} // namespace godot

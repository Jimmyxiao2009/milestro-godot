#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>

// Forward declaration
namespace milestro::skia::textlayout {
	class InputBoxDrawSnapshot;
}

namespace godot {

class MilestroCanvas;
class MilestroInputBox;

/// InputBoxDrawSnapshot wrapper for Milestro Skia text layout.
class MilestroInputBoxDrawSnapshot : public RefCounted {
	GDCLASS(MilestroInputBoxDrawSnapshot, RefCounted)

protected:
	static void _bind_methods();
	friend class MilestroInputBox;

	milestro::skia::textlayout::InputBoxDrawSnapshot* get_native() const { return native_snapshot; }
	void set_native(milestro::skia::textlayout::InputBoxDrawSnapshot* snapshot);

public:
	MilestroInputBoxDrawSnapshot();
	~MilestroInputBoxDrawSnapshot() override;

	bool paint(Ref<MilestroCanvas> canvas, float x, float y, float width, float height);

private:
	milestro::skia::textlayout::InputBoxDrawSnapshot* native_snapshot = nullptr;
};

} // namespace godot

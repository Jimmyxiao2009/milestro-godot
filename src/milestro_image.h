#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::skia {
	class Image;
}

namespace godot {

/// Image wrapper for Milestro Skia.
class MilestroImage : public RefCounted {
	GDCLASS(MilestroImage, RefCounted)

protected:
	static void _bind_methods();

	friend class MilestroCanvas;
	friend class MilestroParagraph;

	milestro::skia::Image* get_native() const { return native_image; }

public:
	MilestroImage();
	~MilestroImage() override;

	/// Load an image from file data.
	bool load_from_file(const String& path);

	/// Get image width.
	int32_t get_width() const;

	/// Get image height.
	int32_t get_height() const;

	/// Set color type.
	void set_color_type(int32_t type);

private:
	milestro::skia::Image* native_image = nullptr;
};

} // namespace godot

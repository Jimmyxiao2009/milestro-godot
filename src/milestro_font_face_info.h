#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::skia {
	class MilestroFontFaceInfo;
}

namespace godot {

/// FontFaceInfo wrapper for Milestro.
class MilestroFontFaceInfo : public RefCounted {
	GDCLASS(MilestroFontFaceInfo, RefCounted)

protected:
	static void _bind_methods();

	friend class Milestro;
	void set_values(const String &source_path_value,
			const String &family_name_value,
			int32_t face_index_value,
			int32_t instance_index_value,
			int32_t packed_index_value,
			int32_t weight_value,
			int32_t width_value,
			int32_t slant_value,
			bool fixed_pitch_value) {
		source_path = source_path_value;
		family_name = family_name_value;
		face_index = face_index_value;
		instance_index = instance_index_value;
		packed_index = packed_index_value;
		weight = weight_value;
		width = width_value;
		slant = slant_value;
		fixed_pitch = fixed_pitch_value;
	}

public:
	MilestroFontFaceInfo();
	~MilestroFontFaceInfo() override;

	String get_source_path() const;
	String get_family_name() const;
	int32_t get_face_index() const;
	int32_t get_instance_index() const;
	int32_t get_packed_index() const;
	int32_t get_weight() const;
	int32_t get_width() const;
	int32_t get_slant() const;
	bool get_fixed_pitch() const;

private:
	String source_path;
	String family_name;
	int32_t face_index = 0;
	int32_t instance_index = 0;
	int32_t packed_index = 0;
	int32_t weight = 0;
	int32_t width = 0;
	int32_t slant = 0;
	bool fixed_pitch = false;
};

} // namespace godot

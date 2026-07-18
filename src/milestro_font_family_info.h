#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::skia {
	class MilestroFontFamilyInfo;
}

namespace godot {

/// FontFamilyInfo wrapper for Milestro.
class MilestroFontFamilyInfo : public RefCounted {
	GDCLASS(MilestroFontFamilyInfo, RefCounted)

protected:
	static void _bind_methods();

	friend class Milestro;
	void set_name(const String &name) { family_name = name; }

public:
	MilestroFontFamilyInfo();
	~MilestroFontFamilyInfo() override;

	/// Get the family name.
	String get_name() const;

private:
	String family_name;
};

} // namespace godot

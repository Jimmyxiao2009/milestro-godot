#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::unicode {
	class StringComparator;
}

namespace godot {

/// String Comparator wrapper for Milestro.
class MilestroStringComparator : public RefCounted {
	GDCLASS(MilestroStringComparator, RefCounted)

protected:
	static void _bind_methods();

	milestro::unicode::StringComparator* get_native() const { return native_comparator; }

public:
	MilestroStringComparator();
	~MilestroStringComparator() override;

	/// Initialize with collation string.
	bool init(const String& collation);

	/// Compare two strings. Returns < 0, 0, or > 0.
	int32_t compare(const String& a, const String& b) const;

private:
	milestro::unicode::StringComparator* native_comparator = nullptr;
};

} // namespace godot

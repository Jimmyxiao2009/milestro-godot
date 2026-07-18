#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::unicode {
	class Transliterator;
}

namespace godot {

/// Unicode Transliterator wrapper for Milestro.
class MilestroUnicodeTransliterator : public RefCounted {
	GDCLASS(MilestroUnicodeTransliterator, RefCounted)

protected:
	static void _bind_methods();

	milestro::unicode::Transliterator* get_native() const { return native_transliterator; }

public:
	MilestroUnicodeTransliterator();
	~MilestroUnicodeTransliterator() override;

	/// Initialize with transliterator ID and direction.
	bool init(const String& id, int32_t direction);

	/// Transliterate text.
	String transliterate(const String& text) const;

private:
	milestro::unicode::Transliterator* native_transliterator = nullptr;
};

} // namespace godot

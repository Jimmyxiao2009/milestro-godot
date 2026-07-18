#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

// Forward declaration
namespace milestro::unicode {
	class Normalizer;
}

namespace godot {

/// Unicode Normalizer wrapper for Milestro.
class MilestroUnicodeNormalizer : public RefCounted {
	GDCLASS(MilestroUnicodeNormalizer, RefCounted)

protected:
	static void _bind_methods();

	milestro::unicode::Normalizer* get_native() const { return native_normalizer; }

public:
	MilestroUnicodeNormalizer();
	~MilestroUnicodeNormalizer() override;

	/// Initialize with normalization form name (e.g., "NFC", "NFD", "NFKC", "NFKD") and mode.
	bool init(const String& name, int32_t mode);

	/// Normalize text.
	String normalize(const String& text) const;

private:
	milestro::unicode::Normalizer* native_normalizer = nullptr;
};

} // namespace godot

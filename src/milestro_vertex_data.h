#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

// Forward declaration
namespace milestro::skia {
	class VertexData;
}

namespace godot {

/// VertexData wrapper for Milestro Skia.
class MilestroVertexData : public RefCounted {
	GDCLASS(MilestroVertexData, RefCounted)

protected:
	static void _bind_methods();

	friend class MilestroPath;

	milestro::skia::VertexData* get_native() const { return native_data; }
	void set_native(milestro::skia::VertexData* data);

public:
	MilestroVertexData();
	~MilestroVertexData() override;

	/// Get the number of vertices.
	int64_t get_vertex_count() const;

	/// Get the size of each vertex in bytes.
	int64_t get_vertex_size() const;

	/// Fill the vertex data into a PackedByteArray.
	PackedByteArray fill_data() const;

private:
	milestro::skia::VertexData* native_data = nullptr;
};

} // namespace godot

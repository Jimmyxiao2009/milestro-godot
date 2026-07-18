#include "milestro_path.h"

#include "milestro_vertex_data.h"
#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroPath::MilestroPath() = default;

MilestroPath::~MilestroPath() {
	if (native_path) {
		MilestroSkiaPathDestroy(native_path);
		native_path = nullptr;
	}
}

void MilestroPath::_bind_methods() {
	ClassDB::bind_method(D_METHOD("to_aa_triangles", "tolerance"), &MilestroPath::to_aa_triangles);
}

void MilestroPath::set_native(milestro::skia::Path* path) {
	native_path = path;
}

Ref<MilestroVertexData> MilestroPath::to_aa_triangles(float tolerance) {
	if (!native_path) return Ref<MilestroVertexData>();
	milestro::skia::VertexData* data = nullptr;
	int64_t rc = MilestroSkiaPathToAATriangles(native_path, data, tolerance);
	if (rc != 0 || !data) return Ref<MilestroVertexData>();
	Ref<MilestroVertexData> result;
	result.instantiate();
	result->set_native(data);
	return result;
}

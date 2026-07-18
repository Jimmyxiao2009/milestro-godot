#include "milestro_vertex_data.h"

#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroVertexData::MilestroVertexData() = default;

MilestroVertexData::~MilestroVertexData() {
	if (native_data) {
		MilestroSkiaVertexDataDestroy(native_data);
		native_data = nullptr;
	}
}

void MilestroVertexData::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_vertex_count"), &MilestroVertexData::get_vertex_count);
	ClassDB::bind_method(D_METHOD("get_vertex_size"), &MilestroVertexData::get_vertex_size);
	ClassDB::bind_method(D_METHOD("fill_data"), &MilestroVertexData::fill_data);
}

void MilestroVertexData::set_native(milestro::skia::VertexData* data) {
	native_data = data;
}

int64_t MilestroVertexData::get_vertex_count() const {
	if (!native_data) return 0;
	uint64_t count = 0;
	MilestroSkiaVertexDataGetVertexCount(native_data, count);
	return static_cast<int64_t>(count);
}

int64_t MilestroVertexData::get_vertex_size() const {
	if (!native_data) return 0;
	uint64_t size = 0;
	MilestroSkiaVertexDataGetVertexSize(native_data, size);
	return static_cast<int64_t>(size);
}

PackedByteArray MilestroVertexData::fill_data() const {
	if (!native_data) return PackedByteArray();
	uint64_t count = 0;
	uint64_t size = 0;
	MilestroSkiaVertexDataGetVertexCount(native_data, count);
	MilestroSkiaVertexDataGetVertexSize(native_data, size);
	PackedByteArray result;
	result.resize(static_cast<int64_t>(count * size));
	MilestroSkiaVertexDataFillData(native_data, result.ptrw());
	return result;
}

#include "milestro_font_face_info.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroFontFaceInfo::MilestroFontFaceInfo() = default;

MilestroFontFaceInfo::~MilestroFontFaceInfo() = default;

void MilestroFontFaceInfo::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_source_path"), &MilestroFontFaceInfo::get_source_path);
	ClassDB::bind_method(D_METHOD("get_family_name"), &MilestroFontFaceInfo::get_family_name);
	ClassDB::bind_method(D_METHOD("get_face_index"), &MilestroFontFaceInfo::get_face_index);
	ClassDB::bind_method(D_METHOD("get_instance_index"), &MilestroFontFaceInfo::get_instance_index);
	ClassDB::bind_method(D_METHOD("get_packed_index"), &MilestroFontFaceInfo::get_packed_index);
	ClassDB::bind_method(D_METHOD("get_weight"), &MilestroFontFaceInfo::get_weight);
	ClassDB::bind_method(D_METHOD("get_width"), &MilestroFontFaceInfo::get_width);
	ClassDB::bind_method(D_METHOD("get_slant"), &MilestroFontFaceInfo::get_slant);
	ClassDB::bind_method(D_METHOD("get_fixed_pitch"), &MilestroFontFaceInfo::get_fixed_pitch);
}

String MilestroFontFaceInfo::get_source_path() const { return source_path; }
String MilestroFontFaceInfo::get_family_name() const { return family_name; }
int32_t MilestroFontFaceInfo::get_face_index() const { return face_index; }
int32_t MilestroFontFaceInfo::get_instance_index() const { return instance_index; }
int32_t MilestroFontFaceInfo::get_packed_index() const { return packed_index; }
int32_t MilestroFontFaceInfo::get_weight() const { return weight; }
int32_t MilestroFontFaceInfo::get_width() const { return width; }
int32_t MilestroFontFaceInfo::get_slant() const { return slant; }
bool MilestroFontFaceInfo::get_fixed_pitch() const { return fixed_pitch; }

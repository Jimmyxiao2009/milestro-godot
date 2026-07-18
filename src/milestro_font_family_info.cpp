#include "milestro_font_family_info.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroFontFamilyInfo::MilestroFontFamilyInfo() = default;

MilestroFontFamilyInfo::~MilestroFontFamilyInfo() = default;

void MilestroFontFamilyInfo::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_name"), &MilestroFontFamilyInfo::get_name);
}

String MilestroFontFamilyInfo::get_name() const {
	return family_name;
}

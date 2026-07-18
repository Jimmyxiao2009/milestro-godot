#include "milestro_image.h"

#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <fstream>
#include <vector>

using namespace godot;

MilestroImage::MilestroImage() = default;

MilestroImage::~MilestroImage() {
	if (native_image) {
		MilestroSkiaImageDestroy(native_image);
		native_image = nullptr;
	}
}

void MilestroImage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load_from_file", "path"), &MilestroImage::load_from_file);
	ClassDB::bind_method(D_METHOD("get_width"), &MilestroImage::get_width);
	ClassDB::bind_method(D_METHOD("get_height"), &MilestroImage::get_height);
	ClassDB::bind_method(D_METHOD("set_color_type", "type"), &MilestroImage::set_color_type);
}

bool MilestroImage::load_from_file(const String& path) {
	if (path.is_empty()) return false;

	// Read file into buffer
	std::ifstream f(path.utf8().get_data(), std::ios::binary);
	if (!f) {
		UtilityFunctions::push_warning("MilestroImage: failed to open file: " + path);
		return false;
	}
	f.seekg(0, std::ios::end);
	std::streamoff size = f.tellg();
	if (size <= 0) {
		UtilityFunctions::push_warning("MilestroImage: empty file: " + path);
		return false;
	}
	f.seekg(0, std::ios::beg);
	std::vector<uint8_t> buffer(static_cast<size_t>(size));
	f.read(reinterpret_cast<char*>(buffer.data()), size);
	if (!f) {
		UtilityFunctions::push_warning("MilestroImage: failed to read file: " + path);
		return false;
	}

	// Destroy existing image
	if (native_image) {
		MilestroSkiaImageDestroy(native_image);
		native_image = nullptr;
	}

	int64_t rc = MilestroSkiaImageCreate(native_image, buffer.data(), static_cast<uint64_t>(buffer.size()));
	return rc == 0 && native_image != nullptr;
}

int32_t MilestroImage::get_width() const {
	if (!native_image) return 0;
	int32_t w = 0;
	MilestroSkiaImageGetWidth(native_image, w);
	return w;
}

int32_t MilestroImage::get_height() const {
	if (!native_image) return 0;
	int32_t h = 0;
	MilestroSkiaImageGetHeight(native_image, h);
	return h;
}

void MilestroImage::set_color_type(int32_t type) {
	if (native_image) {
		MilestroSkiaImageSetColorType(native_image, type);
	}
}

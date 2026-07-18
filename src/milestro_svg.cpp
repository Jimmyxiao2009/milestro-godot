#include "milestro_svg.h"

#include "milestro_canvas.h"
#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <fstream>
#include <vector>

using namespace godot;

MilestroSvg::MilestroSvg() = default;

MilestroSvg::~MilestroSvg() {
	if (native_svg) {
		MilestroSkiaSvgDestroy(native_svg);
		native_svg = nullptr;
	}
}

void MilestroSvg::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load_from_file", "path"), &MilestroSvg::load_from_file);
	ClassDB::bind_method(D_METHOD("render", "canvas"), &MilestroSvg::render);
}

bool MilestroSvg::load_from_file(const String& path) {
	if (path.is_empty()) return false;

	std::ifstream f(path.utf8().get_data(), std::ios::binary);
	if (!f) {
		UtilityFunctions::push_warning("MilestroSvg: failed to open file: " + path);
		return false;
	}
	f.seekg(0, std::ios::end);
	std::streamoff size = f.tellg();
	if (size <= 0) {
		UtilityFunctions::push_warning("MilestroSvg: empty file: " + path);
		return false;
	}
	f.seekg(0, std::ios::beg);
	std::vector<uint8_t> buffer(static_cast<size_t>(size));
	f.read(reinterpret_cast<char*>(buffer.data()), size);
	if (!f) {
		UtilityFunctions::push_warning("MilestroSvg: failed to read file: " + path);
		return false;
	}

	if (native_svg) {
		MilestroSkiaSvgDestroy(native_svg);
		native_svg = nullptr;
	}

	int64_t rc = MilestroSkiaSvgCreate(native_svg, buffer.data(), static_cast<uint64_t>(buffer.size()));
	return rc == 0 && native_svg != nullptr;
}

void MilestroSvg::render(Ref<MilestroCanvas> canvas) {
	if (!native_svg || !canvas.is_valid() || !canvas->get_native()) return;
	MilestroSkiaSvgRender(native_svg, canvas->get_native());
}

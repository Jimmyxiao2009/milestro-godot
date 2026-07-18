#include "register_types.h"

#include "milestro_binding.h"
#include "milestro_font.h"
#include "milestro_text_style.h"
#include "milestro_paragraph_style.h"
#include "milestro_strut_style.h"
#include "milestro_paragraph_builder.h"
#include "milestro_paragraph.h"
#include "milestro_canvas.h"
#include "milestro_image.h"
#include "milestro_path.h"
#include "milestro_vertex_data.h"
#include "milestro_svg.h"
#include "milestro_text_draw_snapshot.h"
#include "milestro_reusable_text_draw_snapshot.h"
#include "milestro_input_box.h"
#include "milestro_input_box_draw_snapshot.h"
#include "milestro_font_family_info.h"
#include "milestro_font_face_info.h"
#include "milestro_unicode_normalizer.h"
#include "milestro_unicode_segmenter.h"
#include "milestro_unicode_transliterator.h"
#include "milestro_string_comparator.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_milestro_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<Milestro>();
	ClassDB::register_class<MilestroFont>();
	ClassDB::register_class<MilestroTextStyle>();
	ClassDB::register_class<MilestroParagraphStyle>();
	ClassDB::register_class<MilestroStrutStyle>();
	ClassDB::register_class<MilestroParagraphBuilder>();
	ClassDB::register_class<MilestroParagraph>();
	ClassDB::register_class<MilestroCanvas>();
	ClassDB::register_class<MilestroImage>();
	ClassDB::register_class<MilestroPath>();
	ClassDB::register_class<MilestroVertexData>();
	ClassDB::register_class<MilestroSvg>();
	ClassDB::register_class<MilestroTextDrawSnapshot>();
	ClassDB::register_class<MilestroReusableTextDrawSnapshot>();
	ClassDB::register_class<MilestroInputBox>();
	ClassDB::register_class<MilestroInputBoxDrawSnapshot>();
	ClassDB::register_class<MilestroFontFamilyInfo>();
	ClassDB::register_class<MilestroFontFaceInfo>();
	ClassDB::register_class<MilestroUnicodeNormalizer>();
	ClassDB::register_class<MilestroUnicodeSegmenter>();
	ClassDB::register_class<MilestroUnicodeTransliterator>();
	ClassDB::register_class<MilestroStringComparator>();
}

void uninitialize_milestro_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization entry point used by milestro.gdextension
GDExtensionBool GDE_EXPORT milestro_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
		const GDExtensionClassLibraryPtr p_library,
		GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_milestro_module);
	init_obj.register_terminator(uninitialize_milestro_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}

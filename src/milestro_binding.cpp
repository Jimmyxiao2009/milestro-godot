#include "milestro_binding.h"

#include "milestro_font.h"
#include "milestro_text_style.h"
#include "milestro_paragraph_style.h"
#include "milestro_strut_style.h"
#include "milestro_paragraph_builder.h"
#include "milestro_paragraph.h"
#include "milestro_canvas.h"
#include "milestro_image.h"
#include "milestro_svg.h"
#include "milestro_text_draw_snapshot.h"
#include "milestro_reusable_text_draw_snapshot.h"
#include "milestro_input_box.h"
#include "milestro_font_family_info.h"
#include "milestro_font_face_info.h"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include <Milestro/game/milestro_game_interface.h>
#include <Milestro/game/milestro_game_model.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

namespace {

constexpr int64_t kOk = 0;

std::string to_utf8(const String &s) {
	const CharString cs = s.utf8();
	return std::string(cs.get_data(), static_cast<size_t>(cs.length()));
}

bool read_file_bytes(const std::string &path, std::vector<uint8_t> &out) {
	std::ifstream f(path, std::ios::binary);
	if (!f) {
		return false;
	}
	f.seekg(0, std::ios::end);
	const std::streamoff n = f.tellg();
	if (n <= 0) {
		return false;
	}
	f.seekg(0, std::ios::beg);
	out.resize(static_cast<size_t>(n));
	f.read(reinterpret_cast<char *>(out.data()), n);
	return static_cast<bool>(f);
}

String bytes_to_string(milestro::game::model::BytesWrapper *wrapper) {
	if (!wrapper) return String();
	uint8_t *ptr = nullptr;
	uint64_t size = 0;
	const int64_t rc = MilestroGameModelBytesWrapperCStr(wrapper, ptr, size);
	String result;
	if (rc == kOk && ptr && size > 0) {
		result = String::utf8(reinterpret_cast<const char *>(ptr), static_cast<int>(size));
	}
	milestro::game::model::DataEnvelop *envelop = wrapper;
	MilestroGameModelDataEnvelopDestroy(envelop);
	return result;
}

String borrowed_utf8_to_string(uint8_t *ptr, uint64_t size) {
	if (!ptr || size == 0) return String();
	return String::utf8(reinterpret_cast<const char *>(ptr), static_cast<int>(size));
}

int count_non_zero(const std::vector<uint8_t> &pixels) {
	int non_zero = 0;
	for (uint8_t b : pixels) {
		if (b != 0) {
			++non_zero;
		}
	}
	return non_zero;
}

} // namespace

void Milestro::_bind_methods() {
	// Version
	ClassDB::bind_method(D_METHOD("get_version"), &Milestro::get_version);
	ClassDB::bind_method(D_METHOD("is_icu_loaded"), &Milestro::is_icu_loaded);
	ClassDB::bind_method(D_METHOD("set_font_fallback_enabled", "enabled"), &Milestro::set_font_fallback_enabled);
	ClassDB::bind_method(D_METHOD("is_font_fallback_enabled"), &Milestro::is_font_fallback_enabled);
	ClassDB::bind_method(D_METHOD("clear_font_caches"), &Milestro::clear_font_caches);
	ClassDB::bind_method(D_METHOD("to_upper", "locale", "text"), &Milestro::to_upper);
	ClassDB::bind_method(D_METHOD("to_lower", "locale", "text"), &Milestro::to_lower);

	// Font Registry
	ClassDB::bind_method(D_METHOD("register_font_from_file", "path"), &Milestro::register_font_from_file);
	ClassDB::bind_method(D_METHOD("get_registered_font_families"), &Milestro::get_registered_font_families);
	ClassDB::bind_method(D_METHOD("get_registered_font_faces"), &Milestro::get_registered_font_faces);

	// Factory Methods
	ClassDB::bind_method(D_METHOD("create_font", "family", "weight", "size"), &Milestro::create_font);
	ClassDB::bind_method(D_METHOD("create_text_style"), &Milestro::create_text_style);
	ClassDB::bind_method(D_METHOD("create_paragraph_style"), &Milestro::create_paragraph_style);
	ClassDB::bind_method(D_METHOD("create_strut_style"), &Milestro::create_strut_style);
	ClassDB::bind_method(D_METHOD("create_paragraph_builder", "style"), &Milestro::create_paragraph_builder);
	ClassDB::bind_method(D_METHOD("create_canvas", "width", "height"), &Milestro::create_canvas);
	ClassDB::bind_method(D_METHOD("create_image_from_file", "path"), &Milestro::create_image_from_file);
	ClassDB::bind_method(D_METHOD("create_svg_from_file", "path"), &Milestro::create_svg_from_file);
	ClassDB::bind_method(D_METHOD("create_text_draw_snapshot", "font", "text", "r", "g", "b", "a"), &Milestro::create_text_draw_snapshot);
	ClassDB::bind_method(D_METHOD("create_reusable_text_draw_snapshot", "font", "capacity", "r", "g", "b", "a"), &Milestro::create_reusable_text_draw_snapshot);
	ClassDB::bind_method(D_METHOD("create_input_box", "para_style", "text_style"), &Milestro::create_input_box);

	// Backward-compatible render methods
	ClassDB::bind_method(D_METHOD("render_text", "text", "font_family", "font_size", "width", "height"),
			&Milestro::render_text);
	ClassDB::bind_method(D_METHOD("render_text_metrics", "text", "font_family", "font_size", "width", "height"),
			&Milestro::render_text_metrics);
	ClassDB::bind_method(D_METHOD("render_mixed", "text", "font_families", "font_size", "width", "height",
									   "image_paths", "image_positions", "image_heights"),
			&Milestro::render_mixed);
}

Dictionary Milestro::get_version() const {
	int32_t major = 0;
	int32_t minor = 0;
	int32_t patch = 0;
	const int64_t rc = MilestroGetVersion(major, minor, patch);

	Dictionary d;
	d["ok"] = (major > 0 || minor > 0 || patch > 0) && (rc == static_cast<int64_t>(major));
	d["code"] = rc;
	d["major"] = major;
	d["minor"] = minor;
	d["patch"] = patch;
	return d;
}

bool Milestro::is_icu_loaded() const {
	int32_t loaded = 0;
	return MilestroIsICULoaded(loaded) == kOk && loaded != 0;
}

bool Milestro::set_font_fallback_enabled(bool enabled) const {
	return MilestroSkiaFontCollectionSetFontFallbackEnabled(enabled ? 1 : 0) == kOk;
}

bool Milestro::is_font_fallback_enabled() const {
	int32_t enabled = 0;
	return MilestroSkiaFontCollectionIsFontFallbackEnabled(enabled) == kOk && enabled != 0;
}

bool Milestro::clear_font_caches() const {
	return MilestroSkiaFontCollectionClearCaches() == kOk;
}

String Milestro::to_upper(const String &locale, const String &text) const {
	const std::string locale_utf8 = to_utf8(locale);
	const std::string text_utf8 = to_utf8(text);
	milestro::game::model::BytesWrapper *wrapper = nullptr;
	const int64_t rc = MilestroUnicodeCaseMapToUpper(wrapper,
			reinterpret_cast<uint8_t *>(const_cast<char *>(locale_utf8.data())),
			reinterpret_cast<uint8_t *>(const_cast<char *>(text_utf8.data())));
	return rc == kOk ? bytes_to_string(wrapper) : String();
}

String Milestro::to_lower(const String &locale, const String &text) const {
	const std::string locale_utf8 = to_utf8(locale);
	const std::string text_utf8 = to_utf8(text);
	milestro::game::model::BytesWrapper *wrapper = nullptr;
	const int64_t rc = MilestroUnicodeCaseMapToLower(wrapper,
			reinterpret_cast<uint8_t *>(const_cast<char *>(locale_utf8.data())),
			reinterpret_cast<uint8_t *>(const_cast<char *>(text_utf8.data())));
	return rc == kOk ? bytes_to_string(wrapper) : String();
}

int64_t Milestro::register_font_from_file(const String &path) const {
	const std::string p = to_utf8(path);
	return MilestroSkiaFontRegistryRegisterFontFromFile(
			reinterpret_cast<uint8_t *>(const_cast<char *>(p.data())),
			static_cast<uint64_t>(p.size()));
}

TypedArray<MilestroFontFamilyInfo> Milestro::get_registered_font_families() const {
	TypedArray<MilestroFontFamilyInfo> result;
	milestro::skia::MilestroFontFamilyList* list = nullptr;
	int64_t rc = MilestroSkiaFontRegistryGetRegisteredFontFamilyList(list);
	if (rc != kOk || !list) return result;

	uint64_t size = 0;
	MilestroSkiaFontFamilyListGetSize(list, size);
	for (uint64_t i = 0; i < size; ++i) {
		milestro::skia::MilestroFontFamilyInfo* info = nullptr;
		MilestroSkiaFontFamilyListRefElementAt(list, info, i);
		if (!info) continue;
		Ref<MilestroFontFamilyInfo> gd_info;
		gd_info.instantiate();
		uint8_t *name = nullptr;
		uint64_t name_size = 0;
		if (MilestroSkiaFontFamilyInfoGetName(info, name, name_size) == kOk) {
			gd_info->set_name(borrowed_utf8_to_string(name, name_size));
		}
		result.push_back(gd_info);
		MilestroSkiaFontFamilyInfoDestroy(info);
	}
	MilestroSkiaFontFamilyListDestroy(list);
	return result;
}

TypedArray<MilestroFontFaceInfo> Milestro::get_registered_font_faces() const {
	TypedArray<MilestroFontFaceInfo> result;
	milestro::skia::MilestroFontFaceList* list = nullptr;
	int64_t rc = MilestroSkiaFontRegistryGetRegisteredFontFaceList(list);
	if (rc != kOk || !list) return result;

	uint64_t size = 0;
	MilestroSkiaFontFaceListGetSize(list, size);
	for (uint64_t i = 0; i < size; ++i) {
		milestro::skia::MilestroFontFaceInfo* info = nullptr;
		MilestroSkiaFontFaceListRefElementAt(list, info, i);
		if (!info) continue;
		Ref<MilestroFontFaceInfo> gd_info;
		gd_info.instantiate();
		uint8_t *source_path = nullptr;
		uint64_t source_path_size = 0;
		uint8_t *family_name = nullptr;
		uint64_t family_name_size = 0;
		int32_t face_index = 0;
		int32_t instance_index = 0;
		int32_t packed_index = 0;
		int32_t weight = 0;
		int32_t width = 0;
		int32_t slant = 0;
		int32_t fixed_pitch = 0;
		MilestroSkiaFontFaceInfoGetSourcePath(info, source_path, source_path_size);
		MilestroSkiaFontFaceInfoGetFamilyName(info, family_name, family_name_size);
		MilestroSkiaFontFaceInfoGetFaceIndex(info, face_index);
		MilestroSkiaFontFaceInfoGetInstanceIndex(info, instance_index);
		MilestroSkiaFontFaceInfoGetPackedIndex(info, packed_index);
		MilestroSkiaFontFaceInfoGetWeight(info, weight);
		MilestroSkiaFontFaceInfoGetWidth(info, width);
		MilestroSkiaFontFaceInfoGetSlant(info, slant);
		MilestroSkiaFontFaceInfoGetFixedPitch(info, fixed_pitch);
		gd_info->set_values(borrowed_utf8_to_string(source_path, source_path_size),
				borrowed_utf8_to_string(family_name, family_name_size), face_index,
				instance_index, packed_index, weight, width, slant, fixed_pitch != 0);
		result.push_back(gd_info);
		MilestroSkiaFontFaceInfoDestroy(info);
	}
	MilestroSkiaFontFaceListDestroy(list);
	return result;
}

Ref<MilestroFont> Milestro::create_font(const String& family, int32_t weight, float size) const {
	Ref<MilestroFont> font;
	font.instantiate();
	const std::string family_utf8 = to_utf8(family);
	milestro::skia::Font *native_font = nullptr;
	const int64_t rc = MilestroSkiaFontRegistryResolveTypeface(native_font,
			reinterpret_cast<uint8_t *>(const_cast<char *>(family_utf8.data())),
			static_cast<uint64_t>(family_utf8.size()), weight, size, 1);
	if (rc != kOk || !native_font) return Ref<MilestroFont>();
	font->set_native(native_font);
	return font;
}

Ref<MilestroTextStyle> Milestro::create_text_style() const {
	Ref<MilestroTextStyle> style;
	style.instantiate();
	return style;
}

Ref<MilestroParagraphStyle> Milestro::create_paragraph_style() const {
	Ref<MilestroParagraphStyle> style;
	style.instantiate();
	return style;
}

Ref<MilestroStrutStyle> Milestro::create_strut_style() const {
	Ref<MilestroStrutStyle> style;
	style.instantiate();
	return style;
}

Ref<MilestroParagraphBuilder> Milestro::create_paragraph_builder(Ref<MilestroParagraphStyle> style) const {
	if (!style.is_valid()) return Ref<MilestroParagraphBuilder>();
	Ref<MilestroParagraphBuilder> builder;
	builder.instantiate();
	builder->init(style);
	return builder;
}

Ref<MilestroCanvas> Milestro::create_canvas(int32_t width, int32_t height) const {
	Ref<MilestroCanvas> canvas;
	canvas.instantiate();
	if (!canvas->create(width, height)) return Ref<MilestroCanvas>();
	return canvas;
}

Ref<MilestroImage> Milestro::create_image_from_file(const String& path) const {
	Ref<MilestroImage> image;
	image.instantiate();
	if (!image->load_from_file(path)) return Ref<MilestroImage>();
	return image;
}

Ref<MilestroSvg> Milestro::create_svg_from_file(const String& path) const {
	Ref<MilestroSvg> svg;
	svg.instantiate();
	if (!svg->load_from_file(path)) return Ref<MilestroSvg>();
	return svg;
}

Ref<MilestroTextDrawSnapshot> Milestro::create_text_draw_snapshot(Ref<MilestroFont> font, const String& text, int32_t r, int32_t g, int32_t b, int32_t a) const {
	Ref<MilestroTextDrawSnapshot> snapshot;
	snapshot.instantiate();
	if (!snapshot->create(font, text, r, g, b, a)) return Ref<MilestroTextDrawSnapshot>();
	return snapshot;
}

Ref<MilestroReusableTextDrawSnapshot> Milestro::create_reusable_text_draw_snapshot(Ref<MilestroFont> font, int64_t capacity, int32_t r, int32_t g, int32_t b, int32_t a) const {
	Ref<MilestroReusableTextDrawSnapshot> snapshot;
	snapshot.instantiate();
	if (!snapshot->create(font, capacity, r, g, b, a)) return Ref<MilestroReusableTextDrawSnapshot>();
	return snapshot;
}

Ref<MilestroInputBox> Milestro::create_input_box(Ref<MilestroParagraphStyle> para_style, Ref<MilestroTextStyle> text_style) const {
	Ref<MilestroInputBox> box;
	box.instantiate();
	if (!box->init(para_style, text_style)) return Ref<MilestroInputBox>();
	return box;
}

Ref<Image> Milestro::render_text(const String &text, const String &font_family, double font_size,
		int32_t width, int32_t height) const {
	const Dictionary m = render_text_metrics(text, font_family, font_size, width, height);
	if (!bool(m.get("ok", false))) {
		return Ref<Image>();
	}
	return m.get("image", Ref<Image>());
}

Dictionary Milestro::render_text_metrics(const String &text, const String &font_family, double font_size,
		int32_t width, int32_t height) const {
	PackedStringArray families;
	if (!font_family.is_empty()) {
		families.push_back(font_family);
	}
	return render_mixed(text, families, font_size, width, height, PackedStringArray(), PackedVector2Array(),
			PackedFloat32Array());
}

Dictionary Milestro::render_mixed(const String &text, const PackedStringArray &font_families, double font_size,
		int32_t width, int32_t height, const PackedStringArray &image_paths,
		const PackedVector2Array &image_positions, const PackedFloat32Array &image_heights) const {
	Dictionary out;
	out["ok"] = false;
	out["code"] = int64_t(-1);
	out["height"] = 0.0;
	out["longest_line"] = 0.0;
	out["non_zero_pixels"] = 0;
	out["byte_size"] = 0;
	out["images_drawn"] = 0;

	if (width <= 0 || height <= 0 || font_size <= 0.0) {
		out["code"] = int64_t(-2);
		return out;
	}

	const std::string utf8 = to_utf8(text);
	if (utf8.empty()) {
		out["code"] = int64_t(-3);
		return out;
	}

	// Keep family strings alive for the C API pointer array.
	std::vector<std::string> family_storage;
	std::vector<uint8_t *> family_ptrs;
	family_storage.reserve(static_cast<size_t>(font_families.size()));
	for (int i = 0; i < font_families.size(); ++i) {
		const std::string f = to_utf8(font_families[i]);
		if (f.empty()) {
			continue;
		}
		family_storage.push_back(f);
	}
	family_ptrs.reserve(family_storage.size());
	for (auto &f : family_storage) {
		family_ptrs.push_back(reinterpret_cast<uint8_t *>(f.data()));
	}

	milestro::skia::textlayout::TextStyle *text_style = nullptr;
	milestro::skia::textlayout::ParagraphStyle *para_style = nullptr;
	milestro::skia::textlayout::ParagraphBuilder *builder = nullptr;
	milestro::skia::textlayout::Paragraph *paragraph = nullptr;
	milestro::skia::Canvas *canvas = nullptr;
	std::vector<milestro::skia::Image *> loaded_images;

	auto cleanup = [&]() {
		for (auto *img : loaded_images) {
			if (img) {
				MilestroSkiaImageDestroy(img);
			}
		}
		loaded_images.clear();
		if (canvas) {
			MilestroSkiaCanvasDestroy(canvas);
			canvas = nullptr;
		}
		if (paragraph) {
			MilestroSkiaTextlayoutParagraphDestroy(paragraph);
			paragraph = nullptr;
		}
		if (builder) {
			MilestroSkiaTextlayoutParagraphBuilderDestroy(builder);
			builder = nullptr;
		}
		if (para_style) {
			MilestroSkiaTextlayoutParagraphStyleDestroy(para_style);
			para_style = nullptr;
		}
		if (text_style) {
			MilestroSkiaTextlayoutTextStyleDestroy(text_style);
			text_style = nullptr;
		}
	};

	// Prefer font fallback so emoji can resolve via secondary families / system.
	MilestroSkiaFontCollectionSetFontFallbackEnabled(1);

	int64_t rc = MilestroSkiaTextlayoutTextStyleCreate(text_style);
	if (rc != kOk || text_style == nullptr) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	rc = MilestroSkiaTextlayoutTextStyleSetFontSize(text_style, static_cast<float>(font_size));
	if (rc != kOk) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	// Opaque dark text for visibility on light/transparent canvas.
	rc = MilestroSkiaTextlayoutTextStyleSetColor(text_style, 20, 20, 24, 255);
	if (rc != kOk) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	if (!family_ptrs.empty()) {
		rc = MilestroSkiaTextlayoutTextStyleSetFontFamilies(text_style, family_ptrs.data(),
				static_cast<uint32_t>(family_ptrs.size()));
		if (rc != kOk) {
			out["code"] = rc;
			cleanup();
			return out;
		}
	}

	rc = MilestroSkiaTextlayoutParagraphStyleCreate(para_style);
	if (rc != kOk || para_style == nullptr) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	rc = MilestroSkiaTextlayoutParagraphStyleSetTextStyle(para_style, text_style);
	if (rc != kOk) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	rc = MilestroSkiaTextlayoutParagraphBuilderCreate(builder, para_style);
	if (rc != kOk || builder == nullptr) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	rc = MilestroSkiaTextlayoutParagraphBuilderPushStyle(builder, text_style);
	if (rc != kOk) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	rc = MilestroSkiaTextlayoutParagraphBuilderAddText(builder,
			reinterpret_cast<uint8_t *>(const_cast<char *>(utf8.data())), utf8.size());
	if (rc != kOk) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	rc = MilestroSkiaTextlayoutParagraphBuilderBuild(builder, paragraph);
	if (rc != kOk || paragraph == nullptr) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	rc = MilestroSkiaTextlayoutParagraphLayout(paragraph, static_cast<float>(width));
	if (rc != kOk) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	float measured_height = 0.f;
	float longest_line = 0.f;
	MilestroSkiaTextlayoutParagraphGetHeight(paragraph, measured_height);
	MilestroSkiaTextlayoutParagraphGetLongestLine(paragraph, longest_line);
	out["height"] = measured_height;
	out["longest_line"] = longest_line;

	if (measured_height <= 0.f) {
		out["code"] = int64_t(-4);
		cleanup();
		return out;
	}

	const size_t pixel_bytes = static_cast<size_t>(width) * static_cast<size_t>(height) * 4u;
	std::vector<uint8_t> pixels(pixel_bytes, 0);

	// Soft light background so transparent emoji/images still show structure.
	for (size_t i = 0; i + 3 < pixel_bytes; i += 4) {
		pixels[i + 0] = 245;
		pixels[i + 1] = 246;
		pixels[i + 2] = 248;
		pixels[i + 3] = 255;
	}

	rc = MilestroSkiaCanvasCreateWithMemory(canvas, width, height, pixels.data(), 0, 0);
	if (rc != kOk || canvas == nullptr) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	// 1) Paint text (+ emoji via font fallback chain)
	rc = MilestroSkiaTextlayoutParagraphPaint(paragraph, canvas, 16.f, 12.f);
	if (rc != kOk) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	// 2) Overlay images
	int images_drawn = 0;
	const int n_img = image_paths.size();
	for (int i = 0; i < n_img; ++i) {
		const std::string path = to_utf8(image_paths[i]);
		std::vector<uint8_t> file_bytes;
		if (!read_file_bytes(path, file_bytes)) {
			UtilityFunctions::push_warning(String("Milestro: failed to read image: ") + image_paths[i]);
			continue;
		}

		milestro::skia::Image *sk_img = nullptr;
		rc = MilestroSkiaImageCreate(sk_img, file_bytes.data(), static_cast<uint64_t>(file_bytes.size()));
		if (rc != kOk || sk_img == nullptr) {
			UtilityFunctions::push_warning(String("Milestro: ImageCreate failed: ") + image_paths[i]);
			continue;
		}
		loaded_images.push_back(sk_img);

		int32_t iw = 0;
		int32_t ih = 0;
		MilestroSkiaImageGetWidth(sk_img, iw);
		MilestroSkiaImageGetHeight(sk_img, ih);
		if (iw <= 0 || ih <= 0) {
			continue;
		}

		Vector2 pos(16.f, measured_height + 24.f);
		if (i < image_positions.size()) {
			pos = image_positions[i];
		}

		float target_h = static_cast<float>(ih);
		if (i < image_heights.size() && image_heights[i] > 0.f) {
			target_h = image_heights[i];
		}
		const float scale = target_h / static_cast<float>(ih);
		const float target_w = static_cast<float>(iw) * scale;

		rc = MilestroSkiaCanvasDrawImage(canvas, sk_img, 0.f, 0.f, static_cast<float>(iw), static_cast<float>(ih),
				pos.x, pos.y, pos.x + target_w, pos.y + target_h);
		if (rc == kOk) {
			++images_drawn;
		}
	}

	rc = MilestroSkiaCanvasGetTexture(canvas, pixels.data());
	if (rc != kOk) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	const int non_zero = count_non_zero(pixels);

	PackedByteArray packed;
	packed.resize(static_cast<int64_t>(pixel_bytes));
	std::memcpy(packed.ptrw(), pixels.data(), pixel_bytes);

	Ref<Image> image = Image::create_from_data(width, height, false, Image::FORMAT_RGBA8, packed);

	out["ok"] = true;
	out["code"] = kOk;
	out["non_zero_pixels"] = non_zero;
	out["byte_size"] = static_cast<int64_t>(pixel_bytes);
	out["image"] = image;
	out["width"] = width;
	out["canvas_height"] = height;
	out["images_drawn"] = images_drawn;

	cleanup();
	return out;
}

#include "milestro_binding.h"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include <Milestro/game/milestro_game_interface.h>

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
	ClassDB::bind_method(D_METHOD("get_version"), &Milestro::get_version);
	ClassDB::bind_method(D_METHOD("register_font_from_file", "path"), &Milestro::register_font_from_file);
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

int64_t Milestro::register_font_from_file(const String &path) const {
	const std::string p = to_utf8(path);
	return MilestroSkiaFontRegistryRegisterFontFromFile(
			reinterpret_cast<uint8_t *>(const_cast<char *>(p.data())),
			static_cast<uint64_t>(p.size()));
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

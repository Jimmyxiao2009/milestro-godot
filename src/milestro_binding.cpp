#include "milestro_binding.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

// Consumer-side include of the official plugin API.
// Headers live under include/Milestro/{game,common}; export macros need "common" on the path.
// MILESTRO_DLL is provided by the build system for import decoration.
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

} // namespace

void Milestro::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_version"), &Milestro::get_version);
	ClassDB::bind_method(D_METHOD("register_font_from_file", "path"), &Milestro::register_font_from_file);
	ClassDB::bind_method(D_METHOD("render_text", "text", "font_family", "font_size", "width", "height"),
			&Milestro::render_text);
	ClassDB::bind_method(D_METHOD("render_text_metrics", "text", "font_family", "font_size", "width", "height"),
			&Milestro::render_text_metrics);
}

Dictionary Milestro::get_version() const {
	int32_t major = 0;
	int32_t minor = 0;
	int32_t patch = 0;
	// Native contract: return value is the major version number (see milestro_game_plugin_main.cpp).
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
	Dictionary out;
	out["ok"] = false;
	out["code"] = int64_t(-1);
	out["height"] = 0.0;
	out["longest_line"] = 0.0;
	out["non_zero_pixels"] = 0;
	out["byte_size"] = 0;

	if (width <= 0 || height <= 0 || font_size <= 0.0) {
		out["code"] = int64_t(-2);
		return out;
	}

	const std::string utf8 = to_utf8(text);
	const std::string family = to_utf8(font_family);
	if (utf8.empty()) {
		out["code"] = int64_t(-3);
		return out;
	}

	milestro::skia::textlayout::TextStyle *text_style = nullptr;
	milestro::skia::textlayout::ParagraphStyle *para_style = nullptr;
	milestro::skia::textlayout::ParagraphBuilder *builder = nullptr;
	milestro::skia::textlayout::Paragraph *paragraph = nullptr;
	milestro::skia::Canvas *canvas = nullptr;

	auto cleanup = [&]() {
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

	// Opaque black text on transparent/cleared canvas.
	rc = MilestroSkiaTextlayoutTextStyleSetColor(text_style, 0, 0, 0, 255);
	if (rc != kOk) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	if (!family.empty()) {
		uint8_t *family_ptr = reinterpret_cast<uint8_t *>(const_cast<char *>(family.data()));
		rc = MilestroSkiaTextlayoutTextStyleSetFontFamilies(text_style, &family_ptr, 1);
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
			reinterpret_cast<uint8_t *>(const_cast<char *>(utf8.data())),
			utf8.size());
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

	if (measured_height <= 0.f || longest_line <= 0.f) {
		out["code"] = int64_t(-4);
		cleanup();
		return out;
	}

	const size_t pixel_bytes = static_cast<size_t>(width) * static_cast<size_t>(height) * 4u;
	std::vector<uint8_t> pixels(pixel_bytes, 0);

	// clearPixels=1 zeros the buffer; verticalFlip=0 keeps Godot-friendly top-left origin.
	rc = MilestroSkiaCanvasCreateWithMemory(canvas, width, height, pixels.data(), 0, 1);
	if (rc != kOk || canvas == nullptr) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	rc = MilestroSkiaTextlayoutParagraphPaint(paragraph, canvas, 0.f, 0.f);
	if (rc != kOk) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	// Pull pixels back (same buffer for memory-backed canvas, but call real API).
	rc = MilestroSkiaCanvasGetTexture(canvas, pixels.data());
	if (rc != kOk) {
		out["code"] = rc;
		cleanup();
		return out;
	}

	int non_zero = 0;
	for (uint8_t b : pixels) {
		if (b != 0) {
			++non_zero;
		}
	}

	PackedByteArray packed;
	packed.resize(static_cast<int64_t>(pixel_bytes));
	{
		uint8_t *dst = packed.ptrw();
		std::memcpy(dst, pixels.data(), pixel_bytes);
	}

	Ref<Image> image = Image::create_from_data(width, height, false, Image::FORMAT_RGBA8, packed);

	out["ok"] = true;
	out["code"] = kOk;
	out["non_zero_pixels"] = non_zero;
	out["byte_size"] = static_cast<int64_t>(pixel_bytes);
	out["image"] = image;
	out["width"] = width;
	out["canvas_height"] = height;

	cleanup();
	return out;
}

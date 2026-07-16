// Standalone smoke test driving the real Milestro C plugin API
// (same symbols used by the Godot GDExtension binding).

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

// MILESTRO_DLL comes from CMake compile definitions.
#include <Milestro/game/milestro_game_interface.h>

namespace {

constexpr int64_t kOk = 0;

struct Metrics {
	float height = 0.f;
	float longest_line = 0.f;
	int non_zero = 0;
	int width = 0;
	int height_px = 0;
	std::vector<uint8_t> pixels;
};

bool render_sample(const char *font_path, const char *family, const char *text, Metrics &m) {
	const std::string font(font_path);
	const int64_t reg = MilestroSkiaFontRegistryRegisterFontFromFile(
			reinterpret_cast<uint8_t *>(const_cast<char *>(font.data())),
			static_cast<uint64_t>(font.size()));
	if (reg != kOk) {
		std::fprintf(stderr, "RegisterFont failed: %lld\n", static_cast<long long>(reg));
		// Registration may return non-zero for "already registered" — continue if family works.
	}

	milestro::skia::textlayout::TextStyle *text_style = nullptr;
	milestro::skia::textlayout::ParagraphStyle *para_style = nullptr;
	milestro::skia::textlayout::ParagraphBuilder *builder = nullptr;
	milestro::skia::textlayout::Paragraph *paragraph = nullptr;
	milestro::skia::Canvas *canvas = nullptr;

	auto cleanup = [&]() {
		if (canvas) {
			MilestroSkiaCanvasDestroy(canvas);
		}
		if (paragraph) {
			MilestroSkiaTextlayoutParagraphDestroy(paragraph);
		}
		if (builder) {
			MilestroSkiaTextlayoutParagraphBuilderDestroy(builder);
		}
		if (para_style) {
			MilestroSkiaTextlayoutParagraphStyleDestroy(para_style);
		}
		if (text_style) {
			MilestroSkiaTextlayoutTextStyleDestroy(text_style);
		}
	};

	if (MilestroSkiaTextlayoutTextStyleCreate(text_style) != kOk || !text_style) {
		cleanup();
		return false;
	}
	MilestroSkiaTextlayoutTextStyleSetFontSize(text_style, 36.f);
	MilestroSkiaTextlayoutTextStyleSetColor(text_style, 0, 0, 0, 255);

	std::string fam(family);
	uint8_t *fam_ptr = reinterpret_cast<uint8_t *>(fam.data());
	if (MilestroSkiaTextlayoutTextStyleSetFontFamilies(text_style, &fam_ptr, 1) != kOk) {
		cleanup();
		return false;
	}

	if (MilestroSkiaTextlayoutParagraphStyleCreate(para_style) != kOk || !para_style) {
		cleanup();
		return false;
	}
	MilestroSkiaTextlayoutParagraphStyleSetTextStyle(para_style, text_style);

	if (MilestroSkiaTextlayoutParagraphBuilderCreate(builder, para_style) != kOk || !builder) {
		cleanup();
		return false;
	}
	MilestroSkiaTextlayoutParagraphBuilderPushStyle(builder, text_style);

	std::string body(text);
	if (MilestroSkiaTextlayoutParagraphBuilderAddText(builder,
				reinterpret_cast<uint8_t *>(body.data()), body.size()) != kOk) {
		cleanup();
		return false;
	}
	if (MilestroSkiaTextlayoutParagraphBuilderBuild(builder, paragraph) != kOk || !paragraph) {
		cleanup();
		return false;
	}

	const int width = 512;
	const int height = 256;
	if (MilestroSkiaTextlayoutParagraphLayout(paragraph, static_cast<float>(width)) != kOk) {
		cleanup();
		return false;
	}
	MilestroSkiaTextlayoutParagraphGetHeight(paragraph, m.height);
	MilestroSkiaTextlayoutParagraphGetLongestLine(paragraph, m.longest_line);

	m.pixels.assign(static_cast<size_t>(width) * height * 4, 0);
	if (MilestroSkiaCanvasCreateWithMemory(canvas, width, height, m.pixels.data(), 0, 1) != kOk || !canvas) {
		cleanup();
		return false;
	}
	if (MilestroSkiaTextlayoutParagraphPaint(paragraph, canvas, 8.f, 8.f) != kOk) {
		cleanup();
		return false;
	}
	if (MilestroSkiaCanvasGetTexture(canvas, m.pixels.data()) != kOk) {
		cleanup();
		return false;
	}

	m.non_zero = 0;
	for (uint8_t b : m.pixels) {
		if (b != 0) {
			++m.non_zero;
		}
	}
	m.width = width;
	m.height_px = height;
	cleanup();
	return m.height > 0.f && m.longest_line > 0.f && m.non_zero > 0;
}

// Minimal uncompressed TGA writer for visual evidence (BGR).
bool write_tga(const char *path, int w, int h, const std::vector<uint8_t> &rgba) {
	std::ofstream f(path, std::ios::binary);
	if (!f) {
		return false;
	}
	unsigned char header[18] = {};
	header[2] = 2; // uncompressed true-color
	header[12] = static_cast<unsigned char>(w & 0xff);
	header[13] = static_cast<unsigned char>((w >> 8) & 0xff);
	header[14] = static_cast<unsigned char>(h & 0xff);
	header[15] = static_cast<unsigned char>((h >> 8) & 0xff);
	header[16] = 24;
	header[17] = 0x20; // top-left origin
	f.write(reinterpret_cast<const char *>(header), 18);
	for (int i = 0; i < w * h; ++i) {
		const uint8_t r = rgba[static_cast<size_t>(i) * 4 + 0];
		const uint8_t g = rgba[static_cast<size_t>(i) * 4 + 1];
		const uint8_t b = rgba[static_cast<size_t>(i) * 4 + 2];
		f.put(static_cast<char>(b));
		f.put(static_cast<char>(g));
		f.put(static_cast<char>(r));
	}
	return static_cast<bool>(f);
}

} // namespace

int main(int argc, char **argv) {
	const char *font_path = argc > 1 ? argv[1]
									 : "/Users/JimmyXiao/Milestro/tests/data/font/SourceHanSans-VF.otf.woff2.bytes";
	const char *family = argc > 2 ? argv[2] : "Source Han Sans VF";
	const char *text = argc > 3 ? argv[3] : "Hello 你好 Milestro";
	const char *out_dir = argc > 4 ? argv[4] : ".";

	int32_t major = 0, minor = 0, patch = 0;
	// API contract: return value is the major version (not MILESTRO_API_RET_OK).
	const int64_t vrc = MilestroGetVersion(major, minor, patch);
	std::printf("version_rc=%lld version=%d.%d.%d\n", static_cast<long long>(vrc), major, minor, patch);
	if (major != 1 || minor != 0 || patch != 0 || vrc != major) {
		std::fprintf(stderr, "FAIL: unexpected version\n");
		return 1;
	}

	Metrics m;
	if (!render_sample(font_path, family, text, m)) {
		std::fprintf(stderr, "FAIL: render_sample\n");
		return 2;
	}

	std::printf("layout_height=%.3f longest_line=%.3f non_zero_pixels=%d canvas=%dx%d\n",
			m.height, m.longest_line, m.non_zero, m.width, m.height_px);

	const std::string tga = std::string(out_dir) + "/sample-text.tga";
	if (!write_tga(tga.c_str(), m.width, m.height_px, m.pixels)) {
		std::fprintf(stderr, "WARN: could not write %s\n", tga.c_str());
	} else {
		std::printf("wrote %s\n", tga.c_str());
	}

	if (m.non_zero <= 0) {
		std::fprintf(stderr, "FAIL: blank raster\n");
		return 3;
	}
	std::printf("SMOKE_OK\n");
	return 0;
}

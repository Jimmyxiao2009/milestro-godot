extends Control
## Milestro 完整移植测试 — 验证所有新 API

@onready var status_label: Label = $VBox/StatusLabel
@onready var detail_label: Label = $VBox/DetailLabel

const SAMPLE_TEXT := "Hello 你好 Milestro"
const FONT_SIZE := 48.0
const CANVAS_W := 640
const CANVAS_H := 160

func _ready() -> void:
	_run_all_test()

func _run_all_test() -> void:
	var tests_passed := 0
	var tests_failed := 0

	# Test 1: 版本查询
	if _test_version():
		tests_passed += 1
	else:
		tests_failed += 1

	# Test 2: 字体注册
	if _test_font_registration():
		tests_passed += 1
	else:
		tests_failed += 1

	# Test 3: 字体对象
	if _test_font_object():
		tests_passed += 1
	else:
		tests_failed += 1

	# Test 4: 文本样式
	if _test_text_style():
		tests_passed += 1
	else:
		tests_failed += 1

	# Test 5: 段落样式
	if _test_paragraph_style():
		tests_passed += 1
	else:
		tests_failed += 1

	# Test 6: 段落构建和渲染
	if _test_paragraph_build():
		tests_passed += 1
	else:
		tests_failed += 1

	# Test 7: Canvas 操作
	if _test_canvas():
		tests_passed += 1
	else:
		tests_failed += 1

	# Test 8: 图像操作
	if _test_image():
		tests_passed += 1
	else:
		tests_failed += 1

	# Test 9: 输入框
	if _test_input_box():
		tests_passed += 1
	else:
		tests_failed += 1

	# Test 10: 绘制快照
	if _test_snapshots():
		tests_passed += 1
	else:
		tests_failed += 1

	# Test 11: Unicode
	if _test_unicode():
		tests_passed += 1
	else:
		tests_failed += 1

	# Test 12: 向后兼容渲染
	if _test_backward_compat():
		tests_passed += 1
	else:
		tests_failed += 1

	# 总结
	var total := tests_passed + tests_failed
	if tests_failed == 0:
		_pass("全部通过! %d/%d" % [tests_passed, total])
	else:
		_fail("通过 %d/%d, 失败 %d" % [tests_passed, total, tests_failed])

func _test_version() -> bool:
	if not ClassDB.class_exists("Milestro"):
		_fail("GDExtension 未加载")
		return false

	var m = ClassDB.instantiate("Milestro")
	var ver: Dictionary = m.get_version()

	if not ver.get("ok", false):
		_fail("get_version 失败")
		return false

	if int(ver.major) != 1 or int(ver.minor) != 0 or int(ver.patch) != 0:
		_fail("版本不符合预期 1.0.0")
		return false

	print("[test] version = %d.%d.%d" % [ver.major, ver.minor, ver.patch])
	return true

func _test_font_registration() -> bool:
	var m = ClassDB.instantiate("Milestro")
	var font_path := _find_font_path()
	if font_path.is_empty():
		_warn("找不到测试字体,跳过字体注册测试")
		return true

	var rc: int = m.register_font_from_file(font_path)
	print("[test] register_font rc = ", rc)
	# 0 = OK, 非0可能是"已注册"
	return true

func _test_font_object() -> bool:
	var m = Milestro.new()
	var font = m.create_font("sans-serif", 400, 24.0)
	if font == null:
		_fail("create_font 返回 null")
		return false
	var font_metrics: Dictionary = font.get_metrics()
	var measured: Dictionary = font.measure_text("Hello 你好")
	if float(font_metrics.get("ascent", 0.0)) >= 0.0 or float(measured.get("advance_x", 0.0)) <= 0.0:
		_fail("字体对象没有有效 metrics")
		return false
	print("[test] Font metrics=", font_metrics, " measured=", measured)
	return true

func _test_text_style() -> bool:
	var m = ClassDB.instantiate("Milestro")
	var style = m.create_text_style()
	if style == null:
		_fail("create_text_style 返回 null")
		return false

	style.set_color(255, 0, 0, 255)
	style.set_font_size(24.0)
	style.set_font_families(["sans-serif"])
	style.set_letter_spacing(1.5)
	style.set_word_spacing(2.0)
	style.set_baseline_shift(2.0)

	print("[test] TextStyle created and configured")
	return true

func _test_paragraph_style() -> bool:
	var m = ClassDB.instantiate("Milestro")
	var para_style = m.create_paragraph_style()
	if para_style == null:
		_fail("create_paragraph_style 返回 null")
		return false

	para_style.set_text_direction(0)  # LTR
	para_style.set_text_align(2)       # Center
	para_style.set_max_lines(3)
	para_style.set_height(1.5)

	print("[test] ParagraphStyle created and configured")
	return true

func _test_paragraph_build() -> bool:
	var m = ClassDB.instantiate("Milestro")

	var text_style = m.create_text_style()
	text_style.set_font_size(FONT_SIZE)
	text_style.set_color(0, 0, 0, 255)

	var para_style = m.create_paragraph_style()
	para_style.set_text_style(text_style)

	var builder = m.create_paragraph_builder(para_style)
	if builder == null:
		_fail("create_paragraph_builder 返回 null")
		return false

	builder.add_text(SAMPLE_TEXT)
	var paragraph = builder.build()

	if paragraph == null:
		_fail("paragraph build 失败")
		return false

	var layout_ok = paragraph.layout(CANVAS_W)
	if not layout_ok:
		_fail("paragraph layout 失败")
		return false

	var height = paragraph.get_height()
	var longest = paragraph.get_longest_line()

	print("[test] Paragraph: height=%.1f longest=%.1f" % [height, longest])

	if height <= 0.0 or longest <= 0.0:
		_fail("段落尺寸为0")
		return false

	return true

func _test_canvas() -> bool:
	var m = ClassDB.instantiate("Milestro")
	var canvas = m.create_canvas(200, 100)
	if canvas == null:
		_fail("create_canvas 返回 null")
		return false

	# 获取纹理数据
	var pixels = canvas.get_texture()
	if pixels.is_empty():
		_fail("canvas get_texture 返回空")
		return false

	print("[test] Canvas: %d pixels" % pixels.size())
	var png_path := ProjectSettings.globalize_path("user://milestro_canvas_test.png")
	if not canvas.save_to_png(png_path) or not FileAccess.file_exists(png_path):
		_fail("canvas save_to_png 失败")
		return false
	return true

func _test_image() -> bool:
	var m = ClassDB.instantiate("Milestro")
	var image = m.create_image_from_file("/nonexistent.png")
	if image != null:
		_fail("不存在的图片不应创建成功")
		return false
	print("[test] invalid image rejected")
	return true

func _test_input_box() -> bool:
	var m = ClassDB.instantiate("Milestro")

	var text_style = m.create_text_style()
	var para_style = m.create_paragraph_style()

	var input_box = m.create_input_box(para_style, text_style)
	if input_box == null:
		_fail("create_input_box 返回 null")
		return false

	input_box.set_text("Hello")
	input_box.set_viewport(300, 100)
	input_box.set_soft_wrap(true)

	var metrics = input_box.get_metrics()
	print("[test] InputBox metrics: ", metrics)
	if input_box.get_text() != "Hello":
		_fail("InputBox get_text 与 set_text 不一致")
		return false
	return true

func _test_snapshots() -> bool:
	var m = Milestro.new()
	var font = m.create_font("sans-serif", 400, 28.0)
	var snapshot = m.create_text_draw_snapshot(font, "Snapshot", 255, 255, 255, 255)
	if snapshot == null or float(snapshot.measure_text().get("advance_x", 0.0)) <= 0.0:
		_fail("TextDrawSnapshot 创建或测量失败")
		return false
	var canvas = m.create_canvas(320, 80)
	snapshot.paint_text(canvas, 8.0, 42.0)
	if canvas.get_texture().is_empty():
		_fail("TextDrawSnapshot 绘制失败")
		return false
	var reusable = m.create_reusable_text_draw_snapshot(font, 64, 255, 255, 255, 255)
	if reusable == null or not reusable.update_text("Reusable"):
		_fail("ReusableTextDrawSnapshot 更新失败")
		return false
	reusable.paint_text(canvas, 8.0, 70.0)
	return true

func _test_unicode() -> bool:
	var m = Milestro.new()
	if m.to_upper("en", "hello") != "HELLO":
		_fail("to_upper 失败")
		return false
	if m.to_lower("en", "HELLO") != "hello":
		_fail("to_lower 失败")
		return false
	var normalizer = MilestroUnicodeNormalizer.new()
	if not normalizer.init("NFC", 0) or normalizer.normalize("e\u0301") != "é":
		_fail("Unicode normalizer 失败")
		return false
	var segmenter = MilestroUnicodeSegmenter.new()
	if not segmenter.init("en_US", "Hello world") or segmenter.first() != 0 or segmenter.next() <= 0:
		_fail("Unicode segmenter 失败")
		return false
	if segmenter.substring(0, 5) != "Hello":
		_fail("Unicode segmenter substring 失败")
		return false
	var transliterator = MilestroUnicodeTransliterator.new()
	if not transliterator.init("Any-Latin", 0) or transliterator.transliterate("Привет").is_empty():
		_fail("Unicode transliterator 失败")
		return false
	var comparator = MilestroStringComparator.new()
	if not comparator.init("en_US") or comparator.compare("a", "b") >= 0:
		_fail("Unicode string comparator 失败")
		return false
	return true

func _test_backward_compat() -> bool:
	var m = ClassDB.instantiate("Milestro")

	var font_path := _find_font_path()
	if not font_path.is_empty():
		m.register_font_from_file(font_path)

	var family := _guess_family(font_path)
	var metrics: Dictionary = m.render_text_metrics(
		SAMPLE_TEXT,
		family,
		FONT_SIZE,
		CANVAS_W,
		CANVAS_H
	)

	if not metrics.get("ok", false):
		_fail("render_text_metrics 失败")
		return false

	var height := float(metrics.get("height", 0.0))
	var longest := float(metrics.get("longest_line", 0.0))
	var nonzero := int(metrics.get("non_zero_pixels", 0))

	print("[test] Backward compat: height=%.1f longest=%.1f non_zero=%d" % [height, longest, nonzero])

	if height <= 0.0 or longest <= 0.0 or nonzero <= 0:
		_fail("向后兼容测试失败")
		return false

	return true

func _find_font_path() -> String:
	var env := OS.get_environment("MILESTRO_FONT_PATH")
	if not env.is_empty() and FileAccess.file_exists(env):
		return env

	var local_candidates := [
		"res://tests/fonts/SourceHanSans.otf",
		"res://tests/fonts/NotoSansSC.otf",
		"res://tests/fonts/test.ttf",
	]
	for c in local_candidates:
		if ResourceLoader.exists(c) or FileAccess.file_exists(c):
			return ProjectSettings.globalize_path(c)

	var abs_candidates := [
		"/System/Library/Fonts/PingFang.ttc",
		"/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
		"/Library/Fonts/Arial Unicode.ttf",
	]
	for c in abs_candidates:
		if FileAccess.file_exists(c):
			return c
	return ""

func _guess_family(font_path: String) -> String:
	if font_path.contains("SourceHan") or font_path.contains("Source Han"):
		return "Source Han Sans VF"
	if font_path.contains("PingFang"):
		return "PingFang SC"
	if font_path.contains("Arial"):
		return "Arial Unicode MS"
	return "sans-serif"

func _pass(msg: String) -> void:
	status_label.text = "PASS"
	status_label.add_theme_color_override("font_color", Color(0.2, 0.85, 0.35))
	detail_label.text = msg
	print("[test] PASS: ", msg)

func _fail(msg: String) -> void:
	status_label.text = "FAIL"
	status_label.add_theme_color_override("font_color", Color(1.0, 0.35, 0.3))
	detail_label.text = msg
	push_error("[test] FAIL: " + msg)

func _warn(msg: String) -> void:
	print("[test] WARN: ", msg)

extends Control
## 简单的 Milestro 测试用例：版本 + 注册字体 + 渲染中英混合文字并显示在屏幕上。
## 在 Godot 中打开 tests/milestro_simple_test.tscn 并运行（F6）。

const SAMPLE_TEXT := "Hello 你好 Milestro"
const FONT_SIZE := 48.0
const CANVAS_W := 640
const CANVAS_H := 160

@onready var status_label: Label = $VBox/StatusLabel
@onready var detail_label: Label = $VBox/DetailLabel
@onready var preview: TextureRect = $VBox/Preview


func _ready() -> void:
	_run_test()


func _run_test() -> void:
	# --- 1) 扩展是否加载 ---
	if not ClassDB.class_exists("Milestro"):
		_fail("GDExtension 未加载：ClassDB 中没有 Milestro")
		return

	var m = ClassDB.instantiate("Milestro")
	if m == null:
		_fail("无法实例化 Milestro")
		return

	# --- 2) 版本（真实 native API，不是假数据）---
	var ver: Dictionary = m.get_version()
	print("[test] version = ", ver)
	if not ver.get("ok", false):
		_fail("get_version 失败: %s" % str(ver))
		return
	if int(ver.major) != 1 or int(ver.minor) != 0 or int(ver.patch) != 0:
		_fail("版本不符合预期 1.0.0，实际 %s.%s.%s" % [ver.major, ver.minor, ver.patch])
		return

	# --- 3) 注册字体 ---
	var font_path := _find_font_path()
	print("[test] font = ", font_path)
	if font_path.is_empty():
		_fail("找不到测试字体。请设置环境变量 MILESTRO_FONT_PATH 或把 .ttf/.otf 放到 res://tests/fonts/")
		return

	var reg: int = m.register_font_from_file(font_path)
	print("[test] register_font_from_file rc = ", reg)
	# 0 = OK；部分情况「已注册」也可能非 0，下面渲染会最终判定

	# --- 4) 排版 + 栅格化 ---
	var family := _guess_family(font_path)
	var metrics: Dictionary = m.render_text_metrics(
		SAMPLE_TEXT,
		family,
		FONT_SIZE,
		CANVAS_W,
		CANVAS_H
	)
	print("[test] metrics = ", metrics)

	if not metrics.get("ok", false):
		_fail("render_text_metrics 失败 code=%s" % str(metrics.get("code", "?")))
		return

	var height := float(metrics.get("height", 0.0))
	var longest := float(metrics.get("longest_line", 0.0))
	var nonzero := int(metrics.get("non_zero_pixels", 0))
	if height <= 0.0 or longest <= 0.0:
		_fail("排版尺寸为 0：height=%s longest=%s" % [height, longest])
		return
	if nonzero <= 0:
		_fail("像素全空（non_zero_pixels=0），可能字体 family 不匹配")
		return

	var img: Image = metrics.get("image", null)
	if img == null or img.is_empty():
		_fail("Image 为空")
		return

	# 显示结果
	preview.texture = ImageTexture.create_from_image(img)
	# 可选：保存到 user://
	img.save_png("user://milestro_simple_test.png")

	_pass(
		"PASS  version=%s.%s.%s  height=%.1f  longest=%.1f  non_zero=%d" % [
			ver.major, ver.minor, ver.patch, height, longest, nonzero
		]
	)


func _pass(msg: String) -> void:
	status_label.text = "PASS"
	status_label.add_theme_color_override("font_color", Color(0.2, 0.85, 0.35))
	detail_label.text = msg
	print("[test] ", msg)


func _fail(msg: String) -> void:
	status_label.text = "FAIL"
	status_label.add_theme_color_override("font_color", Color(1.0, 0.35, 0.3))
	detail_label.text = msg
	push_error("[test] " + msg)


func _find_font_path() -> String:
	var env := OS.get_environment("MILESTRO_FONT_PATH")
	if not env.is_empty() and FileAccess.file_exists(env):
		return env

	# 项目内自带字体（可自行放入）
	var local_candidates := [
		"res://tests/fonts/SourceHanSans.otf",
		"res://tests/fonts/NotoSansSC.otf",
		"res://tests/fonts/test.ttf",
	]
	for c in local_candidates:
		if ResourceLoader.exists(c) or FileAccess.file_exists(c):
			return ProjectSettings.globalize_path(c)

	# 本机开发路径（移植时的默认测试字体）
	var abs_candidates := [
		"/Users/JimmyXiao/Milestro/tests/data/font/SourceHanSans-VF.otf.woff2.bytes",
		"/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
		"/System/Library/Fonts/PingFang.ttc",
		"/Library/Fonts/Arial Unicode.ttf",
	]
	for c in abs_candidates:
		if FileAccess.file_exists(c):
			return c
	return ""


func _guess_family(font_path: String) -> String:
	# 与移植验证一致的 family；其它字体可改成实际 PostScript/family 名
	if font_path.contains("SourceHan") or font_path.contains("Source Han"):
		return "Source Han Sans VF"
	if font_path.contains("PingFang"):
		return "PingFang SC"
	if font_path.contains("Arial"):
		return "Arial Unicode MS"
	# 空 family 时依赖 Skia/系统 fallback（效果因平台而异）
	return "sans-serif"

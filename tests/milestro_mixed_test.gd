extends Control
## 混排测试：中英文 + Emoji + 图片（PNG/JPG），走 native Milestro canvas。
## 运行：打开本场景 F6，或设为主场景。

const SAMPLE_TEXT := "Hello 你好 🎉 混排测试 😀 image↓"
const FONT_SIZE := 40.0
const CANVAS_W := 720
const CANVAS_H := 420

@onready var status_label: Label = $VBox/StatusLabel
@onready var detail_label: Label = $VBox/DetailLabel
@onready var preview: TextureRect = $VBox/Preview


func _ready() -> void:
	_run_mixed_test()


func _run_mixed_test() -> void:
	if not ClassDB.class_exists("Milestro"):
		_fail("GDExtension 未加载：没有 Milestro 类")
		return

	var m = ClassDB.instantiate("Milestro")
	if m == null:
		_fail("无法实例化 Milestro")
		return

	var ver: Dictionary = m.get_version()
	if not ver.get("ok", false):
		_fail("get_version 失败: %s" % str(ver))
		return

	# --- 字体：CJK + Color Emoji ---
	var cjk := _find_cjk_font()
	var emoji := _find_emoji_font()
	print("[mixed] cjk_font=", cjk)
	print("[mixed] emoji_font=", emoji)

	if cjk.is_empty():
		_fail("找不到 CJK 字体")
		return

	var rc_cjk: int = m.register_font_from_file(cjk)
	print("[mixed] register cjk rc=", rc_cjk)
	if not emoji.is_empty():
		var rc_emoji: int = m.register_font_from_file(emoji)
		print("[mixed] register emoji rc=", rc_emoji)

	var families := PackedStringArray()
	families.push_back("Source Han Sans VF")
	if not emoji.is_empty():
		families.push_back("Noto Color Emoji")
	# 再挂一个通用 fallback 名
	families.push_back("sans-serif")

	# --- 图片资源 ---
	var img_paths := PackedStringArray()
	var img_pos := PackedVector2Array()
	var img_h := PackedFloat32Array()

	var png := _find_asset("bg_day_character.png")
	var jpg := _find_asset("a_reincarnation_of_a_scattering_spring.jpg")
	print("[mixed] png=", png, " jpg=", jpg)

	if png.is_empty() and jpg.is_empty():
		_fail("找不到测试图片（tests/assets 或 Milestro/tests/data/image）")
		return

	if not png.is_empty():
		img_paths.push_back(png)
		img_pos.push_back(Vector2(24, 100))
		img_h.push_back(96.0)
	if not jpg.is_empty():
		img_paths.push_back(jpg)
		img_pos.push_back(Vector2(160, 100))
		img_h.push_back(96.0)

	# --- 混排渲染 ---
	var metrics: Dictionary = m.render_mixed(
		SAMPLE_TEXT,
		families,
		FONT_SIZE,
		CANVAS_W,
		CANVAS_H,
		img_paths,
		img_pos,
		img_h
	)
	print("[mixed] metrics=", metrics)

	if not metrics.get("ok", false):
		_fail("render_mixed 失败 code=%s" % str(metrics.get("code", "?")))
		return

	var height := float(metrics.get("height", 0.0))
	var longest := float(metrics.get("longest_line", 0.0))
	var nonzero := int(metrics.get("non_zero_pixels", 0))
	var drawn: int = int(metrics.get("images_drawn", 0))

	if height <= 0.0:
		_fail("文字排版 height=0")
		return
	if drawn < 1:
		_fail("没有成功绘制任何图片 images_drawn=%d" % drawn)
		return
	if nonzero <= 0:
		_fail("像素全空")
		return

	var img: Image = metrics.get("image", null)
	if img == null or img.is_empty():
		_fail("Image 为空")
		return

	preview.texture = ImageTexture.create_from_image(img)
	img.save_png("user://milestro_mixed_test.png")
	# 也写到项目可检查的位置（headless 验证）
	var abs_out := ProjectSettings.globalize_path("user://milestro_mixed_test.png")
	print("[mixed] saved ", abs_out)

	var has_emoji := SAMPLE_TEXT.contains("🎉") or SAMPLE_TEXT.contains("😀")
	_pass(
		"PASS ver=%s.%s.%s text_h=%.1f longest=%.1f non_zero=%d images=%d emoji_in_text=%s" % [
			ver.major, ver.minor, ver.patch, height, longest, nonzero, drawn, str(has_emoji)
		]
	)


func _pass(msg: String) -> void:
	status_label.text = "PASS"
	status_label.add_theme_color_override("font_color", Color(0.2, 0.85, 0.35))
	detail_label.text = msg
	print("[mixed] ", msg)


func _fail(msg: String) -> void:
	status_label.text = "FAIL"
	status_label.add_theme_color_override("font_color", Color(1.0, 0.35, 0.3))
	detail_label.text = msg
	push_error("[mixed] " + msg)


func _find_cjk_font() -> String:
	var env := OS.get_environment("MILESTRO_FONT_PATH")
	if not env.is_empty() and FileAccess.file_exists(env):
		return env
	for c in [
		"/Users/JimmyXiao/Milestro/tests/data/font/SourceHanSans-VF.otf.woff2.bytes",
		ProjectSettings.globalize_path("res://tests/fonts/SourceHanSans.otf"),
	]:
		if FileAccess.file_exists(c):
			return c
	return ""


func _find_emoji_font() -> String:
	var env := OS.get_environment("MILESTRO_EMOJI_FONT_PATH")
	if not env.is_empty() and FileAccess.file_exists(env):
		return env
	for c in [
		"/Users/JimmyXiao/Milestro/tests/data/font/NotoColorEmoji.ttf.bytes",
		ProjectSettings.globalize_path("res://tests/fonts/NotoColorEmoji.ttf"),
		"/System/Library/Fonts/Apple Color Emoji.ttc",
	]:
		if FileAccess.file_exists(c):
			return c
	return ""


func _find_asset(name: String) -> String:
	var res := "res://tests/assets/" + name
	var g := ProjectSettings.globalize_path(res)
	if FileAccess.file_exists(g):
		return g
	var upstream := "/Users/JimmyXiao/Milestro/tests/data/image/" + name
	if FileAccess.file_exists(upstream):
		return upstream
	return ""
